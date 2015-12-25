// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TigCalibration.h>

TigCalibration::TigCalibration()
  :  TigDataObject(2)
     //  ,mCalibration(NULL)
{
  mName = "calibration";
  mWhatToDo = CALIBRATION;
  ObjType = "Formula";
}

TigCalibration::~TigCalibration()
{
  //  cout << "[TigCalibration::~TigCalibration] " << mName << endl;

  // cout << "[TigCalibration::~TigCalibration] done" << endl;
}

//--- Evaluate
bool
TigCalibration::Evaluate()
{
  //  cout << "[TigCalibration::Evaluate] " << mName << " - max " << mCalibration.size() <<  " channels " << endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;
  for (int i = 0; i<InputData.size(); i++ ) {
    if ( *(InputUpdated.at(i)) == false ) return false;
  }
  if (this->InputData.size() != 2 ){
    cout << "[TigCalibration::Evaluate] wrong input data size" << endl;
    return false;
  }
    double channels[mCalibration.size()], values[mCalibration.size()];
    int counter = 0;
    for (int i=0; i<(InputData.at(0))->first; i++){
      //      cout << "[TigCalibration::Evaluate] " << mName << " " << i << " of " << (InputData.at(0))->size() << " input values " << endl;
	    int channel = (InputData.at(0))->second[i];
	    if (mCalibration.find(channel) != mCalibration.end()) {
	      //	     cout << "[TigCalibration::Evaluate] " << mName << "-" << channel << ": " << counter <<  " of " << mCalibration.size() ; 
	      double value = (InputData.at(1))->second[i];
	      //	      cout << " ADC value " << value ;
	      values[counter] =  ( value * mCalibration[ channel ].first ) + mCalibration[ channel ].second;
	      //	      cout << " -> " << values[counter] << endl; 
	      channels[counter] = (InputData.at(0))->second[i];
	      //	      cout << " next up: " << counter <<endl;
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
  // cout << "[TigFormula::Initialize] " << mName << endl;
  if (mName.compare("calibration") == 0 ) mName = mInputNames.at(0) + "_calib";
  bool check =  this->TigDataObject::Initialize();
  //  SetNumData(2);
  return check;
}

//--- ParseInput
bool
TigCalibration::ParseInput(string line)
{
  bool result = false;
  string token;
  int channel;
  istringstream stream(line.c_str());	
  if (stream >> channel )
    {
      double gain, offset;
      stream >> gain >> offset;
      mCalibration[ channel ] = make_pair(gain,offset);   
      //      cout << "[TigCalibration::ParseInput] " << mInputNames.at(0) << ": " << channel << " - " << mCalibration[channel].first << " xCh + " << offset << endl; 
      result = true;
    }
  else result = this->TigDataObject::ParseInput(line);
  return result;
}
