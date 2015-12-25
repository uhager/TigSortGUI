// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TigTree.h>
#include <TigDataObject.h>

TigDataObject::TigDataObject()
  : TigObject()
 ,mParent(NULL)
  ,mDataLength(0)
  ,mWhatToDo(-1)
  ,SizeDetermined(false)
  ,mWriteToTree(true)
{
  //  cout << "[TigDataObject::TigDataObject]" << endl;
  IsUpdated = new bool;
  *IsUpdated = false;
  ObjType = "DataObject";
  // cout << "[TigDataObject::TigDataObject] done" << endl;
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
  // cout << "[TigDataObject::TigDataObject] done" << endl;
}

TigDataObject::~TigDataObject()
{
  //  cout << "[TigDataObject::~TigDataObject] " << mName << endl;
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
    //    cout << "[TigDataObject::AddCut] " << mName << " cut "   << name << "  channel/value " << par.second   << endl;
    mNeededCuts.push_back(par);
    CutData.push_back(NULL);
}

void
TigDataObject::AddInput(string name, int id)
{
    pair<string,int> par;
    par.first  = name;
    par.second = id;
    //    cout << "[TigDataObject::AddInput]  " << mName  << " input " << par.first << "  channel/value " << par.second   << endl;
    mNeeded.push_back(par);
    InputUpdated.push_back(NULL);
    InputData.push_back(NULL);
}


void
TigDataObject::Clear()
{
  // cout << "[TigDataObject::Clear] " << mName << endl;
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
  //  cout << "[TigDataObject::DataSize] " << mName  << " - " << Data.size() << endl;
  return Data.size(); 
}

bool
TigDataObject::Evaluate()
{
  //  cout << "[TigDataObject::Evaluate] " << mName << endl ;
  if (ObjType == "Detector")     return true; 
  else if (ObjType == "DataObject") {
    if ((Data.size() == 1) && (mDataLength == 1)){
      Data.at(0)->first = 1;
      *IsUpdated = true;
      return true;      
    }
    else if (Data.size()){
      if ( (Data.at(0))->first > 0) {
	//	cout << "[TigDataObject::Evaluate] " << mName << " " <<  (Data.at(0))->first ;
	for (int i=1;i<Data.size(); i++) Data.at(i)->first = Data.at(0)->first ;
	//	cout << " 1st entry = " << Data.at(0)->second[0] <<  endl;
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
  //  cout << "[TigDataObject::IniInputs] " << mName << endl;
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
      	cout << "[TigDataObject::IniInputs] input " << parN << " not found for object " << mName << endl;
      	return false;
      }
      else {
	for (int j =0; j<inputSize; j++){
	  AddInput(parN,j);
	}
      }
    }
    //    cout << "[TigFormula::Initialize] parameter " << i << "  channel/value " << par->second   << endl;
  }
  for (int i=0; i<mCutNames.size(); i++) AddCut(mCutNames.at(i));
  //    for (int i=0; i<mNeeded.size(); i++) 
  return true;
}


bool
TigDataObject::Initialize()
{
  //  cout << "[TigDataObject::Initialize]  " << this->Name() << endl; 
  bool check;
  //  if (!check) return false;
  check = mParent->FindObject(mName);
  if (check) {
    //    mParent->DeleteObject(mName);
    cout << "[TigDataObject::Initialize] object with name " << mName << " exists. Deleting new object." << endl;
    return false;
  }
  check = this->IniInputs();
  //  cout << "[TigDataObject::Initialize]  " << this->Name() << " initialized " << endl; 
  return check;
}

bool
TigDataObject::ParseInput(string line)
{
  //  cout << "[TigDataObject::ParseInput] " << mName << endl;
  string token;
  istringstream stream(line.c_str());	
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
	 //	 cout << "[TigDataObject::ParseInput] unknown token " << token << endl;
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
  //cout << "[TigDataObject::SetDataLength] "  << mName << " - " << hits<< endl;
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
  // cout << "[TigDataObject::SetNumData] " << mName << " - " << num << endl;
  for (int i=Data.size()-1; i>=0; i--) {
    delete Data.at(i)->second;
    delete Data.at(i);
  }
  Data.clear();
    for (int i=0; i<num; i++) {
      pair<int,double*> *toAdd1 = new pair<int,double*>;
      toAdd1->first = 0;
      //double *tmp = NULL;
      //double tmp[gMaxEntries];
      //toAdd1->second = tmp;
      if (mDataLength > gMaxEntries) toAdd1->second = new double[mDataLength];
      else toAdd1->second = new double[gMaxEntries];
      Data.push_back( toAdd1 );
   }

 //    cout << "[TigDataObject::SetNumData] data size " << Data.Size() << endl;    
}


void
TigDataObject::Update(int hits, int* channel, int *value)
{
  if (Data.size() <2 ){
    cout << "[TigDataObject::Update] wrong data size " << mName<< endl;
    return;
  }
  if (mDataLength <hits ){
    //cout << "[TigDataObject::Update] too many hits "<< mName << ": " << hits << " of " << mDataLength << endl;
    SetDataLength(hits);
  }
  for (int i=0; i<Data.size(); i++) {
    Data.at(i)->first =hits;  
   }
  copy(channel, channel+hits, Data.at(0)->second);
  copy(value, value+hits, Data.at(1)->second);
  *IsUpdated = true;
 }

void
TigDataObject::Update(int hits, double* channel, double *value)
{
  if (Data.size() <2 ){
    cout << "[TigDataObject::Update] wrong data size " << mName << endl;
    return;
  }
  if (mDataLength <hits ){
    // cout << "[TigDataObject::Update] too many hits " << mName   << ": " << hits << " of " << mDataLength << endl;
    SetDataLength(hits);
  }
  for (int i=0; i<Data.size(); i++) {
    Data.at(i)->first =hits;  
   }
  copy(channel, channel+hits, Data.at(0)->second);
  copy(value, value+hits, Data.at(1)->second);
  *IsUpdated = true;
 }

void
TigDataObject::Update(int hits, int *value)
{
  if (Data.size() < 1 ){
    cout << "[TigDataObject::Update] wrong data size " << mName << endl;
    return;
  }
  if (mDataLength <hits ){
    //    cout << "[TigDataObject::Update] too many hits " << mName<< endl;
    SetDataLength(hits);
  }
  Data.at(0)->first = hits;
  copy(value, value+hits, Data.at(0)->second);

  *(this->IsUpdated) = true;
}

void
TigDataObject::Update(int hits, double *value)
{
  if (Data.size() < 1 ){
    cout << "[TigDataObject::Update] wrong data size " << mName << endl;
    return;
  }
  if (mDataLength <hits ){
    // cout << "[TigDataObject::Update] too many hits " << mName   << ": " << hits << " of " << mDataLength << endl;
    SetDataLength(hits);
  }
  // cout << "[TigDataObject::Update] before: hits " << Data.at(0)->first  << " - &Data.at(0)->first " << &(Data.at(0)->first)  << " - Data.at(0)->second " << Data.at(0)->second  << endl;
  Data.at(0)->first = hits;
  copy(value, value+hits, Data.at(0)->second);
  *(this->IsUpdated) = true;
  //  cout << "[TigDataObject::Update] after: hits " << Data.at(0)->first  << " - &Data.at(0)->first " << &(Data.at(0)->first)  << " - Data.at(0)->second " << Data.at(0)->second  << endl;
}

/*
int
TigDataObject::Size()
{
  return Data.size();
}
*/

