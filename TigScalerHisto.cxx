// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TigTree.h>
#include <TigScalerHisto.h>


TigScalerHisto::TigScalerHisto()
  :TigHistoObject()
  ,xMax(20000)
 
{
  //  mHisto = NULL;
  mWhatToDo = SCALER_HISTO;
}

TigScalerHisto::~TigScalerHisto()
{
  //  if (mHisto) delete mHisto;
}

//---- Initialise
bool
TigScalerHisto::Initialize()
{
  // cout << "TigScalerHisto " << mName << " initialising" << endl;
  bool check =  this->TigDataObject::Initialize();
   if (!check) return check;
   if (!xMax){
     cout << "[TigScalerHisto::Initialize] no maximum x set for " << mName << endl;
     return false;
   }
    if (mNeeded.size() != 1){
      cout << "[TigScalerHisto::Initialize] too many inputs for " << mName << endl;
      return false;
   }
    mChannel = mParent->GetScalerIndex(mChannelName);
    if (mChannel < 0 ){
      cout << "[TigScalerHisto::Initialize]  scaler " << mChannelName << " not found for " << mName << endl;
      return false;
   }
    mHisto = new TH1F(mName.c_str(),mChannelName.c_str(),xMax, 0, xMax);
      //      cout << "TigScalerHisto " << mName << " test 1" << endl;     
      //cout << "TigScalerHisto " << mName << " test 2" << endl;     
    //   cout << "TigScalerHisto " << mName << " initialised" << endl;
     return true;
  }

bool
TigScalerHisto::Evaluate()
{
  //  cout << "[TigScalerHisto::Evaluate] " << mName << endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;

   int needed = mNeeded.size(); 
  if (  needed != InputData.size() ) {
    cout << "[TigScalerHisto::Fill] parameter mismatch: needed " << needed << " received: " << InputData.size() << endl;
    return  true;
  }
  if (  mChannel  > (InputData.at(0))->first ) {
    cout << "[TigScalerHisto::Fill] needed channel  " << mChannel << " out of range of input data for" << mName << endl;
    return  true;
  }
  
 for (int i = 0; i<needed; i++ ) {
   if ( *(InputUpdated.at(i)) == false ) return false;
  }
  // cout << " before:  " << endl; ;
 //for (int j = 0; j<needed; j++)  cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << endl;
 // cout << endl;

 mHisto->Fill(mParent->ScEventID(),(InputData.at(0))->second[mChannel]);
          // cout << "after:  " << endl; ;
  // for (int j = 0; j<needed; j++)  cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << endl;
  // cout << endl;
 // cout << "[TigScalerHisto::Evaluate] done" << endl;
  return true;
}

bool 
TigScalerHisto::ParseInput(string line)
{
  bool result = false;
  string token;
  istringstream stream(line.c_str());	
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
  //  cout << "TigScalerHisto " << mName << " x binning " << bins << ", " << min << ", " << max << endl;
  xMax = max;
}

