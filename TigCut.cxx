// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TROOT.h>
#include <TigCut.h>


TigCut::TigCut()
  :mCut(NULL)
{
  ObjType = "Formula" ;
}

TigCut::~TigCut()
{
  if (mCut) delete mCut;

}


bool
TigCut::Evaluate()
{
  //  cout << "[TigCut::Evaluate] " << mName << endl;
  *IsUpdated = false;
  for (int i = 0; i<InputData.size(); i++ ) {
    if (  *(InputUpdated.at(i)) == false ) return false;
    if ((InputData.at(i))->first != 1) return false;
  }
  int passed = 0;
  switch (mWhatToDo){
  case TCUTG:{
    if (mCut->IsInside((InputData.at(0))->second[0],(InputData.at(1))->second[0] ) ){
    passed = 1;
  }break;
    case RANGECUT: {
      if ( ( (InputData.at(0))->second[0] >= mMinRange ) && (  (InputData.at(0))->second[0] <= mMaxRange )   ) passed = 1;
    }break;
  }
  }
  this->Update(1,&passed);
  return true;
}

//---- Initialize
bool
TigCut::Initialize()
{
  //  cout << "[TigCut::Initialize] " << mName << endl;
  bool check =  this->TigDataObject::Initialize();
  switch (mWhatToDo){
  case TCUTG:{
  if (mNeeded.size() != 2){
    cout << "[TigCut::Initialize] TCutG " << mName <<" needs 2 inputs, found " << mNeeded.size() << endl;
    return false;
  }
  string processLine = ".x " + mFile;
  gROOT->ProcessLine(processLine.c_str());
  mCut = (TCutG*)gROOT->FindObjectAny(mCutName.c_str());
  if (!mCut) {
    cout << "[TigCut::Initialize] no cut " << mCutName << endl;
    return false;
  }
    }break;
  case RANGECUT:{
    if (mNeeded.size() != 1){
      cout << "[TigCut::Initialize] " << mName <<  " cut needs 1 inputs" << endl;
      return false;
  }
      if (mMaxRange < mMinRange){
      cout << "[TigCut::Initialize] " << mName <<  " maximum range smaller than minimum" << endl;
      return false;
  }

  } break;
  }
  this->SetNumData(1);
  this->SetDataLength(1);

  return true;
}

bool 
TigCut::ParseInput(string line)
{
  bool result = false;
  string token;
  istringstream stream(line.c_str());	
  stream >> token;
  if (token.compare("tcutg") ==0){
    string file, object;
    stream >> file >> object;
    this->SetCutName(file,object);
    result = true;
  }
  else if (token.compare("range") ==0){
    double min, max;
    stream >> min >> max;
    this->SetRange(min,max);
    result = true;
  }

  else result = this->TigDataObject::ParseInput(line);
  return result;
}



//---- SetCutName
void
TigCut::SetCutName(string pFile, string pObject)
{
  mFile = pFile;
  mCutName = pObject;
  mWhatToDo = TCUTG;
}


 void 
 TigCut::SetRange(double min, double max)
 {
   mMinRange = min;
   mMaxRange = max;
   mWhatToDo = RANGECUT;
 }
