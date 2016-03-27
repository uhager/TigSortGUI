// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigTree.h"
#include "TigScalerHisto.h"


using std::string;

TigScalerHisto::TigScalerHisto()
  :TigHistoObject()
  ,xMax(20000)
 
{
  //  mHisto = NULL;
  mWhatToDo = SCALER_HISTO;
}


//---- Initialise
bool
TigScalerHisto::Initialize()
{
  // std::cout << "TigScalerHisto " << mName << " initialising" << std::endl;
  bool check =  this->TigDataObject::Initialize();
   if (!check) return check;
   if (!xMax){
     std::cout << "[TigScalerHisto::Initialize] no maximum x set for " << mName << std::endl;
     return false;
   }
    if (mNeeded.size() != 1){
      std::cout << "[TigScalerHisto::Initialize] too many inputs for " << mName << std::endl;
      return false;
   }
    mChannel = mParent->GetScalerIndex(mChannelName);
    if (mChannel < 0 ){
      std::cout << "[TigScalerHisto::Initialize]  scaler " << mChannelName << " not found for " << mName << std::endl;
      return false;
   }
    mHisto = new TH1F(mName.c_str(),mChannelName.c_str(),xMax, 0, xMax);
      //      std::cout << "TigScalerHisto " << mName << " test 1" << std::endl;     
      //std::cout << "TigScalerHisto " << mName << " test 2" << std::endl;     
    //   std::cout << "TigScalerHisto " << mName << " initialised" << std::endl;
     return true;
  }

bool
TigScalerHisto::Evaluate()
{
  //  std::cout << "[TigScalerHisto::Evaluate] " << mName << std::endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;

   int needed = mNeeded.size(); 
  if (  needed != InputData.size() ) {
    std::cout << "[TigScalerHisto::Fill] parameter mismatch: needed " << needed << " received: " << InputData.size() << std::endl;
    return  true;
  }
  if (  mChannel  > (InputData.at(0))->first ) {
    std::cout << "[TigScalerHisto::Fill] needed channel  " << mChannel << " out of range of input data for" << mName << std::endl;
    return  true;
  }
  
 for (int i = 0; i<needed; i++ ) {
   if ( *(InputUpdated.at(i)) == false ) return false;
  }
  // std::cout << " before:  " << std::endl; ;
 //for (int j = 0; j<needed; j++)  std::cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << std::endl;
 // std::cout << std::endl;

 mHisto->Fill(mParent->ScEventID(),(InputData.at(0))->second[mChannel]);
          // std::cout << "after:  " << std::endl; ;
  // for (int j = 0; j<needed; j++)  std::cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << std::endl;
  // std::cout << std::endl;
 // std::cout << "[TigScalerHisto::Evaluate] done" << std::endl;
  return true;
}

bool 
TigScalerHisto::ParseInput(string line)
{
  bool result = false;
  string token;
std::istringstream stream(line.c_str());	
  stream >> token;

  if ( token.compare("maxbins") == 0)
    {
          int bins;
          stream >> bins;
	  this->SetXMax(bins);
	  result = true;
	}
  else if ( token.compare("channel") == 0)
    {
      string channel ;
      stream >> channel;
      mChannelName = channel;
      return true;
    }
  else result = this->TigDataObject::ParseInput(line);
  return result;
}


//---- SetXBinning
void 
TigScalerHisto::SetXMax(int max)
{
  //  std::cout << "TigScalerHisto " << mName << " x binning " << bins << ", " << min << ", " << max << std::endl;
  xMax = max;
}

