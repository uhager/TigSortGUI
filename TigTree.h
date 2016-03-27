// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGTREE_H
#define TIGTREE_H

#include <vector>
#include <string>
#include <TTree.h>
#include <TChain.h>

#include "TigObject.h"
#include "TigDataObject.h"
#include "TigEvent.h"
#include "TigDetector.h"
#include "TigScaler.h"
#include "TigScalerHisto.h"
#include "TigFormula.h"
#include "TigHisto.h"
#include "TigWaveformHisto.h"
#include "TigCut.h"
#include "TigCalibration.h"
#include "TigSortObject.h"
#include "TigLookupTable.h"

#ifdef __MAKECINT__
#pragma link C++ class std::vector<std::vector<int> >+;
#endif


class TigManager;


class TigAssemble
{
 public:
  TigAssemble(void);
  ~TigAssemble(void);

  void AddData(int pDetector, int pChannel, double pValue);
  void AddDetector(int pSignal);
  void AddWfData(int pDet, std::vector<short> wfData);
  void AddWfDetector();
  int AnaEventID(){return mAnaEventID;}
  double* Data(int pDet);
  int EventID(){return mEventID;}
  int RunNumber(){return mRunNumber;}
  void SetAnaEventID(int pEventID){mAnaEventID = pEventID;}
  void SetDetectors(std::vector<double*> pDets){mEventData = pDets;}
  void SetEventID(int pEventID){mEventID = pEventID;}
  void SetRunNumber(int pNum){mRunNumber = pNum;}
  void SetTimeStamp(int pNum){mTimeStamp = pNum;}
  int Size(){return mEventData.size();}
  int TimeStamp(){return mTimeStamp;}
  std::vector<short> Waveform(int pDet){return mWaveforms.at(pDet);}

 private:
  int mAnaEventID;
  std::vector<double*> mEventData;
  int mEventID;
  int mRunNumber;
  int mTimeStamp;
  std::vector<std::vector<short> > mWaveforms;
};

class TigTree : public TigObject
{
 public:				//----- public ---------------------
  TigTree(void);
  ~TigTree(void);

  void AddDetector(TigDetector* pToAdd);
  void AddInputFile(TString pFile);
  void AddScaler(TigScaler* pToAdd);
  void Banks(std::vector<std::string> &pRequested);
  int Buffer(){return mBuffer;}
  bool ConfigFromRootTree(TTree *pTree);
  void ConnectFormulas();
  std::vector<TigDataObject*> DataObjects(){return mObjects;}
  void DeleteFormulas();
  void DeleteHistos();
  void DeleteObject(std::string name);
  void DeleteTTree(){mTree->Delete();}
  std::vector<std::string> DetectorNames();
  bool FillTree();
  bool FillTree(TigAssemble* pAssembled);
  bool FindScalerBank(std::string pBankName);
  TigDataObject *FindDataObject(std::string name);
  bool FindObject(std::string name);
  int FlushBuffer(int pCutoff);
   bool GenerateTree;  // switch between filling tree and filling histograms
   int GetAddress(std::string pDetName, int pChannel);
  int GetDataSize(std::string dataObj);
  int GetScalerIndex(std::string pName);
  std::vector<std::string> HistoNames();
 TH1 *Histogram(int histN);
 void InitInputChainAddresses();
  void Initialize(void);
  void ParseCalibration(istream& pStream);
  void ParseDetector(istream& pStream);
  void ParseFormHist(istream& pStream);
  void ParseScaler(istream& pStream);
  bool ProcessSignal(TigEvent* pEvent);
  bool ProcessScaler(std::string pBankName, std::vector<int> pValues);
  void ResetTree();
  void ResetHistos();
  bool RunChain();
  int ScEventID(){return mScEventID;}
  void SetBuffer(int pBuffer){mBuffer=pBuffer;}
  void SetDirectory(TDirectory *dir) {mTree->SetDirectory(dir);}
  void SetParent(TigManager *parent){mParent = parent;}
  void Step(bool step){mStepFile = step;}
  bool ToggleWrite(int det);
  void WriteHistos();
  TTree *Tree(){return mTree;}
  void Write();

 protected:			//----- protected ------------------
  int AddEvent(int pEventID);
  long BranchCount(void) const;
  void ParseCut(istream& pStream);
  void ParseFormula(istream& pStream);
  void ParseHisto(istream& pStream);
  void ParseScalerHisto(istream& pStream);
  void ParseSorter(istream& pStream);
  void ParseLookup(istream& pStream);
  void ParseWaveform(istream& pStream);
  void ProcessValues(void);

  int mAnaEventID;
  std::vector<TigAssemble*> mAssembled;
  int mBuffer;
  Int_t mCurrentEntry;
  std::vector<TigDataObject*> mObjects;
  bool mHasEventData;
  std::vector<TigHistoObject*> mHistos;
  TChain *mInputChain;
  TigManager *mParent;
  int mRunNumber;
  int mScEventID;
  bool mStepFile;
  int mTimeStamp;
  TTree* mTree;
  int mTrigEventID;
  std::vector<TigWaveformHisto*> mWfHistos;

 private:			//----- private --------------------
  TigTree(const TigTree& );
  TigTree& operator = (const TigTree& );

  //  ClassDef(TigTree,0);

};


#endif /* TIGTREE_H*/
