// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGMANAGER_H
#define TIGMANAGER_H


#include <string>
#include <vector>

#include <TFolder.h>
#include <TObjArray.h>
#include <TList.h>
#include <TThread.h>
#include <TH1.h>
#include <TMidasEvent.h>

#include "TigTree.h"
#include "TigEvent.h"
#include "TigDetector.h"
#include "TigScaler.h"
#include "TigUnpack.h"
#include "TigMidasHandler.h"

#include <RQ_OBJECT.h>


class TigManager
{
  RQ_OBJECT("TigManager")
    
    public:
  TigManager(void);
  virtual ~TigManager(void);

  static TigManager& Instance(void);
  int AnaEventID(){return mAnaEventID;}
  bool ConfigFromRootFile(std::string filename);
  void ConnectFormulas();
  void DeleteHistos(void);
  void DeleteFormulas(void);
  void DeleteTrees(void);
  bool FlushTreeBuffers(int pCutoff);
  void GenerateTrees(bool trees);
  std::vector<TigDataObject*> GetDataObjects(std::string tree);
  std::vector<std::string> GetDetectors(std::string tree);
  bool GetGenerateTrees(){return mGenerateTrees;}
  std::vector<std::string> GetHistos(std::string tree);
  const char* GetOutFile(){ return mOutFile.c_str();}
  int GetOutfileSetting(){return mSingleOutfile;}
  const char * GetOutPostfix(){return mOutPostfix.c_str();}
  const char* GetOutPrefix(){ return mOutPrefix.c_str();}
  TList * GetRunFiles(){return mRunFiles;}
  std::string GetTreeName(int index);
  int GuiInterval(){return mGuiInterval;}
  TH1 *Histogram(int treeN, int histN);
  void HistoUpdate() ;  // *SIGNAL*
  int HistoUpdateInterval(){return mHistoUpdateInterval;}
  void IncAnaEventID(){mAnaEventID++;}
  bool IsRunning(){return mRunning;}
  bool KeepRunning(){return mKeepRunning;}
  TigMidasHandler *MidasHandler(){return mMidasHandler;}
  void ParseCalibrationFile(std::string configFile);
  void ParseFormHistFile(std::string configFile);
  void ParseInputFile(std::string configFile);
  void ProcessTig64(TMidasEvent* pEvent, std::string pBankName);
  void ProcessSignal(TigEvent* pEvent);
  void ProcessScaler(TMidasEvent* pEvent);
  void ProcessScalerData(std::string pBankName, std::vector<int> pValues);
  void ResetHistos(void);
  void ResetTrees(void);
  void RunRoot(int treeN);
  int RunNumber(){return mRunNumber;}
  void Run();
  void SaveHistos();
  void SaveTrees(int treeN);
  void SetGuiInterval(int time);
  void SetHistoUpdateInterval(int time);
  void SetOutFile( char *out){mOutFile = out;}
  void SetOutPostfix( char *out){mOutPostfix = out;}
  void SetOutPrefix( char *out){mOutPrefix = out;}
  void SetRunFiles(TList *list);
  void SetRunNumber(int num){mRunNumber = num;}
  bool SetWriteDet(int treeN, int det);
  void StepMidasFile();
  void Stepped(){mSteps++;}
  void StepTrees(bool step);
  void StopRunning();
  void SwitchOutfileSetting(Int_t index){mSingleOutfile = index-1;}
  int TimeStamp(){return mTimeStamp;}
  std::vector<std::string> TreeNames();
  void Write(void){}

  bool mHasConfig;
  bool mHasRootConfig;
  bool mHasMidasFiles;
  bool mHasRootFiles;

 protected:					//----- protected ------------------
  // void CreateWindow(void);
  bool OpenTreeOutFile(int treeN);
  // TigDetector *ParseDetector(istream& pStream);
  void ParsePrimitive(std::string pToken, istream& pStream);
  // TigScaler *ParseScaler(istream& pStream);
  void RunHisto();
  void ParseTree(istream& pStream);

  int mAnaEventID; // global event ID used by all trees
  int mBuffer;
  bool mGenerateTrees;
  int mGuiInterval;
  int mHistoUpdateInterval;
  // TigFrame *mFrame;
  bool mKeepRunning;
  TigMCSUnpack* mMCSUnpack;
  TigMidasHandler *mMidasHandler;
  std::string mOutFile;
  std::string mOutPrefix;
  std::string mOutPostfix;
  TFile *mRootInfile;
  TFile *mRootOutfile;
  TList *mRunFiles;
  bool mRunning;
  int mRunNumber;
  bool mSingleOutfile;
  int mSteps;
  int mTimeStamp; // MIDAS event time stamp used by all trees
  std::vector<TigTree*> mTrees;
  TigUnpack* mUnpack;

 private:		
  TigManager(const TigManager& );
  TigManager& operator = ( const TigManager& );

  // ClassDef(TigManager,0)
};


#endif /* TIGMANAGER_H */
