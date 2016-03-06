// part of TigSortGUI
// author: Ulrike Hager

#include <TigTree.h>
#include <TigWaveformHisto.h>

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
    cout << "[TigWaveformHisto::Initialize] " << mName << " too many inputs" << endl;
    return false;
  }
  if (mAddress < 0) mAddress = mParent->GetAddress(mInputNames.at(0), mChannel);
  if (mAddress < 0) {
    cout << "[TigWaveformHisto::Initialize] " << mName << " can't find address for input " << mInputNames.at(0) << " ch " << mChannel <<  endl;
    return false;
  }
  mHisto = new TH1F(mName.c_str(),mName.c_str(),mXbins, 0, mXbins);
  return true;
}

//---- ParseInput
bool
TigWaveformHisto::ParseInput(string line)
{
  bool result = true;
  string token;
  istringstream stream(line.c_str());	
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
      stream >> hex  >> mAddress;
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
