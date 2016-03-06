// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TigTree.h>
#include <TigDependentObject.h>

TigDependentObject::TigDependentObject()
  : TigDataObject()
  ,mWhatToDo(-1)
  ,SizeDetermined(false)
{
 
}

TigDependentObject::TigDependentObject(int num)
  :TigDataObject(num)
  ,mWhatToDo(-1)
  ,SizeDetermined(false)
{
}

TigDependentObject::~TigDependentObject()
{
  //   cout << "[TigDependentObject::~TigDependentObject] " << mName << endl;
  // nothing to do, all your pointer are belong to others.
}

void
TigDependentObject::AddCut(string name)
{
    pair<string,int> par;
    par.first  = name;
    par.second = 0;
    //    cout << "[TigDependentObject::AddInput] parameter " << i << "  channel/value " << par.second   << endl;
    mNeededCuts.push_back(par);
    CutData.push_back( NULL );
}

void
TigDependentObject::AddInput(string name, int id)
{
    pair<string,int> par;
    par.first  = name;
      par.second = id;
    //    cout << "[TigDependentObject::AddInput] parameter " << i << "  channel/value " << par.second   << endl;
    mNeeded.push_back(par);
    InputUpdated.push_back(NULL);
    InputData.push_back( NULL );
}


bool
TigDependentObject::Evaluate()
{
  *IsUpdated = false;
  for (int i = 0; i<CutData.size(); i++) {
    if ( CutData.at(i)->size() == 0 ) return false;
    if ( CutData.at(i)->at(0) == 0 ) return false;
  }
  return true;
}

bool
TigDependentObject::IniInputs()
{
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
	cout << "[TigDependentObject::IniInputs] input " << parN << " not found for object " << mName << endl;
	return false;
      }
      else {
	for (int j =0; j<inputSize; j++){
	  AddInput(parN,j);
	}
      }
    }
    //    cout << "[TigFormula::Initialize] parameter " << i << "  channel/value " << par.second   << endl;
  }
  for (int i=0; i<mCutNames.size(); i++) AddCut(mCutNames.at(i));
  //    for (int i=0; i<mNeeded.size(); i++) 
  return true;
}

bool
TigDependentObject::Initialize()
{
  bool check = this->TigDataObject::Initialize();
  if (!check) return false;
 return this->IniInputs();
}

bool
TigDependentObject::ParseInput(string line)
{
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
	 //	 cout << "[TigDependentObject::ParseInput] unknown token " << token << endl;
	 return this->TigObject::ParseInput(line);
       }
      return true;
 }
