// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGSCALER_H
#define TIGSCALER_H

#include <vector>
#include <string>

#include "TigDataObject.h"


class TigScaler : public TigDataObject
{
 public:
  TigScaler();
  ~TigScaler();
  void AddRequest(std::string pName, int pChannel);
  std::string Bank(){return mBank;}
  bool Initialize(void);
  int Index(std::string pName);
  std::string ScalerName(int pIndex);
  std::vector<std::string> Names(void) { return mNames; }
  virtual bool ParseInput(std::string line);
  bool ParseSignal(std::string line);
  bool ProcessEvent(std::vector<int> pData);
  void Reset();
  void SetBank(std::string pBank){mBank = pBank;}
  //  void SetEventID(int pEventID){mEventID = pEventID;}
  int Size(){return mRequested.size();}
  int* mEventData;

 protected:			//----- protected ------------------
  std::string mBank;
  std::vector<int> mRequested; //  channels requested
  std::vector<std::string> mNames; // names for requested channels (branch names in tree)

 private:			//----- private --------------------
  TigScaler( const TigScaler& );
  TigScaler& operator=( const TigScaler& );
};

#endif /* TIGSCALER_H */
