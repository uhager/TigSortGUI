// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGSCALER_H
#define TIGSCALER_H

#include <vector>
#include <cstring>
#include <TigDataObject.h>

using namespace std;

class TigScaler : public TigDataObject
{
 public:
  TigScaler();
  ~TigScaler();
  void AddRequest(string pName, int pChannel);
  string Bank(){return mBank;}
  bool Initialize(void);
  int Index(string pName);
  string ScalerName(int pIndex);
  vector<string> Names(void) { return mNames; }
  virtual bool ParseInput(string line);
  bool ParseSignal(string line);
  bool ProcessEvent(vector<int> pData);
  void Reset();
  void SetBank(string pBank){mBank = pBank;}
  //  void SetEventID(int pEventID){mEventID = pEventID;}
  int Size(){return mRequested.size();}
  int* mEventData;

 protected:			//----- protected ------------------
  string mBank;
  vector<int>	mRequested; //  channels requested
  vector<string> mNames; // names for requested channels (branch names in tree)

 private:			//----- private --------------------

};

#endif /* TIGSCALER_H */
