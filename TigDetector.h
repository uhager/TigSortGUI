// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGDETECTOR_H
#define TIGDETECTOR_H

#include <vector>
#include <string>
#include <TTree.h>
//#include <TigObject.h>
#include <TigEvent.h>
#include <TigDataObject.h>

using namespace std;

class TigDetector :  public TigDataObject
{
 public:				//----- public ---------------------
  TigDetector(void);
  ~TigDetector(void);

  void Clear();
  void AddSignal(int pChannel, int pAddress);
 void AddSignals(int minCh, int maxCh, int minAdd, int maxAdd);
  void ChangeDescription(string pDesc)	{ mDescription = pDesc; }
  bool Evaluate();
  bool FindSignal(TigEvent* pEvent);
  int GetAddress(int pChannel);
  bool Initialize(void);
  virtual bool ParseInput(string line);
  bool ParseSignal(string line);
  bool ProcessEvent(double* pData);
  bool ProcessSignal(TigEvent* pEvent, double (&pData)[2]);
  void SetDataType(string pTypeString, vector<double> pParameters);

  double* mEventData;
   double* mEventChannels;
  int mHits;

 protected:			//----- protected ------------------
  int mDataType;   // 0: charge (default), 1: CFD, 2: wf energy, 3: wf peak, 4: wf baseline
  bool mHasEventData;
  vector<double>	mParameters; // parameters needed for waveform analysis, i.e. bins
   map<int,int> mAddresses; //mAddresses[address] = index
   map<int,int> mChannels; // mChannel[index] = detector strip, translation between mEventData and mEventChannel position

 private:			//----- private --------------------
   TigDetector(const TigDetector&);
   TigDetector& operator=(const TigDetector&);
   
};

#endif /* TIGDETECTOR_H*/
