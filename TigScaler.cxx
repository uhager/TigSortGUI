// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigScaler.h"


using std::string;

//---- TigScaler
TigScaler::TigScaler(void)
: mEventData(NULL)
, mBank("MCS0")
{
  ObjType = "Scaler";
 }

//---- ~TigScaler
TigScaler::~TigScaler(void)
{
	delete [] mEventData;
}

//---- AddRequest
void 
TigScaler::AddRequest(string pName, int pChannel)
{
  mNames.push_back(pName);
  mRequested.push_back(pChannel);
}

//---- Index
int
TigScaler::Index(string pName)
{
  for (int i = 0; i<mNames.size(); i++)
    {
      if (mNames.at(i).compare(pName) == 0 ) return i;
    }
  return -1;
}

//---- Initialize
bool
TigScaler::Initialize(void)
{
  long numRequest = mRequested.size();
  mEventData = new int[numRequest];	
  for (int i = 0; i<numRequest; i++) {
    mEventData[i] = 0;
  }
  SetNumData(1);
  SetDataLength(mRequested.size());
  //  mName = mBank;
  return true;
}

//---- ParseInput
bool
TigScaler::ParseInput(string line)
{

  bool result = true;
  string token;
  std::istringstream stream(line.c_str());	
  stream >> token;

  if ( token.compare("bank") == 0)
	{	
	  stream >> token;
	  std::cout << ".(" << token << ").";
	  SetBank(token);
	}
  else result = this->TigObject::ParseInput(line);
  return result; 
}

//---- ParseInput
bool
TigScaler::ParseSignal(string line)
{
  bool result = true;
  string token;
  std::istringstream stream(line.c_str());	
   stream >> token;
   if ( token == "" || token[0] == '#') {}  //comment or blank
   else if ( token.compare("end") == 0) {
    //    std::cout << "[TigScaler::ParseSignal] " << mName << " end of signals" << std::endl;
     result = false;
   }
   else
     {
       int channel;
       stream >> channel;
       AddRequest(token, channel);
       //  std::cout << "added signal: " << channel << " - " << token << std::endl;
     }   
  return result; 
}


//---- ProcessEvent
bool 
TigScaler::ProcessEvent(std::vector<int> pData)
{
  for (int i = 0; i<mRequested.size(); i++)
    {
      if (mRequested[i] > pData.size()){
	std::cout << "[TigScaler::ProcessEvent] requested scaler channel not in data"<< std::endl;
	return false;
      }
      mEventData[i] = pData[ mRequested[i] ];
    }
  this->Update(mRequested.size(),mEventData);
      return true;
}

//---- Reset
void
TigScaler::Reset()
{
  ::memset(mEventData, 0, mRequested.size() * sizeof(int) );
  this->Clear();
}

//---- ScalerName
string
TigScaler::ScalerName(int pIndex)
{
  if (pIndex > mNames.size() ){
    std::cout << "[TigScaler::Name] requested name " << pIndex << " out of  " << mNames.size()  << std::endl;
    return "";
  }
  return mNames.at(pIndex);
}

