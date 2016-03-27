// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigSortObject.h"

using std::string;
using std::vector;

TigSortObject::TigSortObject() 
  : TigDataObject()
{
  mWhatToDo = -1;
  ObjType = "Formula" ;
}


/*
void
TigSortObject::Clear()
{
  std::cout << "[TigSortObject::Clear] " << mName << std::endl;
  this->TigDataObject::Clear();
}
*/

bool
TigSortObject::Evaluate()
{
  //  std::cout << "[TigSortObject::Evaluate] " << mName << std::endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;

  switch (mWhatToDo){
  case TFORMULA:
    return true;
  case MAXVALUE: {
  for (int i = 0; i<InputData.size(); i++ ) {
    if ( *(InputUpdated.at(i)) == false ) return false;
  }
    if (this->InputData.size() != 2 ){
      std::cout << "[TigSortObject::Evaluate] max value wrong input data size" << std::endl;
      return false;
    }
    //  std::cout << "[TigSortObject::Evaluate] 'maxValue' " << mName <<  std::endl;
    double maxCh, maxV;
    maxV = 0;
    for (int j = 0; j < (InputData.at(1))->first; j++) {
      if ((InputData.at(1))->second[j] >= maxV) {
	maxV = (InputData.at(1))->second[j];
	maxCh = (InputData.at(0))->second[j];
      }
    }
    if (maxV>0){
      this->Update(1,&maxCh,&maxV);
    }
  } break;
  case SELECTCHANNEL: {
    for (int i = 0; i<InputData.size(); i++ ) {
      if ( *(InputUpdated.at(i)) == false ) return false;
    }
    if (this->InputData.size() != 2 ){
      std::cerr << "[TigSortObject::Evaluate] 'select' wrong input data size" << std::endl;
      return false;
    }
    //    std::cout << "[TigSortObject::Evaluate] " << mName << " 'select' " << std::endl;
    double channel[mParameters.size()], value[mParameters.size()];
    int counter = 0;
    for (int i=0; i<(InputData.at(0))->first; i++){
      //      std::cout << "[TigSortObject::Evaluate] " << mName << " 'select' loop " << i << " " << mInputNames.at(i) << " size " << (InputData.at(0))->first << std::endl;
      std::set<int>::iterator iter;
      iter = mChannels.find( (InputData.at(0))->second[i] );
      if (iter != mChannels.end()) {
	channel[ counter ] =(InputData.at(0))->second[i];
	value[ counter ] =(InputData.at(1))->second[i];
	counter++;
      }
    }
    this->Update(counter, channel, value);
  } break;
  case COMBINE: {
    bool updated = false;
    for (int i = 0; i<InputData.size(); i++ ) {
      if ( *(InputUpdated.at(i)) == true ) updated = true;
    }
    if (!updated) return false;
    int inPerInput = InputData.size()/mInputNames.size();
    //    std::cout << "[TigSortObject::Evaluate] 'combine' " << mName  << " " <<  inPerInput << " inputs" << std::endl;
    vector< vector<double> > combined;
    for (int j=0; j<inPerInput; j++) {
      vector<double> tmp;
        for (int i =j; i<InputData.size(); i=i+inPerInput){
	  if ( *(InputUpdated.at(i)) == true ){
	    for (int k=0; k<(InputData.at(i))->first; k++) tmp.push_back( (double)((InputData.at(i))->second[k]) );
      }    }
    combined.push_back(tmp ); 
}
    int size = combined.at(0).size();
    for (int i=1; i<combined.size(); i++)
      if (size != combined.at(i).size()){
      std::cerr << "[TigSortObject::Evaluate] this did not go well... data size mismatch " << std::endl;
      return false;
    }
    double value0[size];
    for (int i=0; i<combined.at(0).size(); i++) value0[i] = combined[0][i];
    if (combined.size() == 2){
    double value1[size];
    for (int i=0; i<combined.at(1).size(); i++) value1[i] = combined[1][i];
    this->Update(size, value0,value1);
    }
    else this->Update(size,value0);
  } break;
  }
  return true;
}

void 
TigSortObject::IncreaseDataLength(int d)
{
  if (mWhatToDo==COMBINE){
    //  std::cout << "[TigSortObject::IncreaseDataLength] " << mName << " - " << d  << std::endl;
    mDataLength += d;
    this->SetDataLength(mDataLength);
  }
}

bool
TigSortObject::Initialize()
{
  //  std::cout << "[TigSortObject::Initialize] " << mName << std::endl;
  bool check =  this->TigDataObject::Initialize();
  if (!check) return false;
   
  switch (mWhatToDo){
  case TFORMULA:
    return true;
  case MAXVALUE: {
    if (mInputNames.size() != 1) {
      std::cerr << "[TigSortObject::Initialize] maxValue needs exactly one input" << std::endl;
      return false;
    }
    //      this->AddInput( mNeeded.at(0).first , 1);
    this->SetNumData(2);
    this->SetDataLength(1);
    return true;
    break;
  }
  case SELECTCHANNEL: { 
    if (mInputNames.size() != 1) {
      std::cerr << "[TigSortObject::Initialize] selectChannel needs exactly one input" << std::endl;
      return false;
    }
    if (mParameters.size()==0) {
      std::cerr << "[TigSortObject::IniParams] no channels specified to select" << std::endl;
      return false;
    }
    int channel; 
    for (int k=0; k<mParameters.size(); k++) {
      std::istringstream ( mParameters.at(k) ) >> channel;
      mChannels.insert( channel) ;
    }
    //  this->AddInput( mNeeded.at(0).first , 1);
    this->SetNumData(2);
    this->SetDataLength(mChannels.size());
    return true;
    break;
  }
  case COMBINE: {
    if (mInputNames.size() < 2) {
      std::cerr << "[TigSortObject::Initialize] combine requires at least 2 inputs to make sense" << std::endl;
      return false;
    }
    //      this->AddInput( mNeeded.at(0).first , 1);

    int inPerInput = InputData.size()/mInputNames.size();
    if (inPerInput*mInputNames.size() != InputData.size() ){
      std::cerr << "[TigSortObject::Initialize] trying to combine objects with different data sizes. Try choosing _channel/value" << std::endl;
      return false;
    }
    if (inPerInput > 2 ){
      std::cerr << "[TigSortObject::Initialize] Ooops! More that 2 values per object not currently implemented!" << std::endl;
      return false;
    }
    this->SetNumData(inPerInput);
    //    for (int i = 0; i<InputData.size(); i++ ) IncreaseDataLength(InputData.at(i)->first);
    return true;
    break;
  }
  default: {
    std::cerr << "[TigSortObject::Initialize] failed" << std::endl;
    return false;
  }
  }
  return false;
}

bool 
TigSortObject::ParseInput(string line)
{
  //  std::cout << "[TigSortObject::ParseInput] " << mName << std::endl;
  bool result = false;
  string token;
  std::istringstream stream(line.c_str());	
  stream >> token;
  if ( token.compare("parameters") == 0)
    {
      vector<string> parameters;
      while ( !stream.eof() ) {
	parameters.push_back("");
	stream >> parameters.back();
      }
      this->SetParameters(parameters);
      result = true;
    }	  
  else if ( token.compare("what") == 0)
    {
      stream >> token;
      if ( token.compare("maxValue") == 0 ) mWhatToDo = MAXVALUE;
      else if ( token.compare("selectChannel") == 0 ) mWhatToDo = SELECTCHANNEL; 
      else if ( token.compare("combine") == 0 ) mWhatToDo = COMBINE;
      else {
	std::cerr << "[TigSortObject::ParseInput] '" << token << "' unknown" << std::endl;
	return false;
      }
      result = true;
    }
  else result = this->TigDataObject::ParseInput(line);
  return result;
}
