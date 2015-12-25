// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <cmath>
#include <TMath.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TigEvent.h>

//---- TigEvent
TigEvent::TigEvent(void)
  :mCharge(-1)
  ,mCFD(-1)
  ,mLED(-1)
  ,mAddress(-1)
  ,mEventID(-1)
   //  ,mWaveform(NULL)
  ,mTimestamp(-1)
  ,mLifetime(-1)
  ,mTriggersRequested(-1)
  ,mTriggersAccepted(-1)
  ,mTimestampUp(-1)
{
  //  cout << "[TigEvent::TigEvent]" << endl;
  mLinEq.matrix[0][0] = 0;
}

//---- ~TigEvent
TigEvent::~TigEvent(void)
{
  //  cout << "[TigEvent::~TigEvent]" << endl;
  mWaveform.clear();
}

//---- AddWfSample
void
TigEvent::AddWfSample(int pSample)
{
  short sSample = pSample;
  if ( sSample & (1<<13) ) { sSample |= 0x3 << 14; }   // charge bit extension 14 to 16 bits 
  mWaveform.push_back(sSample+8192);
}

//---- CalcCFDTimeDiff
double
TigEvent::CalcCFDTimeDiff()
{
  //  cout << "[TigEvent::CalcCFDTimeDiff]" << endl;
  double result;
  double TSns = mTimestamp * 10.0; // 10 ns per sample
  double CFDns = mCFD *10.0/16.0;   // 1/16 sample resolution in CFD
  result = CFDns - TSns ;
  cout << "[TigEvent::CalcCFDTimeDiff] result " << result << endl;
  return result;
}

//---- CalcWfBase
double
TigEvent::CalcWfBase(vector<double> pParameters)
{
  if (mWaveform.size()<250){
    cout << "[TigEvent::CalcWfBase] incomplete waveform, length " << mWaveform.size() << endl;
    return -1;
  }
  if (pParameters.size()<4){
    cout << "[TigEvent::CalcWfBase] too few parameters " << pParameters.size() << endl;
    return -1;
  }
  double result = 0;
  for (int i=pParameters.at(0); i<=pParameters.at(1);i++) result += fabs(mWaveform.at(i));
  for (int i=pParameters.at(2); i<=pParameters.at(3);i++) result += fabs(mWaveform.at(i));
  int numBins = pParameters.at(1) - pParameters.at(0) +pParameters.at(3) - pParameters.at(2) +2;
  result /= (double)numBins;
  return result;
}

//---- CalcWfEnergy
double
TigEvent::CalcWfEnergy(vector<double> pParameters)
{
  if (mWaveform.size()<250){
    cout << "[TigEvent::CalcWfEnergy] incomplete waveform, length " << mWaveform.size() << endl;
    return -1;
  }
  if (pParameters.size()<6){
    cout << "[TigEvent::CalcWfEnergy] too few parameters " << pParameters.size() << endl;
    return -1;
  }
  double result = 0;
  for (int i=pParameters.at(0); i<=pParameters.at(1);i++) result -= mWaveform.at(i);
  for (int i=pParameters.at(4); i<=pParameters.at(5);i++) result += mWaveform.at(i);
  for (int i=pParameters.at(2); i<=pParameters.at(3);i++) result -= mWaveform.at(i);
  result = fabs(result);
  return result;
}

//---- CalcWfMaxBin
int
TigEvent::CalcWfMaxBin()
{
  int result = 0;
  int maxValue = -1000000;
    for (int i=0; i<mWaveform.size();i++) {
      if (mWaveform.at(i) > maxValue) {
	maxValue = mWaveform.at(i);
	result = i;
      }
    }
  return result;
}

//---- CalcWfMinBin
int
TigEvent::CalcWfMinBin()
{
  int result = 0;
  int minValue = 1000000;
    for (int i=0; i<mWaveform.size();i++) {
      if (mWaveform.at(i) <= minValue) {
	minValue = mWaveform.at(i);
	result = i;
      }
    }
  // cout << "wf: " ;
  // for (int i=0; i<mWaveform.size();i++) {
  //   if (i==result) cout << " _" << mWaveform.at(i) << "_";
  //   else cout << " " << mWaveform.at(i);
  //   }
  // cout << endl;
  return result;
}

//---- CalcWfEnergy
double
TigEvent::CalcWfPeak(vector<double> pParameters)
{
  if (mWaveform.size()<250){
    cout << "[TigEvent::CalcWfPeak] incomplete waveform, length " << mWaveform.size() << endl;
    return -1;
  }
  if (pParameters.size()<2){
    cout << "[TigEvent::CalcWfPeak] too few parameters " << pParameters.size() << endl;
    return -1;
  }
  double result = 0;
  int numBins = pParameters.at(1) - pParameters.at(0) + 1;
  for (int i=pParameters.at(0); i<=pParameters.at(1);i++) result += (int)(fabs((float)mWaveform.at(i)/(float)numBins));
  return result;
}

double
TigEvent::CalcWfSinePhase(vector<double> pParameters)
{
  //  cout << "[TigEvent::CalcWfSinePhase]" << endl;
  double result = -1;

////////// use Kris' calculation

  if (mLinEq.matrix[0][0] == 0) {
    double period ;
    if (pParameters.size()) period = pParameters.at(0)/10.0; ///(double)10;
    else period = 85.47/(double)10; // ISAC RF, 11.7 MHz
    SolveLinEqIni(mWaveform.size(),period);
  }

  for(int i=0;i<mLinEq.dim;i++) mLinEq.vector[i]=0;
  
  for(int i=0;i<mWaveform.size();i++)
    {
      mLinEq.vector[0]+=mWaveform.at(i)*sin(mLinEq.freq*i);
      mLinEq.vector[1]+=mWaveform.at(i)*cos(mLinEq.freq*i);
      mLinEq.vector[2]+=mWaveform.at(i);
    }
  if(SolveLinEq(&mLinEq)==0)
    {
      return 0;
    }
 double A =sqrt(mLinEq.solution[0]*mLinEq.solution[0]+mLinEq.solution[1]*mLinEq.solution[1]);
  double C=mLinEq.solution[2];
 
  double s=-mLinEq.solution[1]/A;
  double c=mLinEq.solution[0]/A;

  if(s>=0)
    result = acos(c)/mLinEq.freq;
  else
     result = (1-acos(c)/(2*TMath::Pi()))*(2*TMath::Pi())/mLinEq.freq;
  //  cout << "TigEvent::CalcWfSinePhase] " << result << endl;

  result *= 10.0;
  return result;
}

//---- Value
double
TigEvent::Value(int pDataType, vector<double> pParameters)
{
  switch(pDataType){
  case 0:
    return mCharge;
  case 1:
    return mCFD;
   case 2:
    return CalcWfEnergy(pParameters);
  case 3:
    return CalcWfPeak(pParameters);
  case 4:
    return CalcWfBase(pParameters);
  case 5:
    return CalcWfMaxBin();
  case 6:
    return CalcWfMinBin();
  case 7:
    return mTimestamp;
  case 8:
    return mTimestampUp;
  case 9:
    return mLifetime;
  case 10:
    return mTriggersAccepted;
  case 11:
    return mTriggersRequested;
  case 12:
    return mLED;
  case 13:
    return CalcWfSinePhase(pParameters);
  default:
    return -1;
  }
}

int 
TigEvent::SolveLinEq(lin_eq_type *lin_eq)
{
  //  cout << "[TigEvent::SolveLinEq]" << endl;
  lin_eq_type z;
  long double w;

  int n=lin_eq->dim;

      memcpy(z.matrix,lin_eq->matrix,sizeof(lin_eq->matrix));
      w=SolveLinEqDet(n,&z);

      if(w==0.) 
	{
	return 0;
	}
    for(int i=0;i<n;i++)
    {
      memcpy(z.matrix,lin_eq->matrix,sizeof(lin_eq->matrix));
      memcpy(z.matrix[i],lin_eq->vector,sizeof(lin_eq->vector));
      lin_eq->solution[i]=SolveLinEqDet(n,&z)/w;
    }
    return 1;
}


long double  
TigEvent::SolveLinEqDet(int m, lin_eq_type *lin_eq)
{
  //  cout << "[TigEvent::SolveLinEqDet]" << endl;
  int i,j;
  long double s;

  if(m==1) return lin_eq->matrix[0][0];


  if(lin_eq->matrix[m-1][m-1]==0.)
    {
      j=m-1;
      while(lin_eq->matrix[m-1][j]==0 && j>=0) j--;
      if(j<0) 
	return 0.;
      else
	for(i=0;i<m;i++)
	  {
	    s=lin_eq->matrix[i][m-1];
	    lin_eq->matrix[i][m-1]=lin_eq->matrix[i][j];
	    lin_eq->matrix[i][j]=s;
	  }
  }
  for(j=m-2;j>=0;j--)
    for(i=0;i<m;i++)
      lin_eq->matrix[i][j]-=lin_eq->matrix[i][m-1]/lin_eq->matrix[m-1][m-1]*lin_eq->matrix[m-1][j];
  return lin_eq->matrix[m-1][m-1]*SolveLinEqDet(m-1,lin_eq);
}


void
TigEvent::SolveLinEqIni(int N, double T)
{
  //  cout << "[TigEvent::SolveLinEqIni]" << endl;
  int i;
  double s,sn,snm,s2,s2n,s2nm,c,cn,cnm,c2,c2n,c2nm,w;
  mLinEq.dim=3;

  mLinEq.freq=(2*TMath::Pi())/T;

  s=sin(mLinEq.freq);
  sn=sin(N*mLinEq.freq);
  snm=sin((N-1)*mLinEq.freq);
  s2=sin(2*mLinEq.freq);
  s2n=sin(2*N*mLinEq.freq);
  s2nm=sin(2*(N-1)*mLinEq.freq);

  c=cos(mLinEq.freq);
  cn=cos(N*mLinEq.freq);
  cnm=cos((N-1)*mLinEq.freq);
  c2=cos(2*mLinEq.freq);
  c2n=cos(2*N*mLinEq.freq);
  c2nm=cos(2*(N-1)*mLinEq.freq);
 

  mLinEq.matrix[0][0]=0.5*N-0.25*(1-c2-c2n+c2nm)/(1-c2);
  mLinEq.matrix[0][1]=0.25*(s2+s2nm-s2n)/(1-c2);
  mLinEq.matrix[1][0]=mLinEq.matrix[0][1];
  mLinEq.matrix[0][2]=0.5*(s+snm-sn)/(1-c);
  mLinEq.matrix[2][0]=mLinEq.matrix[0][2];
  mLinEq.matrix[1][1]=0.5*N+0.25*(1-c2-c2n+c2nm)/(1-c2);
  mLinEq.matrix[1][2]=0.5*(1-c-cn+cnm)/(1-c);
  mLinEq.matrix[2][1]=mLinEq.matrix[1][2];
  mLinEq.matrix[2][2]=N;
}
