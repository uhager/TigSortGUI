// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigCalibration.h"

TigCalibration::TigCalibration()
  :  TigDataObject(2)
     //  ,mCalibration(NULL)
{
  mName = "calibration";
  mWhatToDo = CALIBRATION;
  ObjType = "Formula";
}



//--- Evaluate
bool
TigCalibration::Evaluate()
{
  //  std::cout << "[TigCalibration::Evaluate] " << mName << " - max " << mCalibration.size() <<  " channels " << std::endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;
  for (int i = 0; i<InputData.size(); i++ ) {
    if ( *(InputUpdated.at(i)) == false ) return false;
  }
  if (this->InputData.size() != 2 ){
    std::cout << "[TigCalibration::Evaluate] wrong input data size" << std::endl;
    return false;
  }
    double channels[mCalibration.size()], values[mCalibration.size()];
    int counter = 0;
    for (int i=0; i<(InputData.at(0))->first; i++){
      //      std::cout << "[TigCalibration::Evaluate] " << mName << " " << i << " of " << (InputData.at(0))->size() << " input values " << std::endl;
	    int channel = (InputData.at(0))->second[i];
	    if (mCalibration.find(channel) != mCalibration.end()) {
	      //	     std::cout << "[TigCalibration::Evaluate] " << mName << "-" << channel << ": " << counter <<  " of " << mCalibration.size() ; 
	      double value = (InputData.at(1))->second[i];
	      //	      std::cout << " ADC value " << value ;
	      values[counter] =  ( value * mCalibration[ channel ].first ) + mCalibration[ channel ].second;
	      //	      std::cout << " -> " << values[counter] << std::endl; 
	      channels[counter] = (InputData.at(0))->second[i];
	      //	      std::cout << " next up: " << counter <<std::endl;
	      counter++;
	    }
    }
    if (counter>0) this->Update(counter, channels, values);
}

void 
TigCalibration::IncreaseDataLength(int d)
{
    mDataLength += d;
    this->SetDataLength(mDataLength);
}

bool
TigCalibration::Initialize()
{
  // std::cout << "[TigFormula::Initialize] " << mName << std::endl;
  if (mName.compare("calibration") == 0 ) mName = mInputNames.at(0) + "_calib";
  bool check =  this->TigDataObject::Initialize();
  //  SetNumData(2);
  return check;
}

//--- ParseInput
bool
TigCalibration::ParseInput(std::string line)
{
  bool result = false;
  std::string token;
  int channel;
  std::istringstream stream(line.c_str());	
  if (stream >> channel )
    {
      double gain, offset;
      stream >> gain >> offset;
      mCalibration[ channel ] = std::make_pair(gain,offset);   
      //      std::cout << "[TigCalibration::ParseInput] " << mInputNames.at(0) << ": " << channel << " - " << mCalibration[channel].first << " xCh + " << offset << std::endl; 
      result = true;
    }
  else result = this->TigDataObject::ParseInput(line);
  return result;
}
