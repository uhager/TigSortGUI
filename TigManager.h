// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGMANAGER_H
#define TIGMANAGER_H

//class TigManager;
//class TigDependentObject;
//class TigTree;
//class TigObject;

#include <string>
#include <vector>
#include <TFolder.h>
#include <TObjArray.h>
#include <TList.h>
#include <TThread.h>
#include <TH1.h>
#include <TMidasEvent.h>
#include <TigTree.h>
#include <TigEvent.h>
#include <TigDetector.h>
#include <TigScaler.h>
#include <TigUnpack.h>
#include <TigMidasHandler.h>
#include <RQ_OBJECT.h>
//#include <TigFrame.h>

class TigManager //: public TObject
{
  RQ_OBJECT("TigManager")
public:						//----- public ---------------------
							TigManager(void);
virtual						~TigManager(void);

static TigManager&			Instance(void);
 int AnaEventID(){return mAnaEventID;}
 bool ConfigFromRootFile(string filename);
  void ConnectFormulas();
 void DeleteHistos(void);
 void DeleteFormulas(void);
 void DeleteTrees(void);
 bool FlushTreeBuffers(int pCutoff);
 void GenerateTrees(bool trees);
 vector<TigDataObject*> GetDataObjects(string tree);
 vector<string> GetDetectors(string tree);
 bool GetGenerateTrees(){return mGenerateTrees;}
 vector<string> GetHistos(string tree);
 const char* GetOutFile(){ return mOutFile.c_str();}
 int GetOutfileSetting(){return mSingleOutfile;}
 const char * GetOutPostfix(){return mOutPostfix.c_str();}
 const char* GetOutPrefix(){ return mOutPrefix.c_str();}
 TList * GetRunFiles(){return mRunFiles;}
 string GetTreeName(int index);
 int GuiInterval(){return mGuiInterval;}
 TH1 *Histogram(int treeN, int histN);
 void HistoUpdate() ;  // *SIGNAL*
 int HistoUpdateInterval(){return mHistoUpdateInterval;}
 void IncAnaEventID(){mAnaEventID++;}
 bool IsRunning(){return mRunning;}
 bool KeepRunning(){return mKeepRunning;}
 TigMidasHandler *MidasHandler(){return mMidasHandler;}
void ParseCalibrationFile(string configFile);
void ParseFormHistFile(string configFile);
void ParseInputFile(string configFile);
 void ProcessTig64(TMidasEvent* pEvent, string pBankName);
void ProcessSignal(TigEvent* pEvent);
 void ProcessScaler(TMidasEvent* pEvent);
 void ProcessScalerData(string pBankName, vector<int> pValues);
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
 vector<string> TreeNames();
void Write(void){}

 bool mHasConfig;
 bool mHasRootConfig;
 bool mHasMidasFiles;
 bool mHasRootFiles;

protected:					//----- protected ------------------
 // void CreateWindow(void);
 bool OpenTreeOutFile(int treeN);
 // TigDetector *ParseDetector(istream& pStream);
 void ParsePrimitive(string pToken, istream& pStream);
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
 string mOutFile;
 string mOutPrefix;
 string mOutPostfix;
 TFile *mRootInfile;
 TFile *mRootOutfile;
 TList *mRunFiles;
 bool mRunning;
 int mRunNumber;
 bool mSingleOutfile;
 int mSteps;
 int mTimeStamp; // MIDAS event time stamp used by all trees
 vector<TigTree*> mTrees;
 TigUnpack* mUnpack;

private:					//----- private --------------------
							TigManager(const TigManager& pToCopy);
TigManager&					operator = (const TigManager& pToCopy);

// ClassDef(TigManager,0)
};


#endif /* TIGMANAGER_H */
