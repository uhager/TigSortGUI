// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TigData.h>

ClassImp(TigData)

//using namespace std;


TigData::TigData()
{
  //  cout << "[TigData::TigData]" << endl;
  NData = 0; 
  Data = new Double32_t;
  SetNMax(1);
}


TigData::TigData(int nmax)
{
  //  cout << "[TigData::TigData] " << nmax << endl;
  NData = 0; 
  Data = new Double32_t;
  SetNMax(nmax);
}

TigData::~TigData()
{
  delete Data;
}

void
TigData::Clear()
{
    delete Data;
    Data = new Double32_t[NMax];
  NData = 0;
}

void
TigData::SetNMax(int nmax)
{
  //  cout << "[TigData::SetNMax] NMax " << nmax << endl; 
  NMax = nmax;
  delete Data; 
  Data = new Double32_t[NMax];
}


