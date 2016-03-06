// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGEVENT_H
#define TIGEVENT_H

#include <stdint.h>
#include <vector>
#include <string>


using namespace std;

const int LIN_EQ_DIM=3;

struct lin_eq_type
{
  long double matrix[LIN_EQ_DIM][LIN_EQ_DIM];
  long double vector[LIN_EQ_DIM];
  long double solution[LIN_EQ_DIM];
  int    dim;
  double freq;
};


class TigEvent
{
public:				//----- public ---------------------
  TigEvent(void);
  ~TigEvent(void);

  int Address(){return mAddress;}
  void AddWfSample(int pSample);
  double CalcCFDTimeDiff();
  double CalcWfBase(vector<double> pParameters);
  double CalcWfEnergy(vector<double> pParameters);
  int CalcWfMaxBin();
  int CalcWfMinBin();
  double CalcWfPeak(vector<double> pParameters);
  double CalcWfSinePhase(vector<double> pParameters);
  int CFD(){return mCFD;}
  int Charge(){return mCharge;}
  int EventID(){return mEventID;}
  int Lifetime(){return mLifetime;}
  int LED(){return mLED;}
  void SetAddress(int pAddress){mAddress=pAddress;}
  void SetCFD(int pCFD){mCFD=pCFD;}
  void SetCharge(long pCharge){mCharge=pCharge;}
  void SetEventID(int pEventID){mEventID = pEventID;}
  void SetLED(int pLED){mLED=pLED;}
  void SetLifetime(int pLifetime){mLifetime = pLifetime;}
  void SetTimestamp(int pTimestamp){mTimestamp = pTimestamp;}
  void SetTimestampUp(int pTimestampUp){mTimestampUp = pTimestampUp;}
  void SetTriggersAccepted(int pTriggers){mTriggersAccepted = pTriggers;}
  void SetTriggersRequested(int pTriggers){mTriggersRequested = pTriggers;}
  int SolveLinEq(lin_eq_type *lin_eq);
  long double SolveLinEqDet(int m, lin_eq_type *lin_eq);
  void SolveLinEqIni(int NWf, double freq);
  int Timestamp(){return mTimestamp;}
  int TimestampUp(){return mTimestampUp;}
  int TriggersAccepted(){return mTriggersAccepted;}
  int TriggersRequested(){return mTriggersRequested;}
  double Value(int pDataType, vector<double> pParameters);
  vector<short> Waveform(){return mWaveform;}

protected:			//----- protected ------------------
  int mAddress;
  double mCharge;
  int mCFD;
  int mEventID;
  int mLED;
  int mLifetime;
lin_eq_type mLinEq;
  int mTimestamp;
  int mTimestampUp;
  int mTriggersAccepted;
  int mTriggersRequested;
  vector<short> mWaveform;
 
private:			//----- private --------------------
  TigEvent(const TigEvent& );
  TigEvent& operator = (const TigEvent&);
};

#endif /* TIGEVENT_H */
