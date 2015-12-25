// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>
//#include <strstream>
#include <TROOT.h>
#include <TFolder.h>
#include <TFile.h>
#include <TSystemFile.h>
#include <TKey.h>
#include <TigManager.h>
#include <TigTree.h>

using namespace std;

static TigManager* gInstance = NULL;


//---- TigManager
TigManager::TigManager(void)
  : mBuffer(1000)
  ,mAnaEventID(0)
  ,mRunNumber(0)
  ,mUnpack(NULL)
  ,mMCSUnpack(NULL)
  , mOutPrefix("asm")
  ,mOutPostfix("")
  ,mOutFile("")
  ,mRootOutfile(NULL)
  ,mRunFiles(NULL)
  ,mHasConfig(false)
  ,mHasRootConfig(false)
  ,mHasMidasFiles(false)
  ,mHasRootFiles(false)
  ,mKeepRunning(false)
  ,mRunning(false)
  ,mGenerateTrees(true)
  ,mSingleOutfile(1)
  ,mGuiInterval (100)
  , mHistoUpdateInterval(500)
{

  if ( gInstance == NULL) {
    //    cout << "[TigManager] constructor" << endl;
       gInstance = this;
    mMidasHandler = new TigMidasHandler(this);
    //    CreateWindow();
    mMidasHandler->SetGuiInterval(mGuiInterval);
    mMidasHandler->SetHistoUpdateInterval(mHistoUpdateInterval);
  }
  else
    {
      cout << "[TigManager::TigManager]  Manager is a singleton class and can only be constructed once." << endl;
      exit(1);
    }
}

//---- ~TigManager
TigManager::~TigManager(void)
{
  //  this->Clear();
  delete mRootOutfile;
  delete mRunFiles;
  delete mUnpack;
  delete mMidasHandler;
  //  delete mFrame;
}

bool 
TigManager::ConfigFromRootFile(string filename)
{
  if (gSystem->AccessPathName(filename.c_str()) ){
    cout << "[TigManager::ConfigFromRootFile] can't find file " << filename << endl;
    return false;
  }
  mRootInfile = TFile::Open(filename.c_str());
TList *keys =  mRootInfile->GetListOfKeys();
 TIter iter(keys);
 TKey *key;
 while ( key = (TKey*)iter() ){
   TObject *obj = mRootInfile->Get(key->GetName());
   if (obj->IsA() == TTree::Class()){
     TTree *pTree = (TTree*)obj;
     TigTree*	 tree = new TigTree;
       tree->SetParent(this);
     bool check = tree->ConfigFromRootTree(pTree);
     if (check) {
       tree->Initialize();
       tree->GenerateTree = mGenerateTrees;
       mTrees.push_back(tree);
     }
   }
 }
   mRootInfile->Close();
  mHasRootConfig = true;
  mHasConfig = false;
 return true;
}

//---- ConnectFormulas
void
TigManager::ConnectFormulas()
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++)  (*trees)->ConnectFormulas();
}

//---- DeleteHistos
void
TigManager::DeleteHistos(void)
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++)  (*trees)->DeleteHistos();
}

//---- DeleteHistos
void
TigManager::DeleteFormulas(void)
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++)  (*trees)->DeleteFormulas();
  //  this->ConnectFormulas();
}

//---- DeleteTrees
void
TigManager::DeleteTrees(void)
{
  //  cout << "[TigManager::DeleteTrees] " << endl;
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
    delete (*trees);
  }
  mTrees.clear();	
}

//---- Instance
TigManager&
TigManager::Instance(void)
{
  if ( !gInstance)
    {
      cout << "[TigManager::Instance]  Must construct a manager before calling Instance." << endl;
      exit(1);
    }
	
  return *gInstance;
}

//---- FlushTreeBuffers
bool
TigManager::FlushTreeBuffers(int pCutoff)
{
  int test = 0;
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) test += (*trees)->FlushBuffer(pCutoff);
  if (test>0) return false;
  return true;
}

//---- GenerateTrees
void
TigManager::GenerateTrees(bool tr)
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->GenerateTree = tr;
  mGenerateTrees = tr;
}


//---- GetDetectors
 vector<string> 
 TigManager::GetDetectors(string tree)
 {
   vector<string> result;
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
    if ((*trees)->Name() == tree) {
      result = (*trees)->DetectorNames();
      return result;
    }
  }
  return result;
 }

//---- GetDataObjects
 vector<TigDataObject*> 
 TigManager::GetDataObjects(string tree)
 {
   vector<TigDataObject*> result;
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
    if ((*trees)->Name() == tree) {
      result = (*trees)->DataObjects();
      return result;
    }
  }
  return result;
 }

//---- GetDetectors
 vector<string> 
 TigManager::GetHistos(string tree)
 {
   vector<string> result;
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
    if ((*trees)->Name() == tree) {
      result = (*trees)->HistoNames();
      return result;
    }
  }
  return result;
 }

string 
TigManager::GetTreeName(int index)
{
  if (index>mTrees.size()){
    cout << "[TigManager::GetTreeName] index out of range " << endl;
    return "";
    }
  return mTrees.at(index)->Name();
}

//---- Histogram
TH1 *
TigManager::Histogram(int treeN, int histN)
{
  if (treeN>mTrees.size()){
    cout << "[TigManager::Histogram] index out of range " << treeN << endl;
    return NULL;
    }
  return mTrees.at(treeN)->Histogram(histN);
}

//---- HistoUpdate
void
TigManager::HistoUpdate() 
{ 
  if (!mGenerateTrees) Emit("HistoUpdate()" ) ; 
} 


bool
TigManager::OpenTreeOutFile(int treeN)
{
  //  cout << "[TigManager::OpenTreeOutFile] " << treeN << endl;
  if ( treeN +1 > mTrees.size() ) {
    cout << "[TigManager::OpenTreeOutFile] requested tree number " << treeN << " larger than number of trees " << mTrees.size() << endl;
    return false;
  }
  if (mRootOutfile){
  delete mRootOutfile;
  mRootOutfile = NULL;
  }
  mRootOutfile  = new TFile(mOutFile.c_str(),"RECREATE");
  if (! mRootOutfile) return false;
  mRootOutfile->cd();
  if (treeN < 0 ){
   vector<TigTree*>::iterator trees;
  for (trees= mTrees.begin(); trees < mTrees.end(); trees++) (*trees)->SetDirectory(mRootOutfile);
  }
  else mTrees.at(treeN)->SetDirectory(mRootOutfile);
  return true;
}

//---- ParseCalibrationFile
void
TigManager::ParseCalibrationFile(string inputFile)
{
  ParseFormHistFile(inputFile);
}

//---- ParseFormHistFile
void
TigManager::ParseFormHistFile(string inputFile)
{
  string line, token;
  ifstream input(inputFile.c_str());
  // this->Clear();
  // cout << "Parsing formula/histograms/cut file...";

  getline(input,line);
  while ( input)
    {
      istringstream stream(line.c_str());
     
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //comment or empty
	}
      else if (token == "tree"){
	string treeName;
       	stream >> treeName;
	vector<TigTree*>::iterator trees;
	for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
	  if ((*trees)->Name() == treeName) (*trees)->ParseFormHist(input);
	}
      }
      else 
	cout << "[TigManager::ParseFormHistFile] Unknown token " << token << endl;
	  
      if ( input)
	getline(input,line);
    }
  // cout << "TigManager formula/histogram parsing done.\n";

}


//---- ParseInputFile
void
TigManager::ParseInputFile(string configFile)
{
  //  char *inputFile = NULL;
  string inputFile = "-"; 
  if (configFile.compare("-") != 0) inputFile = configFile;
  else inputFile = getenv("TIGSORT_INPUT");
  if (inputFile.compare("-") == 0)
    {
      cout << "[TigManager::ParseInputFile]  No 'TIGSORT_INPUT' input file specified ";
      exit(1);
    }

  string line;
  ifstream input(inputFile.c_str());
  //  this->Clear();
  // cout << "Parsing TigSort specification file...";

  getline(input,line);
  while ( input)
    {
      string token;
      istringstream stream(line.c_str());
     
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //comment or empty
	}
      else if (token == "buffer"){
       	stream >> mBuffer;
	for (int i=0;i<mTrees.size();i++) mTrees.at(i)->SetBuffer(mBuffer);
      }
      else 
	this->ParsePrimitive(token, input);
	  
      if ( input)
	getline(input,line);
    }
  // cout << "TigManager input parsing done.\n";
  mHasConfig = true;
  mHasRootConfig = false;
}


//---- ParsePrimitive
void
TigManager::ParsePrimitive(string pToken, istream& pStream)
{
  if ( pToken.compare("tree") == 0)
    this->ParseTree(pStream);
  else
    {
      cout << "[TigManager::ParsePrimitive]  Unknown primitive token: " << pToken << endl;
    }
}


//---- ParseTree
void
TigManager::ParseTree(istream& pStream)
{
  string line;
  bool	 bail = false;
  TigTree*	 tree = new TigTree;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      istringstream stream(line.c_str());
      string token;
      stream >> token;
      if ( token == "" || token[0] == '#')
	{
	  //	  cout << "[TigManager::ParseTree] comment " << line << endl;
	  //comment or blank
	}
      else if ( token.compare("description") == 0)
	{
	  stream >> token;
	  tree->ChangeDescription(token);
	}
      else if ( token.compare("end") == 0)
	bail = true;
      else if ( token.compare("name") == 0)
	{	
	  stream >> token;
	  cout << ".(" << token << ").";
	  tree->ChangeName(token);
	}
      else if ( token.compare("detector") == 0)
	{
	  //	  cout << "[TigManager::ParseTree] << found new detector " << endl;
	  tree->ParseDetector(pStream);
	  if (!mUnpack) mUnpack = new TigUnpack;	
	  //	  cout << "[TigManager::ParseTree] << added new detector " << endl;
	}
      else if ( token.compare("scaler") == 0)
	{
	  tree->ParseScaler(pStream);
	  if (!mMCSUnpack) mMCSUnpack = new TigMCSUnpack;	
	}
      else{
	cout << "[TigManager::ParseTree] unknown line " << line << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }

  tree->SetBuffer(mBuffer);	
  tree->Initialize();
  tree->SetParent(this);
  tree->GenerateTree = mGenerateTrees;
  mTrees.push_back(tree);
}

//---- ProcessScaler
void
TigManager::ProcessScaler(TMidasEvent* pEvent)
{
  if (mMCSUnpack)
    {
      //  cout << "Processing scaler" << endl;
      WORD*	data;
      int	bankLength, bankType;
      void *ptr;
      int check;
      vector<string> requestedBanks;
      string bankName;

      mTimeStamp = pEvent->GetTimeStamp();

      vector<TigTree*>::iterator trees;
      for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->Banks(requestedBanks);

      for (int i = 0; i < requestedBanks.size(); i++)
	{
	  bankName = requestedBanks.at(i);
	  bankLength = pEvent->LocateBank(NULL,bankName.c_str(),&ptr);
	  data = (WORD*) ptr;
	  vector<int> values = mMCSUnpack->ProcessData(data, bankLength); 
	  this->ProcessScalerData(bankName,values);
	  //      cout << "[TigManager::ProcessScaler] after unpacking: index = " << index << " - bankLength = " << bankLength << endl;
	}
      bool fillTrees = false;
      for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
	if ( (*trees)->FillTree()) fillTrees = true;
      }
      if (fillTrees) mAnaEventID++;
    }
}

//---- ProcessScaler
void
TigManager::ProcessScalerData(string pBankName, vector<int> pValues)
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++){
    (*trees)->ProcessScaler(pBankName,pValues);
  }
}

//---- ProcessSignal
void						
TigManager::ProcessSignal(TigEvent* pEvent)
{
  vector<TigTree*>::iterator trees;

  for (trees= mTrees.begin(); trees < mTrees.end(); trees++)
    (*trees)->ProcessSignal(pEvent);
}

//---- ProcessTig64
void
TigManager::ProcessTig64(TMidasEvent* pEvent, string pBankName)
{
  if (mUnpack)
    {
      // cout << "Processing Tig64" << endl;
      WORD*	data;
      // UInt_t	bankLength;
      int	bankLength, bankType;
      long	index = 0;
      void *ptr;
      int check;

      mTimeStamp = pEvent->GetTimeStamp();
      bankLength = pEvent->LocateBank(NULL,pBankName.c_str(),&ptr);
      data = (WORD*) ptr;

      while (index < bankLength)
	{
	  check = mUnpack->ProcessData(data+index, bankLength-index); 
	  if (check < 0) break;
	  else index += check;
	  //      cout << "[TigManager::ProcessTig64] after unpacking: index = " << index << " - bankLength = " << bankLength << endl;
	}
    }
}

//---- ResetHistos
void
TigManager::ResetHistos(void)
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->ResetHistos();
}

//---- ResetTrees
void
TigManager::ResetTrees(void)
{
  vector<TigTree*>::iterator trees;
  for ( trees=mTrees.begin(); trees< mTrees.end(); trees++) (*trees)->ResetTree();
}

//---- RunFromRoot
void
TigManager::RunRoot(int treeN)
{
  if (treeN+1 > mTrees.size() ){ 
    cout << "[TigManager::RunRoot] selected tree number larger than number of trees" << endl;
    return;
  }
  if (!mRunFiles->GetSize()){ 
    cout << "[TigManager::RunRoot] no input files" << endl;
    return;
  }
  if (mHasRootConfig && !mHasRootFiles){
    cout << "[TigManager::RunRoot] configured for reading root files, but no root files loaded" << endl;
    return;
  }
   TSystemFile *file;
   TString fname;
    mKeepRunning = true;
    TIter next(mRunFiles);
    //    vector<TigTree*>::iterator trees;
    mRunning = true;
    if (mGenerateTrees){
      bool check = OpenTreeOutFile(treeN);
      if (!check) return;
    }
    while ( (file=(TSystemFile*)next())  && (mKeepRunning) ) {
      fname = file->GetName();
      //      cout << "\n running " << fname << endl;
      (mTrees.at(treeN))->AddInputFile(fname);
      }
    mTrees.at(treeN)->ResetTree();
    mTrees.at(treeN)->InitInputChainAddresses();
    //	FlushTreeBuffers(0);
    mTrees.at(treeN)->RunChain();
  cout << endl;
  if (mGenerateTrees)  this->SaveTrees(treeN);
  mAnaEventID = 0; 
  mRunning = false;
}




//---- RunHisto
void
TigManager::RunHisto()
{
   TSystemFile *file;
   TString fname;
    mKeepRunning = true;
    TIter next(mRunFiles);
    mRunning = true;
    bool check = true;


    while ( (file=(TSystemFile*)next())  && (mKeepRunning) ) {
      fname = file->GetName();
      //  cout << "\n running " << fname << endl;
      int runN =  mMidasHandler->OpenMidasFile(fname.Data());
      if  (runN > -1) {
	mMidasHandler->ProcessMidasFile(fname.Data(),0);
	FlushTreeBuffers(0);

      }
    }

  cout << endl;
  mAnaEventID = 0; 
  mRunning = false;
}


//---- RunTree
void
TigManager::Run()
{
   /*
  mRootOutfile  = new TFile(mOutFile.c_str(),"RECREATE");
  mRootOutfile->cd();
  vector<TigTree*>::iterator trees;
  for (trees= mTrees.begin(); trees < mTrees.end(); trees++) (*trees)->SetDirectory(mRootOutfile);
  */
  if (!mRunFiles->GetSize()){ 
    cout << "[TigManager::Run] no input files" << endl;
    return;
  }
  else if(mHasConfig && !mHasMidasFiles){
    cout << "[TigManager::Run] configured for reading MIDAS files, but no MIDAS files loaded" << endl;
    return;
  } 

  ResetTrees();
  mAnaEventID = 0; 
  if (!mGenerateTrees ){
    if (mHasConfig){
      this->RunHisto();
      return;
    }
  }
  // cout << "[TigManager::Run] single file " << mSingleOutfile << " name " << mOutFile << endl; 

   TSystemFile *file;
   TString fname;
    mKeepRunning = true;
    TIter next(mRunFiles);
    mRunning = true;
    bool check = true;
    if (mSingleOutfile == 0)  check = OpenTreeOutFile(-1);
    if (!check) {
      cout << "[TigManager::RunTree] can't open output file " << mRootOutfile << endl;
      return;
    }

   while ( (file=(TSystemFile*)next())  && (mKeepRunning) ) {
      fname = file->GetName();
      // cout << "\n [TigManager::Run] running " << fname << endl;
      int runN =  mMidasHandler->OpenMidasFile(fname.Data());
      if  (runN > -1) {
	if (mSingleOutfile == 1) {
	  ResetTrees();
	  stringstream ss;
	  ss << mOutPrefix << runN << mOutPostfix << ".root" ;
	  mOutFile = ss.str();
	  check = OpenTreeOutFile(-1);
	  if (!check) {
	    cout << "[TigManager::RunTree] can't open output file " << mRootOutfile << endl;
	    return;
	  }
	}
	mMidasHandler->ProcessMidasFile(fname.Data(),0);
	FlushTreeBuffers(0);
	mMidasHandler->CloseMidasFile();
	if ( mSingleOutfile == 1) SaveTrees(-1);
      
      }
    }
    if (mSingleOutfile == 0 ) SaveTrees(-1);
  cout << endl;
  mAnaEventID = 0; 
  mRunning = false;
}

void 
TigManager::SaveHistos()
{

  if (mRootOutfile){
    delete mRootOutfile;
    mRootOutfile = NULL;
  }
  mRootOutfile  = new TFile(mOutFile.c_str(),"RECREATE");
  if (! mRootOutfile) {
    cerr << "[TigManager::SaveHistos] can't open file " << mOutFile << endl;
    return ;
  }
  mRootOutfile->cd();

    vector<TigTree*>::iterator trees;
    for (trees= mTrees.begin(); trees < mTrees.end(); trees++)   (*trees)->WriteHistos();

  mRootOutfile->Write();
  mRootOutfile->Close();
  delete mRootOutfile;
  mRootOutfile = NULL;

}

void 
TigManager::SaveTrees(int treeN)
{
  if ( treeN +1 > mTrees.size() ) {
    cout << "[TigManager::SaveTrees] requested tree number " << treeN << " larger than number of trees " << mTrees.size() << endl;
    return;
  }

  if (mRootOutfile){
    if (treeN<0){
    vector<TigTree*>::iterator trees;
    for (trees= mTrees.begin(); trees < mTrees.end(); trees++){
      (*trees)->Write();
      (*trees)->ResetTree();
    }
    }
    else {
      mTrees.at(treeN)->Write();
      mTrees.at(treeN)->ResetTree();
    }
  mRootOutfile->Write();
  mRootOutfile->Close();
  delete mRootOutfile;
  mRootOutfile = NULL;
  }
}


void 
TigManager::SetGuiInterval(int time)
{
  mGuiInterval = time;
  mMidasHandler->SetGuiInterval(mGuiInterval);
}

 void 
TigManager::SetHistoUpdateInterval(int time)
 {
  mHistoUpdateInterval = time;
  mMidasHandler->SetHistoUpdateInterval(mHistoUpdateInterval);
 }


//---- SetRunFiles
void 
TigManager::SetRunFiles(TList *list)
{
  if (mRunFiles)  delete mRunFiles;
  mRunFiles = (TList*)list->Clone();
}


//---- SetWriteDet
bool
TigManager::SetWriteDet(int treeN, int det)
{
  //  cout << "[TigManager::SetWriteDet] treeN " << treeN << " - det " << det << endl;
  if (treeN+1 > mTrees.size() ) return false; 
  return mTrees.at(treeN)->ToggleWrite(det);

}

//---- StepMidasFile
void 
TigManager::StepMidasFile()
{
  
  if (!mRunFiles){ 
    cout << "[TigManager::RunTree] no input MIDAS files" << endl;
    return;
  }
  //  ResetTrees();
  // this->StepTrees(true);
    TSystemFile *file;
   TString fname;
    mKeepRunning = true;
    TIter next(mRunFiles);
    file=(TSystemFile*)next();
    mRunning = true;
    bool check = true;
    mSteps = 0;
      fname = file->GetName();
    while (  (mKeepRunning) ) {
      //      cout << "\n running " << fname << endl;
      //  int runN =  mMidasHandler->OpenMidasFile(fname.Data());
      // if  (runN > -1) {
	int check = mMidasHandler->ProcessMidasFile(fname.Data(),0);
	//	cout << "[TigManager::StepMidasFile] check1 " << check << endl;  
	if (check ==1) {
	  //	  cout << "[TigManager::StepMidasFile] file ended "  << endl;  
	  bool check = FlushTreeBuffers(1);
	  //  cout << "[TigManager::StepMidasFile] check2 " << check << endl;  
	  if (check) {
	  mMidasHandler->CloseMidasFile();
	  file = (TSystemFile*)next();
	  // cout << "[TigManager::StepMidasFile] next file  "  << endl;  
	  }
	}	//      }
    }
    //	FlushTreeBuffers();

    //  cout << endl;
    //  this->StepTrees(false);
  mRunning = false;
}

//---- StepTrees
void 
TigManager::StepTrees(bool step)
{
    vector<TigTree*>::iterator trees;
  for (trees= mTrees.begin(); trees < mTrees.end(); trees++)(*trees)->Step(step);
}


//---- StopRunning
void 
TigManager::StopRunning()
{
  mKeepRunning = false;
  // if (mRootOutfile){
  //   mRootOutfile->Write();
  //   delete mRootOutfile;
  //   cout << endl;
  // }
  mRunning = false;
}


vector<string>
TigManager::TreeNames()
{
  vector<string> result;
    vector<TigTree*>::iterator trees;
  for (trees= mTrees.begin(); trees < mTrees.end(); trees++) result.push_back((*trees)->Name());
  return result;
}
