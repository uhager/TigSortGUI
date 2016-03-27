// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigLookupTable.h"

using std::vector;
using std::string;

TigLookupTable::TigLookupTable()
{
  mName = "lookuptable";
  mWhatToDo = LOOKUPTABLE;
  ObjType = "Formula";
  for (int i=0;i<2;i++) {
    vector<int> tmp;
    mChannels.push_back(tmp);
  }
}


void
TigLookupTable::AddTableLine(vector<double> pTable)
{
  mLookupTable.push_back(pTable);
}

bool
TigLookupTable::Evaluate()
{
  //  std::cout << "[TigLookupTable::Evaluate] " << mName <<  std::endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;
  for (int i = 0; i<InputData.size(); i++ ) {
    if ( *(InputUpdated.at(i)) == false ) return false;
  }
  vector<vector<int> > channelCombinations;
  for (int i = 0; i<InputData.size(); i++ ) {
    vector<int> tmp;
    channelCombinations.push_back(tmp);
    for (int j=0; j<(InputData.at(i))->first; j++){
      int channel = InputData.at(i)->second[j];
      for (int k=0;k<mChannels.at(i).size();k++){
	if (channel == mChannels.at(i).at(k) ) {
	  channelCombinations.at(i).push_back(k);
	  break;
	}
      }
    }
  }
  if (channelCombinations.size() == 1){
    int combs = channelCombinations.at(0).size();
    double result[combs];
    for (int i = 0; i<combs; i++) result[i] = mLookupTable[0][ channelCombinations.at(0).at(i) ];
    this->Update(combs,result);
    return true;
  }
  else if (channelCombinations.size() == 2){
    int combs =  channelCombinations.at(0).size() * channelCombinations.at(1).size();
    double result[combs];
    int counter = 0;
    for (int i=0; i<channelCombinations.at(0).size(); i++){
      for (int j=0; j<channelCombinations.at(1).size(); j++){
	result[ counter ] = mLookupTable[ channelCombinations.at(1).at(j) ][ channelCombinations.at(0).at(i) ];
	counter++;
      }
    }
    this->Update(combs,result);
    return true;
  }
  return false;
}

void 
TigLookupTable::IncreaseDataLength(int d)
{
  //  std::cout << "[TigLookupTable::IncreaseDataLength] " << mName << " - " << d  << std::endl;
  mDataLength *= d;
  this->SetDataLength(mDataLength);
}

bool
TigLookupTable::Initialize(void)
{
  // std::cout << "[TigLookupTable::Initialize] " << mName << std::endl;
  if (mLookupTable.size()<1){
      std::cerr << "[TigLookupTable::Initialize] " << mName << " no table data available" << std::endl;
      return false;    
  }
  if (mInputNames.size() > 2) {
      std::cerr << "[TigLookupTable::Initialize] too many inputs" << std::endl;
      return false;
  }
  for (int i=1; i<mLookupTable.size(); i++){
    if (mLookupTable.at(i).size() != mLookupTable.at(0).size()){
      std::cerr << "[TigLookupTable::Initialize] " << mName << " table lines have different lengths" << std::endl;
      return false;
      } 
  }
  if (mChannels.at(0).size()  != mLookupTable.at(0).size()){
    std::cerr << "[TigLookupTable::Initialize] " << mName << " wrong number of x channels" << std::endl;
    return false;
  }
  if (mInputNames.size() == 1) mChannels.at(1).push_back(0);
  if (mChannels.at(1).size()  != mLookupTable.size()){
    std::cerr << "[TigLookupTable::Initialize] " << mName << " wrong number of y channels" << std::endl;
    return false;
  }
  bool check = this->TigDataObject::Initialize();
  if (!check) return false;

 this->SetNumData(1);
 this->SetDataLength(1 );
  //std::cout << "[TigLookupTable::Initialize] " << mName << " initialized " << std::endl;
  return true;
}

bool
TigLookupTable::ParseInput(string line)
{
  bool result = true;
  string token;
  std::istringstream stream(line.c_str());	
  stream >> token;

  if ( token == "" || token[0] == '#') { }  //comment or blank
  else if ( token.compare("xchannels") == 0)
    {
      stream >> token;
      vector<int> parameters;
      while ( !stream.eof() ) {
	parameters.push_back(0);
	stream >> parameters.back();
      }
      this->SetXChannels(parameters);
    }
  else if ( token.compare("ychannels") == 0)
    {
      stream >> token;
      vector<int> parameters;
      while ( !stream.eof() ) {
	parameters.push_back(0);
	stream >> parameters.back();
      }
      this->SetYChannels(parameters);
    }
  else if ( token.compare("table") == 0)
    {
      stream >> token;
      vector<double> parameters;
      while ( !stream.eof() ) {
	parameters.push_back(0);
	stream >> parameters.back();
      }
      this->AddTableLine(parameters);
    }
  else result = this->TigDataObject::ParseInput(line);
  return result; 
}

void
TigLookupTable::SetXChannels(vector<int> pChannels)
{
  mChannels.at(0) = pChannels;
}

void
TigLookupTable::SetYChannels(vector<int> pChannels)
{
  mChannels.at(1) = pChannels;
}


