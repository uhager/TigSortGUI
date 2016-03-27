// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigTree.h"
#include "TigDataObject.h"

using std::string;
using std::pair;
using std::vector;

TigDataObject::TigDataObject()
  : TigObject()
  ,mParent(NULL)
  ,mDataLength(0)
  ,mWhatToDo(-1)
  ,SizeDetermined(false)
  ,mWriteToTree(true)
{
  //  std::cout << "[TigDataObject::TigDataObject]" << std::endl;
  IsUpdated = new bool;
  *IsUpdated = false;
  ObjType = "DataObject";
  // std::cout << "[TigDataObject::TigDataObject] done" << std::endl;
}


TigDataObject::TigDataObject(int num)
  : TigObject()
  ,mDataLength(0)
  ,mParent(NULL)
  ,mWhatToDo(-1)
  ,SizeDetermined(false)
  ,mWriteToTree(true)
{
  this->SetNumData(num);
  IsUpdated = new bool;
  *IsUpdated = false;
  ObjType = "DataObject";
  // std::cout << "[TigDataObject::TigDataObject] done" << std::endl;
}

TigDataObject::~TigDataObject()
{
  //  std::cout << "[TigDataObject::~TigDataObject] " << mName << std::endl;
  for (int i=Data.size()-1; i>=0; i--) {
    delete Data.at(i)->second;
    delete Data.at(i);
  }
  Data.clear();
  CutData.clear();
  InputData.clear();
  delete IsUpdated;
}

void
TigDataObject::AddCut(string name)
{
  pair<string,int> par;
  par.first  = name;
  par.second = 0;
  //    std::cout << "[TigDataObject::AddCut] " << mName << " cut "   << name << "  channel/value " << par.second   << std::endl;
  mNeededCuts.push_back(par);
  CutData.push_back(NULL);
}

void
TigDataObject::AddInput(string name, int id)
{
  pair<string,int> par;
  par.first  = name;
  par.second = id;
  //    std::cout << "[TigDataObject::AddInput]  " << mName  << " input " << par.first << "  channel/value " << par.second   << std::endl;
  mNeeded.push_back(par);
  InputUpdated.push_back(NULL);
  InputData.push_back(NULL);
}


void
TigDataObject::Clear()
{
  // std::cout << "[TigDataObject::Clear] " << mName << std::endl;
  for (int i=0; i<Data.size(); i++) {
    Data.at(i)->first =0 ;
    Data.at(i)->second[0] =0 ;
  }
  *IsUpdated = false;
}

int
TigDataObject::DataLength()
{
  return mDataLength;
}


int
TigDataObject::DataSize()
{
  //  std::cout << "[TigDataObject::DataSize] " << mName  << " - " << Data.size() << std::endl;
  return Data.size(); 
}

bool
TigDataObject::Evaluate()
{
  //  std::cout << "[TigDataObject::Evaluate] " << mName << endl ;
  if (ObjType == "Detector")     return true; 
  else if (ObjType == "DataObject") {
    if ((Data.size() == 1) && (mDataLength == 1)){
      Data.at(0)->first = 1;
      *IsUpdated = true;
      return true;      
    }
    else if (Data.size()){
      if ( (Data.at(0))->first > 0) {
	//	std::cout << "[TigDataObject::Evaluate] " << mName << " " <<  (Data.at(0))->first ;
	for (int i=1;i<Data.size(); i++) Data.at(i)->first = Data.at(0)->first ;
	//	std::cout << " 1st entry = " << Data.at(0)->second[0] <<  std::endl;
	*IsUpdated = true;
	return true;
      }}
    else return false;
  }
  *IsUpdated = false;
  for (int i = 0; i<CutData.size(); i++) {
    if ( (CutData.at(i))->first == 0 ) return false;
    if ( (CutData.at(i))->second[0] == 0 ) return false;
  }
  return true;
}

bool
TigDataObject::IniInputs()
{
  //  std::cout << "[TigDataObject::IniInputs] " << mName << std::endl;
  for (int i=0; i<mInputNames.size(); i++){
    string parN = mInputNames.at(i);
    size_t pos;
    if ( parN.find("_channel") != string::npos ) {
      pos = parN.find("_channel");
      AddInput(parN.substr(0,pos),0);
    }
    else if ( parN.find("_value") != string::npos ) {
      pos = parN.find("_value");
      AddInput(parN.substr(0,pos),1);
    }
    else {
      int inputSize = mParent->GetDataSize(parN);
      if (inputSize <1 ) {
      	std::cout << "[TigDataObject::IniInputs] input " << parN << " not found for object " << mName << std::endl;
      	return false;
      }
      else {
	for (int j =0; j<inputSize; j++){
	  AddInput(parN,j);
	}
      }
    }
    //    std::cout << "[TigFormula::Initialize] parameter " << i << "  channel/value " << par->second   << std::endl;
  }
  for (int i=0; i<mCutNames.size(); i++) AddCut(mCutNames.at(i));
   return true;
}


bool
TigDataObject::Initialize()
{
  //  std::cout << "[TigDataObject::Initialize]  " << this->Name() << std::endl; 
  bool check;
   check = mParent->FindObject(mName);
  if (check) {
     std::cout << "[TigDataObject::Initialize] object with name " << mName << " exists. Deleting new object." << std::endl;
    return false;
  }
  check = this->IniInputs();
  //  std::cout << "[TigDataObject::Initialize]  " << this->Name() << " initialized " << std::endl; 
  return check;
}

bool
TigDataObject::ParseInput(string line)
{
  //  std::cout << "[TigDataObject::ParseInput] " << mName << std::endl;
  string token;
  std::istringstream stream(line.c_str());	
  stream >> token;
  if ( token == "" || token[0] == '#') { }  //comment or blank
  else if ( token.compare("inputs") == 0)
    {
      vector<string> parameters;
      while ( !stream.eof() ) {
	parameters.push_back("");
	stream >> parameters.back();
      }
      this->SetInputNames(parameters);
      return true;
    }   
  else if ( token.compare("cuts") == 0)
    {
      vector<string> parameters;
      while ( !stream.eof() ) {
	parameters.push_back("");
	stream >> parameters.back();
      }
      this->SetCutNames(parameters);
      return true;
    }
  else {
    //	 std::cout << "[TigDataObject::ParseInput] unknown token " << token << std::endl;
    return this->TigObject::ParseInput(line);
  }
  return true;
}

void
TigDataObject::Reset()
{
}

void
TigDataObject::SetDataLength( int hits)
{
  //std::cout << "[TigDataObject::SetDataLength] "  << mName << " - " << hits<< std::endl;
  if (hits>gMaxEntries){
    for (int i=0; i<Data.size(); i++) {
      if (Data.at(i)->second) {
	delete Data.at(i)->second;
	Data.at(i)->second = NULL;
      }
      Data.at(i)->second = new double[hits];
      //    memset(Data.at(i)->second, 0, gMaxEntries*sizeof(int));
    }
  }
  mDataLength = hits;
  this->Clear();
}

void
TigDataObject::SetNumData(int num)
{
  // std::cout << "[TigDataObject::SetNumData] " << mName << " - " << num << std::endl;
  for (int i=Data.size()-1; i>=0; i--) {
    delete Data.at(i)->second;
    delete Data.at(i);
  }
  Data.clear();
  for (int i=0; i<num; i++) {
    pair<int,double*> *toAdd1 = new pair<int,double*>;
    toAdd1->first = 0;
    if (mDataLength > gMaxEntries) toAdd1->second = new double[mDataLength];
    else toAdd1->second = new double[gMaxEntries];
    Data.push_back( toAdd1 );
  }

  //    std::cout << "[TigDataObject::SetNumData] data size " << Data.Size() << std::endl;    
}


void
TigDataObject::Update(int hits, int* channel, int *value)
{
  if (Data.size() <2 ){
    std::cout << "[TigDataObject::Update] wrong data size " << mName<< std::endl;
    return;
  }
  if (mDataLength <hits ){
    //std::cout << "[TigDataObject::Update] too many hits "<< mName << ": " << hits << " of " << mDataLength << std::endl;
    SetDataLength(hits);
  }
  for (int i=0; i<Data.size(); i++) {
    Data.at(i)->first =hits;  
  }
  std::copy(channel, channel+hits, Data.at(0)->second);
  std::copy(value, value+hits, Data.at(1)->second);
  *IsUpdated = true;
}

void
TigDataObject::Update(int hits, double* channel, double *value)
{
  if (Data.size() <2 ){
    std::cout << "[TigDataObject::Update] wrong data size " << mName << std::endl;
    return;
  }
  if (mDataLength <hits ){
    // std::cout << "[TigDataObject::Update] too many hits " << mName   << ": " << hits << " of " << mDataLength << std::endl;
    SetDataLength(hits);
  }
  for (int i=0; i<Data.size(); i++) {
    Data.at(i)->first =hits;  
  }
  std::copy(channel, channel+hits, Data.at(0)->second);
  std::copy(value, value+hits, Data.at(1)->second);
  *IsUpdated = true;
}

void
TigDataObject::Update(int hits, int *value)
{
  if (Data.size() < 1 ){
    std::cout << "[TigDataObject::Update] wrong data size " << mName << std::endl;
    return;
  }
  if (mDataLength <hits ){
    //    std::cout << "[TigDataObject::Update] too many hits " << mName<< std::endl;
    SetDataLength(hits);
  }
  Data.at(0)->first = hits;
  std::copy(value, value+hits, Data.at(0)->second);

  *(this->IsUpdated) = true;
}

void
TigDataObject::Update(int hits, double *value)
{
  if (Data.size() < 1 ){
    std::cout << "[TigDataObject::Update] wrong data size " << mName << std::endl;
    return;
  }
  if (mDataLength <hits ){
    // std::cout << "[TigDataObject::Update] too many hits " << mName   << ": " << hits << " of " << mDataLength << std::endl;
    SetDataLength(hits);
  }
  // std::cout << "[TigDataObject::Update] before: hits " << Data.at(0)->first  << " - &Data.at(0)->first " << &(Data.at(0)->first)  << " - Data.at(0)->second " << Data.at(0)->second  << std::endl;
  Data.at(0)->first = hits;
  std::copy(value, value+hits, Data.at(0)->second);
  *(this->IsUpdated) = true;
  //  std::cout << "[TigDataObject::Update] after: hits " << Data.at(0)->first  << " - &Data.at(0)->first " << &(Data.at(0)->first)  << " - Data.at(0)->second " << Data.at(0)->second  << std::endl;
}

/*
  int
  TigDataObject::Size()
  {
  return Data.size();
  }
*/

