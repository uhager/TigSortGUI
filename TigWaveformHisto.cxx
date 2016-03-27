// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigTree.h"
#include "TigWaveformHisto.h"


using std::vector;

TigWaveformHisto::TigWaveformHisto()
{
  mAddress = -1;
  mChannel = -1;
}


bool 
TigWaveformHisto::Evaluate(vector<short> pWf)
{
  for (int i =0; i<pWf.size(); i++) mHisto->SetBinContent(i,pWf.at(i));
  return true;
}


//---- Initialize
bool
TigWaveformHisto::Initialize()
{

  bool check = this->TigDataObject::Initialize();
  if (!check) return false;
  if (mInputNames.size() > 1){
    std::cout << "[TigWaveformHisto::Initialize] " << mName << " too many inputs" << std::endl;
    return false;
  }
  if (mAddress < 0) mAddress = mParent->GetAddress(mInputNames.at(0), mChannel);
  if (mAddress < 0) {
    std::cout << "[TigWaveformHisto::Initialize] " << mName << " can't find address for input " << mInputNames.at(0) << " ch " << mChannel <<  std::endl;
    return false;
  }
  mHisto = new TH1F(mName.c_str(),mName.c_str(),mXbins, 0, mXbins);
  return true;
}

//---- ParseInput
bool
TigWaveformHisto::ParseInput(std::string line)
{
  bool result = true;
  std::string token;
  std::istringstream stream(line.c_str());	
  stream >> token;

  if ( token.compare("channel") == 0)
    {
      stream >> mChannel;
    }
  else if ( token.compare("xbins") == 0)
    {
      stream >> mXbins;
    }
  else if ( token.compare("address") == 0)
    {
stream >> std::hex  >> mAddress;
    }

  else result = this->TigDataObject::ParseInput(line);
  return result; 
}

bool 
TigWaveformHisto::ProcessSignal(TigEvent* pEvent)
{
  bool result = false; 
  if (pEvent->Address() == mAddress )
    {
      // vector<short> wf = pEvent->Waveform();
      // for (int i =0; i<wf.size(); i++) mHisto->SetBinContent(i,wf.at(i));
      result = true;
    }
  return result;
}
