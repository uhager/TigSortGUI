// part of TigSortGUI
// author: Ulrike Hager

#include <sstream>
#include <iostream>

#include "TigDetector.h"

using std::vector;
using std::string;
using std::map;

//---- TigDetector
TigDetector::TigDetector(void)
  : TigDataObject(2)
  ,mEventData(NULL)
  , mEventChannels(NULL)
  , mHasEventData(false)
  , mDataType(0)
  , mHits(0)
{
  int param[] = {10,15, 200,205, 165,176};
  mParameters = vector<double>(param, param + sizeof(param)/sizeof(double) ); 
  ObjType = "Detector";
}

//---- ~TigDetector
TigDetector::~TigDetector(void)
{
  delete [] mEventData;
  delete [] mEventChannels;
}

//---- AddSignal
void
TigDetector::AddSignal(int pChannel, int pAddress)
{
  int index = mAddresses.size();
  mAddresses[pAddress] = index ;
  mChannels[index] = pChannel;
  //  mDataLength++;
}

//---- AddSignals
void 
TigDetector::AddSignals(int minCh, int maxCh, int minAdd, int maxAdd)
{
  int numCh = maxCh - minCh +1;
  bool channelInv = false;
  int chStep = 1;
  if (numCh<0) chStep *= -1;
  int addStep;
  if (minAdd < maxAdd) addStep = (maxAdd - minAdd +1)/numCh;
  else addStep = (maxAdd - minAdd -1)/numCh;
  //  std::cout << "[TigDetector::AddSignals] " << mName << " " <<  minAdd  << " " << std::hex << minAdd << " maxAdd " << maxAdd << " addStep " << dec << addStep << std::endl; 
  for (int i=minCh, add = minAdd; i<maxCh+1; i = i+chStep, add=add+addStep){
    //std::cout << "[TigDetector::AddSignals] adding " << dec<< i << " - " << std::hex << add << std::endl; 
    this->AddSignal(i, add);
  }
}


//---- Clear
void
TigDetector::Clear()
{
  //  std::cout << "[TigDetector::Clear]" << std::endl;
    if (this->DataLength() == 1) mEventData[0] = 0; 
  mHits = 0;
  this->TigDataObject::Clear();
}

//---- Evaluate
bool
TigDetector::Evaluate()
{
  *IsUpdated = false;
  if (Data.size()>0){
    if ((Data.at(0))->first > 0) {
	//	for (int i=1;i<Data.size(); i++) Data.at(i)->first = Data.at(0)->first ;
      // if  (this->DataSize()==2){
      // 	  std::cout << "[TigDetector::Evaluate] channel/value size mismatch"  << std::endl;
      // 	  return false;
      // }
      *IsUpdated = true; 
    }
  }
  return true;
}

//---- FindSignal
bool
TigDetector::FindSignal(TigEvent* pEvent)
{
  bool result = false; 
  if (mAddresses.find(pEvent->Address()) != mAddresses.end()) result = true;
  return result;
}

//---- GetAddress
int 
TigDetector::GetAddress(int pChannel)
{
  int index = -1;
  map<int,int>::iterator iter;
  for (iter=mChannels.begin(); iter != mChannels.end(); ++iter){
    if ( (*iter).second == pChannel )  index = (*iter).first;
  }
  if (index <0 ) return index;
  for (iter=mAddresses.begin(); iter != mAddresses.end(); ++iter) {
    if ((*iter).second == index) index = (*iter).first;
  }
  return index;
}

//---- Initialize
bool
TigDetector::Initialize(void)
{
  // std::cout << "[TigDetector::Initialize] " << mName << std::endl;
  bool check = this->TigDataObject::Initialize();
 if (!check) return false;
  long numAddresses = mAddresses.size();
  //  std::cout << "[TigDetector::Initialize] numAddresses " << numAddresses << std::endl;
  mEventData = new double[numAddresses];	
  mEventChannels = new double[numAddresses];
  for (int i = 0; i<numAddresses; i++) {
    mEventChannels[i] = 0;
    mEventData[i] = -1;
  }
  //  if (numAddresses == 1) this->SetNumData(1);
  this->SetDataLength( numAddresses);
  //std::cout << "[TigDetector::Initialize] " << mName << " initialized " << std::endl;
  return true;
}

//---- ParseInput
bool
TigDetector::ParseInput(string line)
{
  //  std::cout << "[TigDetector::ParseInput] " << mName << std::endl; 
  bool result = true;
  string token;
  std::istringstream stream(line.c_str());	
  stream >> token;

  if ( token == "" || token[0] == '#') { }  //comment or blank
  else if ( token.compare("datatype") == 0)
    {
      stream >> token;
      vector<double> parameters;
      while ( !stream.eof() ) {
	parameters.push_back(0);
	stream >> parameters.back();
      }
      this->SetDataType(token,parameters);
    }

  else result = this->TigObject::ParseInput(line);
  return result; 
}

//---- ParseInput
bool
TigDetector::ParseSignal(string line)
{
  bool result = true;
  string token;
  std::istringstream stream(line.c_str());	
  stream >> token;
  if ( token == "" || token[0] == '#') { }  //comment or blank
  else if ( token.compare("range") == 0)
    {
      int minCh, maxCh, minAdd, maxAdd;
      stream >> minCh >> maxCh >> std::hex >> minAdd >> std::hex >> maxAdd;
      this->AddSignals(minCh, maxCh, minAdd, maxAdd);
    }
  else if ( token.compare("end") == 0) {
    //    std::cout << "[TigDetector::ParseSignal] " << mName << " end of signals" << std::endl;
    result = false;
  }
  else
    {
      int channel;
      std::istringstream ( token ) >> channel;
      int address;				
      stream >> std::hex >> address;
      this->AddSignal(channel,address);
      //      std::cout << "added signal: " << channel << " - " << address << std::endl;
    }
  return result; 
}

//---- ProcessEvent
bool
TigDetector::ProcessEvent(double* pData)
{
  //std::cout << "[TigDetector::ProcessEvent]" << mName << std::endl;
  *this->IsUpdated = false;
  mHits = 0;
  for (int i=0; i<this->DataLength(); i++)
    {
      if (pData[i] > -1){
	mEventData[mHits] = pData[i];
	mEventChannels[mHits] = mChannels[i];
	mHits++;
      }
    }
  if (mHits>0){ 
    //    if (this->DataSize() == 1) this->Update(mHits, mEventData);
    this->Update(mHits,mEventChannels, mEventData);
  }
 //  std::cout << "[TigDetector::ProcessEvent] end" << std::endl;
  return true;
}

//---- ProcessSignal
bool
TigDetector::ProcessSignal(TigEvent* pEvent, double (&pData)[2])
{
  bool result = false; 
  if (mAddresses.find(pEvent->Address()) != mAddresses.end())
    {
      int addr = pEvent->Address();
      mHasEventData = true;
      result = true;
      pData[0] = mAddresses[addr]; 
      pData[1] = pEvent->Value(mDataType, mParameters);
      //      std::cout << "[TigDetector::ProcessSignal] name " << mName << " datatype " << mDataType << " channel " << pData[0] << " data " << pData[1] << std::endl;
      return result;
    }
  // std::cout << "unknown channel " << pChannel << std::endl;
  return result;
}


//---- SetDataType
void
TigDetector::SetDataType(string pTypeString, vector<double> pParameters)
{
  if (pTypeString.compare("Charge") == 0) mDataType = 0;
  else if (pTypeString.compare("CFD") == 0) mDataType = 1;
  else if (pTypeString.compare("WfEnergy") == 0) mDataType = 2;
  else if (pTypeString.compare("WfPeak") == 0) mDataType = 3;
  else if (pTypeString.compare("WfBase") == 0) mDataType = 4;
  else if (pTypeString.compare("WfMaxBin") == 0) mDataType = 5;
  else if (pTypeString.compare("WfMinBin") == 0) mDataType = 6;
  else if (pTypeString.compare("Timestamp") == 0) mDataType = 7;
  else if (pTypeString.compare("TimestampUp") == 0) mDataType = 8;
  else if (pTypeString.compare("Lifetime") == 0) mDataType = 9;
  else if (pTypeString.compare("TriggersAccepted") == 0) mDataType = 10;
  else if (pTypeString.compare("TriggersRequested") == 0) mDataType = 11;
  else if (pTypeString.compare("LED") == 0) mDataType = 12;
  else if (pTypeString.compare("RFphase") == 0) mDataType = 13;
  else mDataType = 0;
  if ( (mDataType == 2) || (mDataType == 3) || (mDataType == 4) || (mDataType == 13)  ){
    //    if (pParameters.size()>0) {
      switch (mDataType){
      case 2:
	if (pParameters.size() >= 6 ) mParameters = pParameters;
	break;
      case 3:
	if (pParameters.size() >= 2 ) mParameters = pParameters;	
	break;
      case 4:
	if (pParameters.size() >= 4 ) mParameters = pParameters;
	break;
      default: mParameters = pParameters;
	//      }      
    }
    //std::cout << "[TigDetector::SetDataType] parameters: " ;
    // for (int i=0; i<mParameters.size(); i++) std::cout << mParameters.at(i) << ", ";
    //std::cout << std::endl;
  }
  //  std::cout << "[TigDetector::SetDataType] " << pTypeString << " data type is " << mDataType << std::endl;
}

