// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigHisto.h"


using std::string;

TigHisto::TigHisto()
  :xBins(0)
  ,yBins(0)
{

}


//---- Initialise
bool
TigHisto::Initialize()
{
  // std::cout << "TigHisto " << mName << " initialising" << std::endl;
  bool check =  this->TigDataObject::Initialize();
   if (!check) return check;
   if (!xBins){
     std::cout << "[TigHisto::Initialize] no xbins set for " << mName << std::endl;
     return false;
   }
    if (mNeeded.size() == 1){
      //  std::cout << mName << " is 1D" << std::endl;
      mWhatToDo = ONE_D;
      mHisto = new TH1F(mName.c_str(),mInputNames.at(0).c_str(),xBins, xMin, xMax);
    } 
    else if (mNeeded.size() == 2){
      //      std::cout << mName << " is 2D" << std::endl;
      if (!yBins){
	std::cout << "[TigHisto::Initialize] no ybins set" << std::endl;
	return false;
      }
      mWhatToDo = TWO_D;
      //      std::cout << "TigHisto " << mName << " test 1" << std::endl;     
      string title = mInputNames.at(1) + " vs " + mInputNames.at(0);
      //std::cout << "TigHisto " << mName << " test 2" << std::endl;     
      mHisto = new TH2F(mName.c_str(),title.c_str(),xBins, xMin, xMax, yBins, yMin, yMax);
      mHisto->SetOption("COLZ");
    }
    else {
      std::cout << "[TigHisto::Initialize] wrong number of parameters: " << mInputNames.size() << std::endl;
      return false;
    }
    //    std::cout << "TigHisto " << mName << " initialised" << std::endl;
     return true;
  }

bool
TigHisto::Evaluate()
{
  //  std::cout << "[TigHisto::Evaluate] " << mName << std::endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;

   int needed = mNeeded.size(); 
  if (  needed != InputData.size() ) {
    std::cout << "[TigHisto::Fill] parameter mismatch: needed " << needed << " received: " << InputData.size() << std::endl;
    return  true;
  }
  
 for (int i = 0; i<needed; i++ ) {
   if ( *(InputUpdated.at(i)) == false ) return false;
  }
  int combinations = 1;
  for (int i = 0; i<InputData.size(); i++)  combinations *= (InputData.at(i))->first;
  //  std::cout << "[TigHisto::Evaluate] # combinations " << combinations << std::endl;;
  if (combinations){
  double *pars;
  pars = new double[combinations];
   ::memset(pars, 0,  combinations * sizeof(double));

  // std::cout << " before:  " << std::endl; ;
  // for (int j = 0; j<needed; j++)  std::cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << std::endl;
  // std::cout << std::endl;

  double sorted[combinations][needed];
  for (int i = 0; i<needed; i++ ) {
     int remainComb = 1;
     for (int j = i+1; j<needed; j++)  remainComb *= (InputData.at(j))->first;
    for (int j = 0; j < (InputData.at(i))->first; j++) {
      int oncount = 0;
      int iniOff = remainComb * j;
      int offcount = remainComb * (  (InputData.at(i))->first  - 1 );
      for (int k = iniOff; k<combinations; k++){
	bool fill = false;
	if (oncount < remainComb) fill = true;
	if (fill){
	  sorted[k][i] =  (InputData.at(i))->second[j];
	oncount++;
	if (oncount == remainComb) offcount = remainComb * ( (InputData.at(i))->first -1 ) ;
	}
	else {
	  offcount--;
	  if (offcount == 0) oncount = 0;
	}
      }
    }
  }
  for (int i=0; i<combinations; i++) {
    if (mWhatToDo == TWO_D )  mHisto->Fill(sorted[i][0],sorted[i][1]);
    else if (mWhatToDo == ONE_D )  mHisto->Fill(sorted[i][0]);
  }    
          // std::cout << "after:  " << std::endl; ;
  // for (int j = 0; j<needed; j++)  std::cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << std::endl;
  // std::cout << std::endl;
  }
  // std::cout << "[TigHisto::Evaluate] done" << std::endl;
  return true;
}

bool 
TigHisto::ParseInput(string line)
{
  bool result = false;
  string token;
  std::istringstream stream(line.c_str());	
  stream >> token;

  if ( token.compare("xbins") == 0)
    {
          int bins;
          double min, max;
          stream >> bins >> min >> max;
	  this->SetXBinning(bins,min,max);
	  result = true;
    }
  else if ( token.compare("ybins") == 0)
    {
      int bins;
      double min, max;
      stream >> bins >> min >> max;
      this->SetYBinning(bins,min,max);
      result = true;
    }
  else result = this->TigDataObject::ParseInput(line);
  return result;
}




//---- SetXBinning
void 
TigHisto::SetXBinning(int bins,double min,double max)
{
  //  std::cout << "TigHisto " << mName << " x binning " << bins << ", " << min << ", " << max << std::endl;
  xBins = bins;
  xMin = min;
  xMax = max;
}

//---- SetYBinning
void 
TigHisto::SetYBinning(int bins,double min,double max)
{
  //  std::cout << "TigHisto " << mName << " y binning " << bins << ", " << min << ", " << max << std::endl;
  yBins = bins;
  yMin = min;
  yMax = max;
}

