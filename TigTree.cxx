// part of TigSortGUI
// author: Ulrike Hager

#include <sstream>
#include <iostream>
#include <TigTree.h>
#include <TigManager.h>
#include <TigObject.h>

static const long gIndexOffset = 4; //offset in tree for TigSort EventID, Trigger EventID, runN, timestamp


//---- TigAssemble
TigAssemble::TigAssemble(void)
  : mEventID(0)
{
  mAnaEventID = TigManager::Instance().AnaEventID();
  mRunNumber = TigManager::Instance().RunNumber();
  mTimeStamp = TigManager::Instance().TimeStamp();
  TigManager::Instance().IncAnaEventID();
}

//---- ~TigAssemble
TigAssemble::~TigAssemble(void)
{
  vector<double*>::iterator pInt;
  for (pInt=mEventData.begin(); pInt<mEventData.end(); pInt++) delete[] (*pInt);
  mEventData.clear();
  //  cout << "[TigAssemble::~TigAssemble] deleted" << endl;
}

//---- AddData
void
TigAssemble::AddData(int pDetector, int pChannel, double pValue)
{
  double* data = mEventData.at(pDetector);
  data[pChannel] = pValue;
}

//---- AddDetector
void
TigAssemble::AddDetector(int pSignals)
{
  mEventData.push_back(new double[pSignals]);
  ::memset(mEventData.back(), -1,  pSignals * sizeof(double));
}

//---- AddWaveform
void
TigAssemble::AddWfDetector()
{
  vector<short> toAdd;
  mWaveforms.push_back(toAdd);
}

//---- AddWaveform
void
TigAssemble::AddWfData(int pDet,vector<short> data)
{
  mWaveforms.at(pDet) = data;
}

//---- Data
double*
TigAssemble::Data(int pDet)
{
  return mEventData.at(pDet);
}

//---- TigTree
TigTree::TigTree(void)
  : mTrigEventID(0)
  , mAnaEventID(0)
  , mRunNumber(0)
  ,mScEventID(0)
  , mHasEventData(false)
    //  , mAssembled(NULL)
  , mTree(NULL)
  , mBuffer(1000)
  ,mTimeStamp(0)
  ,GenerateTree(true)
  ,mStepFile(false)
  ,mInputChain(0)
  ,mCurrentEntry(0)
{
}

//---- ~TigTree
TigTree::~TigTree(void)
{
  //  cout << "[TigTree::~TigTree] " << mName  << endl;
  if (mInputChain) delete mInputChain;
  vector<TigDataObject*>::iterator detector;
  for (detector= mObjects.begin(); detector < mObjects.end(); detector++) delete (*detector);
  vector<TigAssemble*>::iterator assemble;
  for (assemble= mAssembled.begin(); assemble < mAssembled.end(); assemble++) delete (*assemble);
  mObjects.clear();
  mAssembled.clear();
  DeleteHistos();
  DeleteFormulas();
  //  mTree->Delete("all");
}

//---- AddDetector
void
TigTree::AddDetector(TigDetector* pToAdd)
{
  mObjects.push_back(pToAdd);
}

//---- AddEvent
int
TigTree::AddEvent(int pEventID)
{
  int result;
  for (result=0; result<mAssembled.size(); result++){
    if ( (mAssembled.at(result))->EventID() == pEventID ){
      //cout << "[TigTree::AddEvent] found event " << (mAssembled.at(result))->eventID << " at " << result <<endl;
      return result;
    }
  }
  mAssembled.push_back(new TigAssemble());
  //  cout << "[TigTree::AddEvent]  new event " << mAssembled.size() << endl;
  mAssembled.back()->SetEventID(pEventID);
  vector<TigDataObject*>::iterator detector;
  for (detector= mObjects.begin(); detector < mObjects.end(); detector++) {
    if ( (*detector)->ObjType == "Detector" ) mAssembled.back()->AddDetector( ((TigDetector*)(*detector))->DataLength());
  }
  for (int i = 0; i<mWfHistos.size(); i++) mAssembled.back()->AddWfDetector();
  result = mAssembled.size() - 1;
  return result;
}


//---- AddInputFile
void 
TigTree::AddInputFile(TString pFile)
{
  mInputChain->Add(pFile);

}

//---- AddScaler
void
TigTree::AddScaler(TigScaler* pToAdd)
{
  mObjects.push_back(pToAdd);
}

//---- Banks
void
TigTree::Banks(vector<string> &pRequested)
{
  for (int i = 0; i<mObjects.size(); i++){
    if (mObjects.at(i)->ObjType == "Scaler"){
      TigScaler *scaler = (TigScaler*)(mObjects.at(i));
      pRequested.push_back( scaler->Bank() );
    }}
}

//---- BranchCount
long
TigTree::BranchCount(void) const
{
  //add signals for eventIDs
  return mObjects.size() + gIndexOffset ;
}

//---- ConfigFromRootTree
bool 
TigTree::ConfigFromRootTree(TTree *pTree)
{
  //  cout << "[TigTree::ConfigFromRootTree] " << pTree->GetName() << endl;
  this->ChangeName(pTree->GetName() );
  if (mInputChain) delete mInputChain;
  mInputChain = new TChain(pTree->GetName());
  TObjArray * branches = pTree->GetListOfBranches();
  for (int i = 0; i < branches->GetEntries(); i++){
    TBranch *branch = (TBranch*)branches->At(i);
    int what = 0;
    string name = branch->GetName();
    if (  (name.find("AnalyserEventID") == 0) || (name.find("TrigEventID") == 0) || (name.find("MidasTimeStamp") == 0) || (name.find("RunNumber") == 0)  ) continue;
    if (  name.find("_hits") != string::npos ){
      what = 3;
      size_t pos = name.find("_hits");
      name = name.substr(0,pos);
    }
    else if (  name.find("_channel") != string::npos ){
      what = 1;
      size_t pos = name.find("_channel");
      name = name.substr(0,pos);
    }
    else if (  name.find("_value") != string::npos ){
      what = 2;
      size_t pos = name.find("_value");
      name = name.substr(0,pos);
    }
    bool exists = true;
    TigDataObject *dobj =  this->FindDataObject(name);
    if (dobj ==NULL) {
      //      cout << "[TigTree::ConfigFromRootTree] new object " << name << endl;
      exists = false;
      dobj = new TigDataObject();
      //  pTree->SetBranchAddress(name.c_str(),&(dobj->Data));
      dobj->SetParent(this);
      dobj->ChangeName(name);
      if (!dobj->Initialize()){
	delete dobj;
	continue;
      }
    }
    if (what == 0) {
      dobj->SetNumData(1);
      dobj->SetDataLength(1);
    }
    else if (what ==1) dobj->SetNumData(2);
    else if (dobj->DataSize() < 2) dobj->SetNumData(1);
    if (exists == false) mObjects.push_back(dobj);
  }
  //  cout << "[TigTree::ConfigFromRootTree] " << pTree->GetName() << " finished objects"<< endl;
  this->Initialize();
  return true;
}

//---- ConnectFormulas
void
TigTree::ConnectFormulas()
{
  //  cout << "[TigTree::ConnectFormulas]" << endl;
  vector<TigDataObject*> depend;
  depend.insert(depend.begin(),mObjects.begin(),mObjects.end());
  depend.insert(depend.end(),mHistos.begin(), mHistos.end());
  //  depend.insert(depend.end(),mScHistos.begin(), mScHistos.end());
  vector<TigDataObject*>:: iterator iter;
  //  for (int i=0; i<mFormulas.size(); i++){
  for (iter = depend.begin(); iter != depend.end();){
    //   cout << "[TigTree::ConnectFormulas] object " << (*iter)->Name() << " descr " << (*iter)->Description() << endl;
    bool deleteMe = false;
    vector<pair<string,int> > needed = (*iter)->NeededParameters();
    //  cout << "[TigTree::ConnectFormulas] # needed " << needed.size()  << endl;
    for (int j=0; j<needed.size(); j++) {
      //      cout << "[TigTree::ConnectFormulas] needed " << needed.at(j).first << " - ch/v " << needed.at(j).second  << endl;
      if (deleteMe) continue;
      vector<TigDataObject*>::iterator iObj;
      for (iObj = mObjects.begin(); iObj < mObjects.end(); iObj++){
	if ( (needed.at(j).first).compare( (*iObj)->Name() ) == 0) {
	  if ((*iObj)->DataSize() < needed.at(j).second ){
	    cerr << "[TigTree::ConnectFormulas] DataInput " << needed.at(j).first << " for formula " << (*iter)->Name() << " has wrong size" <<endl;
	    deleteMe = true;
	    continue;
	  }
	  
	  (*iter)->InputData.at(j) = ((*iObj)->Data.at( needed.at(j).second ));
	  //	  cout << "[TigTree::ConnectFormulas] connected input " << (*iter)->Name() << " to output " << (*iObj)->Name() << " pointers: in " << (*iter)->InputData.at(j) << " out " << (*iObj)->Data.at( needed.at(j).second ) << endl;
	  ((*iter)->InputUpdated).at(j) = ((*iObj)->IsUpdated) ;
	  (*iter)->IncreaseDataLength((*iObj)->DataLength())  ;
	  //	  cout << "[TigTree::ConnectFormulas] found det " <<  (*iObj)->Name() << endl;
	  break;
	}
      }
      if (iObj == mObjects.end()) {
	cerr << "[TigTree::ConnectFormulas] can't find input " << needed.at(j).first << " for formula " << (*iter)->Name() << endl;
	deleteMe = true;
      }
    }

    needed = (*iter)->NeededCuts();
    //    cout << "[TigTree::ConnectFormulas] # needed " << needed.size()  << endl;
    for (int j=0; j<needed.size(); j++) {
      //       cout << "[TigTree::ConnectFormulas] needed " << needed.at(j).first << " - ch/v " << needed.at(j).second  << endl;
      if (deleteMe) continue;
      vector<TigDataObject*>::iterator iObj;
      for (iObj = mObjects.begin(); iObj < mObjects.end(); iObj++){
	if ( (needed.at(j).first).compare( (*iObj)->Name() ) == 0) {
	  if ((*iObj)->DataSize() < needed.at(j).second ){
	    cerr << "[TigTree::ConnectFormulas] DataInput " << needed.at(j).first << " for cut " << (*iter)->Name() << " has wrong size" <<endl;
	    deleteMe = true;
	    continue;
	  }
	  (*iter)->CutData.at(j) = ((*iObj)->Data.at( needed.at(j).second ));
	  //	     cout << "[TigTree::ConnectFormulas] found det " <<  (*iObj)->Name() << endl;
	  break;
	}
      }
      if (iObj == mObjects.end()) {
	cerr << "[TigTree::ConnectFormulas] can't find cut " << needed.at(j).first << " for  " << (*iter)->Name() << endl;
	deleteMe = true;
      }
    }
    //    (*iter)->SizeDetermined = true;
    if (deleteMe)  {
      //  cout << "[TigTree::ConnectFormulas] delete me  " <<  (*iter)->Name() << endl;
      DeleteObject((*iter)->Name());
      //      delete * iter;
      // cout << "[TigTree::ConnectFormulas] deleted me  " << endl;
      iter = depend.erase(iter);
      // cout << "[TigTree::ConnectFormulas] erased me  "  << endl;
    }
    else ++iter;
  }
  // cout << "[TigTree::ConnectFormulas] done" << endl;
}

//---- DeleteHistos
void 
TigTree::DeleteFormulas()
{
  //  cout << "[TigTree::DeleteFormulas]" << endl;
  // vector<TigDependentObject*>::iterator obj;
  // for (obj=mFormulas.begin(); obj<mFormulas.end(); ){
  //   delete * obj;
  //   obj = mFormulas.erase(obj);
  // }
  if (mObjects.size() == 0 ) return;
  vector<TigDataObject*>::iterator iObj;
  for (iObj = mObjects.begin(); iObj != mObjects.end();){
    if ( (*iObj)->ObjType == "Formula" ) {
      //      cout << "[TigTree::DeleteFormulas] deleting " << (*iObj)->Name()<< endl;
      delete (*iObj);
      //      cout << "[TigTree::DeleteFormulas] deleted " << endl;
      iObj = mObjects.erase(iObj);
      //      cout << "[TigTree::DeleteFormulas] erased " << endl;
    }
    else ++iObj;
  }
  //  cout << "[TigTree::DeleteFormulas] cleared mFormulas, now reconnecting" << endl;
  this->ConnectFormulas();
  //  cout << "[TigTree::DeleteFormulas]  reconnected" << endl;
}

//---- DeleteHistos
void 
TigTree::DeleteHistos()
{
  //  cout << "[TigTree::DeleteHistos]" << endl;
  vector<TigHistoObject*>::iterator obj;
  for (obj=mHistos.begin(); obj < mHistos.end(); obj++) delete (*obj);
  mHistos.clear();
  vector<TigWaveformHisto*>::iterator obj2;
  for (obj2=mWfHistos.begin(); obj2 !=mWfHistos.end(); ++obj2)   delete (*obj2);
  mWfHistos.clear();

}

//---- DeleteObject
void 
TigTree::DeleteObject(string name)
{
  //  cout << "[TigTree::DeleteObject]" << endl ;
  vector<TigDataObject*>::iterator obj1;
  for (obj1=mObjects.begin(); obj1 < mObjects.end(); obj1++){
    if ( name.compare( (*obj1)->Name() ) == 0) {
      //      cout << "[TigTree::DeleteObject] found object"  << endl ;
      delete (*obj1);
      // cout << "[TigTree::DeleteObject] deleted"  << endl ;
      mObjects.erase(obj1);
      //cout << "[TigTree::DeleteObject] erased"  << endl ;
      return;
    }
  }
  vector<TigHistoObject*>::iterator obj2;
  for (obj2=mHistos.begin(); obj2<mHistos.end(); obj2++) {
    if ( name.compare( (*obj2)->Name() ) == 0) {
      //   cout << "[TigTree::DeleteObject] found object"  << endl ;
      delete (*obj2);
      // cout << "[TigTree::DeleteObject] deleted"  << endl ;
      mHistos.erase(obj2);
      //  cout << "[TigTree::DeleteObject] erased"  << endl ;
      return;
    }
  }
  //  cout << "[TigTree::DeleteObject] " << name << " not found" << endl ;

}

//---- DetectorNames
vector<string> 
TigTree::DetectorNames()
{
  vector<string> result;
  for (int i = 0; i<mObjects.size(); i++) result.push_back(mObjects[i]->Name());
  return result;
}

//---- FillTree
bool
TigTree::FillTree()
{
  //  cout << "[TigTree::FillTree()] " << mName << endl;
  if ( !mHasEventData ) return false;
  mRunNumber = TigManager::Instance().RunNumber();
  mAnaEventID = TigManager::Instance().AnaEventID();
  mTimeStamp = TigManager::Instance().TimeStamp();
  if (GenerateTree) mTree->Fill();  
  else {
    for (int i =0; i<mHistos.size(); i++) if (mHistos.at(i)->WhatToDo() == SCALER_HISTO ) mHistos.at(i)->Evaluate();
  }
  for (int i = 0; i<mObjects.size(); i++) {
    if (mObjects.at(i)->ObjType == "Scaler" ) mObjects.at(i)->Reset();
  }
  mScEventID++;
  mHasEventData = false;
}

bool
TigTree::FillTree(TigAssemble* pAssembled)
{
  //  cout << "[TigTree::FillTree(TigAssemble* pAssembled)] " << mName << endl;
  mAnaEventID = pAssembled->AnaEventID();
  mTrigEventID = pAssembled->EventID();
  mRunNumber = pAssembled->RunNumber();
  mTimeStamp = pAssembled->TimeStamp();
  /* have to find a better way to match detectors to assembled...
     if (mDetectors.size() != pAssembled->Size() ){
     cerr << "[TigTree::FillTree] detector size mismatch" << endl;
     return 0;
     }
  */
  //   cout << "[TigTree::FillTree] detector loop " << endl;;
  for (int i =0; i<mObjects.size(); i++){
    if (mObjects.at(i)->ObjType == "Detector") {
      double* detData = pAssembled->Data(i);
      TigDetector *det = (TigDetector*)(mObjects.at(i));
      bool process = det->ProcessEvent(detData);
      if (process == false ) cerr << "[TigTree::FillTree] event size mismatch" << endl;
    }
    else if (mObjects.at(i)->ObjType == "Formula") {
      mObjects.at(i)->Evaluate();
    }
    //    cout << "[TigTree::FillTree] DataSize " << mObjects.at(i)->Name() << " - " << mObjects.at(i)->Data->Size() << endl; 
  }
  //   cout << "[TigTree::FillTree] fill tree " << mName<< endl;;
  bool updatedHisto = false;
  if (GenerateTree) mTree->Fill();
  else {
    for (int i =0; i<mHistos.size(); i++) if (mHistos.at(i)->WhatToDo() != SCALER_HISTO ) if (mHistos.at(i)->Evaluate()) updatedHisto = true;
    for (int i=0; i<mWfHistos.size(); i++) if (mWfHistos.at(i)->Evaluate(pAssembled->Waveform(i))) updatedHisto = true;
  }
  //   cout << "[TigTree::FillTree] reset detectors " << endl;;
  for (int i = 0; i<mObjects.size(); i++) mObjects[i]->Clear();
  //   cout << "[TigTree::FillTree] done" << endl;
  if (mStepFile && updatedHisto)  mParent->StopRunning();
  return 1;
}

TigDataObject*
TigTree::FindDataObject(string name)
{
  vector<TigDataObject*>::iterator obj1;
  for (obj1=mObjects.begin(); obj1<mObjects.end(); obj1++){
    if ( name.compare( (*obj1)->Name() ) == 0) return (*obj1);
  }
  return NULL;
}

bool 
TigTree::FindObject(string name)
{
  vector<TigDataObject*>::iterator obj1;
  for (obj1=mObjects.begin(); obj1<mObjects.end(); obj1++){
    if ( name.compare( (*obj1)->Name() ) == 0)    return true;
  }
  vector<TigHistoObject*>::iterator obj2;
  for (obj2=mHistos.begin(); obj2<mHistos.end(); obj2++) {
    if ( name.compare( (*obj2)->Name() ) == 0)  return true;
  }
  return false;
}

//---- FindScalerBank
bool 
TigTree::FindScalerBank(string pBankName)
{
  for (int i = 0; i<mObjects.size(); i++) {
    if (mObjects.at(i)->ObjType == "Scaler"){
      TigScaler *scaler = (TigScaler*)(mObjects.at(i));
      if ( pBankName.compare(scaler->Bank()) == 0 ) return true;
    }}
  return false;
}

//---- FlushBuffer
int
TigTree::FlushBuffer(int pCutoff)
{
  int counter = 0;
  vector<TigAssemble*>::iterator assembled;
  for (assembled= mAssembled.begin(); assembled < mAssembled.end();) {
    this->FillTree(*assembled);
    delete (*assembled);
    assembled = mAssembled.erase(assembled);
    counter++;
    if (pCutoff>0 && counter >= pCutoff) break; 
  }
  if (mAssembled.size()>0) return 1;
  return 0;
  //  mAssembled.clear();
}

//---- GetAddress
int 
TigTree::GetAddress(string pDetName, int pChannel)
{
  int result = -1;
  vector<TigDataObject*>::iterator obj3;
  for (obj3=mObjects.begin(); obj3<mObjects.end(); obj3++) {
    if ((*obj3)->ObjType == "Detector"){
      TigDetector *det = (TigDetector*)(*obj3);
      if ( pDetName.compare( det->Name() ) == 0)  {
	result = det->GetAddress(pChannel);
	return result;
      }}
  }
  return result;
}


//---- GetDataSize
int
TigTree::GetDataSize(string dataObj)
{
  //  cout << "[TigTree::GetDataSize] for " << dataObj << endl; 
  vector<TigDataObject*>::iterator iObj;
  for (iObj = mObjects.begin(); iObj < mObjects.end(); iObj++){
    if (dataObj.compare( (*iObj)->Name() ) == 0)   return (*iObj)->DataSize();
  }
  return 0;
}

int 
TigTree::GetScalerIndex(string pName)
{
  int result;
  for (int i = 0; i<mObjects.size(); i++) {
    if (mObjects.at(i)->ObjType == "Scaler"){
      TigScaler *scaler = (TigScaler*)(mObjects.at(i));
      result = scaler->Index(pName);
      if (result > -1) return result;
    }} 
  return -1;
}

//---- HistoNames
vector<string> 
TigTree::HistoNames()
{
  vector<string> result;
  for (int i = 0; i<mHistos.size(); i++) result.push_back(mHistos[i]->Name());
  for (int i = 0; i<mWfHistos.size(); i++) result.push_back(mWfHistos[i]->Name());
  return result;
}

//---- Histogram
TH1 *
TigTree::Histogram(int histN)
{
  //  cout << "[TigTree::Histogram]" << endl;
  vector<TigHistoObject*> histos;
  histos.insert(histos.begin(),mHistos.begin(), mHistos.end());
  histos.insert(histos.end(),mWfHistos.begin(), mWfHistos.end());
  if ( histN > histos.size() ){
    cerr << "[TigTree::Histogram] index out of range " << histN << endl;
    return NULL;
  }
  //  cout << "[TigTree::Histogram] found "<<  mHistos.at(histN)->Name() << endl;
  return histos.at(histN)->Histogram();
}

//---- IniInputChainAddresses
void 
TigTree::InitInputChainAddresses()
{
  //  cout << "[TigTree::InitInputChainAddresses]" << endl;
  mInputChain->SetBranchAddress("RunNumber",&mRunNumber);
  mInputChain->SetBranchAddress("AnalyserEventID",&mAnaEventID);
  mInputChain->SetBranchAddress("MidasTimeStamp",&mTimeStamp);
  if (mInputChain->FindBranch("TrigEventID")) mInputChain->SetBranchAddress("TrigEventID",&mTrigEventID);
  if (mInputChain->FindBranch("ScalerEventID")) mInputChain->SetBranchAddress("ScalerEventID",&mScEventID);

  vector<TigDataObject*>::iterator detector;
  char bName[256],hitName[256];
  for (detector= mObjects.begin(); detector < mObjects.end(); detector++)
    {
      if (((*detector)->ObjType).compare("DataObject") == 0){
	if ( ((*detector)->DataSize() == 1) && ((*detector)->DataLength() == 1) ){
	  mInputChain->SetBranchAddress((*detector)->Name().c_str(),&((*detector)->Data.at(0)->second[0]));      
	}
	else{
	  sprintf(hitName,"%s_hits",(*detector)->Name().c_str());
	  mInputChain->SetBranchAddress(hitName,&((*detector)->Data.at(0)->first));
	  if ((*detector)->DataSize() == 1){
	    sprintf(bName,"%s_value",(*detector)->Name().c_str());
	    mInputChain->SetBranchAddress(bName,((*detector)->Data.at(0)->second));      
	  }
	  else{
	    sprintf(bName,"%s_channel",(*detector)->Name().c_str());
	    mInputChain->SetBranchAddress(bName,((*detector)->Data.at(0)->second));
	    sprintf(bName,"%s_value",(*detector)->Name().c_str());
	    mInputChain->SetBranchAddress(bName,((*detector)->Data.at(1)->second));
	  }
	}
      }
      /*      if ((*detector)->DataSize() == 1) mInputChain->SetBranchAddress((*detector)->Name().c_str(), &((*detector)->Data.at(0) ) );
      else if ((*detector)->DataSize() == 2){
        sprintf(bName,"%s_channel",(*detector)->Name().c_str());
        mInputChain->SetBranchAddress(bName,&((*detector)->Data.at(0)));
        sprintf(bName,"%s_value",(*detector)->Name().c_str());
        mInputChain->SetBranchAddress(bName,&((*detector)->Data.at(1)));
      }
      */
    }
}


//---- Initialize
void
TigTree::Initialize(void)
{
  //  cout << "[TigTree::Initialize]" << endl;
  mTree = new TTree(mName.c_str(), mDescription.c_str());
  mTree->SetDirectory(0);
  // if (!(mParent->mHasRootConfig)){
  mTree->Branch("RunNumber",&mRunNumber,"RunNumber/I");
  mTree->Branch("AnalyserEventID",&mAnaEventID,"AnalyserEventID/I");
  mTree->Branch("MidasTimeStamp",&mTimeStamp,"MidasTimeStamp/I");
  if (mObjects.size()>0) mTree->Branch("TrigEventID",&mTrigEventID,"TrigEventID/I");
  //}
  vector<TigDataObject*>::iterator detector;
  for (detector= mObjects.begin(); detector < mObjects.end(); detector++)
    {
      //      cout << "[TigTree::Initialize] " << (*detector)->Name() << " data size " << (*detector)->DataSize() << endl;
      if ( (*detector)->mWriteToTree ){
	if ((*detector)->ObjType == "Scaler") {
	  if (mTree->FindBranch("ScalerEventID") == 0) mTree->Branch("ScalerEventID",&mScEventID,"ScalerEventID/I");
	}

	char hitName[256], bName[256], bLeaf[256];
	if ( ((*detector)->DataSize() == 1) && ((*detector)->DataLength() == 1) ){
	  sprintf(bLeaf,"%s/D",(*detector)->Name().c_str());
	  mTree->Branch((*detector)->Name().c_str(),&((*detector)->Data.at(0)->second[0]), bLeaf);      

	}
	else{
	  sprintf(hitName,"%s_hits",(*detector)->Name().c_str());
	  sprintf(bLeaf,"%s_hits/I",(*detector)->Name().c_str());
	  mTree->Branch(hitName,&((*detector)->Data.at(0)->first), bLeaf);
	  if ((*detector)->DataSize() == 1){
	    sprintf(bName,"%s_value",(*detector)->Name().c_str());
	    sprintf(bLeaf,"%s_value[%s]/D",(*detector)->Name().c_str(),hitName);
	    mTree->Branch(bName,((*detector)->Data.at(0)->second), bLeaf);      
	  }
	  else{
	    sprintf(bName,"%s_channel",(*detector)->Name().c_str());
	    sprintf(bLeaf,"%s_channel[%s]/D",(*detector)->Name().c_str(),hitName);
	    mTree->Branch(bName,((*detector)->Data.at(0)->second), bLeaf);
	    sprintf(bName,"%s_value",(*detector)->Name().c_str());
	    sprintf(bLeaf,"%s_value[%s]/D",(*detector)->Name().c_str(),hitName);
	    mTree->Branch(bName,((*detector)->Data.at(1)->second), bLeaf);
	  }}
      }
    }
  //  mTree->MakeSelector("TigSelector");
  mTree->SetAutoSave();
  mScEventID = 0;
  //  cout << "[TigTree::Initialize] done" << endl;
}

  //---- ParseCalibration
  void 
    TigTree::ParseCalibration(istream& pStream)
  {
    TigCalibration *calib = new TigCalibration();
    string line, token;
    bool	 bail = false;

    getline(pStream,line);
    while ( pStream && !bail)
      {
	bool check = calib->ParseInput(line);
	if (!check){
	  istringstream stream(line.c_str());	
	  stream >> token;
	  if ( token.compare("end") == 0)
	    bail = true;
	  else
	    cerr << "[TigTree::ParseCalibration] Unknown token " << token << endl;
	}
	if ( !bail)
	  getline(pStream,line);
      }
    calib->SetParent(this);
    bool check = calib->Initialize();
    if (check) mObjects.push_back(calib);
    else delete calib;
  }

//---- ParseCut
void 
TigTree::ParseCut(istream& pStream)
{
  TigCut *cut = new TigCut();
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = cut->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
      stream >> token;
      if ( token.compare("end") == 0)
	bail = true;
      else
	cerr << "[TigTree::ParseCut] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  cut->SetParent(this);
   bool check = cut->Initialize();
   if (check)  mObjects.push_back(cut);
   else delete cut;
}

//---- ParseDetector
void
TigTree::ParseDetector(istream& pStream)
{
  // cout << "[TigTree::ParseDetector]" << endl;
  string line;
  bool	 bail = false;
  TigDetector*	 det = new TigDetector();

  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = det->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
	string token;
      stream >> token;
      if ( token.compare("end") == 0)
	bail = true;
      else if ( token.compare("signals") == 0)
	{
	  bool bailSignals = false;
	  getline(pStream,line);
	  while ( pStream && !bailSignals)
	    {
	      bool check = det->ParseSignal(line);
	      if (!check) {
		bailSignals = true;
		//cout << "[TigTree::ParseDetector] " << det->Name() << " end of signals" << endl;
	      }	
	      if ( !bailSignals)
		getline(pStream,line);
	}
    }

      else
	cerr << "[TigTree::ParseDetector] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }

  det->SetParent(this);
   bool check = det->Initialize();
   if (check)  mObjects.push_back(det);
   else delete det;
   //   cout << "[TigTree::ParseDetector] new detector: " << det->Name() << " #signals: " << det->DataLength() << endl;
}


//---- ParseFormHist
void 
TigTree::ParseFormHist(istream& pStream)
{
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      istringstream stream(line.c_str());	
      stream >> token;
      if ( token == "" || token[0] == '#') {}	  //comment or blank
      else if ( token.compare("end") == 0)
	bail = true;
       else if ( token.compare("formula") == 0)
	{
	  this->ParseFormula(pStream);
	}	  
       else if ( token.compare("histo") == 0)
	{
	  this->ParseHisto(pStream);
	}
       else if ( token.compare("scalerhisto") == 0)
	{
	  this->ParseScalerHisto(pStream);
	}
       else if ( token.compare("sorter") == 0)
	{
	  this->ParseSorter(pStream);
	}	  
       else if ( token.compare("cut") == 0)
	{
	  this->ParseCut(pStream);
	}	  
       else if ( token.compare("calibration") == 0)
	{
	  this->ParseCalibration(pStream);
	}	  
       else if ( token.compare("waveform") == 0)
	{
	  this->ParseWaveform(pStream);
	}	  
      else if ( token.compare("lookup") == 0)
	{
	  this->ParseLookup(pStream);
	}	  
       else 
	cerr << "[TigTree::ParseFormHist] Unknown token " << token << endl;
      if ( !bail)
	getline(pStream,line);
    }
  // cout << "[TigTree::ParseFormHist] parsing done" << endl;
  this->ConnectFormulas();
 }

//---- ParseFormula
void 
TigTree::ParseFormula(istream& pStream)
{
  //  cout << "[TigTree::ParseFormula] "  << endl;
  TigFormula *form = new TigFormula();
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = form->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
      stream >> token;
      if ( token.compare("end") == 0)
	bail = true;
      else
	cerr << "[TigTree::ParseFormula] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  form->SetParent(this);
   bool check = form->Initialize();
   if (check)  mObjects.push_back(form);
   else delete form;
   //  cout << "[TigTree::ParseFormula] done "  << endl;
}

void 
TigTree::ParseLookup(istream& pStream)
{
  //  cout << "[TigTree::ParseFormula] "  << endl;
  TigLookupTable *form = new TigLookupTable();
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = form->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
      stream >> token;
      if ( token.compare("end") == 0)
	bail = true;
      else
	cerr << "[TigTree::ParseLookup] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  form->SetParent(this);
   bool check = form->Initialize();
   if (check)  mObjects.push_back(form);
   else delete form;
   //  cout << "[TigTree::ParseFormula] done "  << endl;
}


//---- ParseHisto
void 
TigTree::ParseHisto(istream& pStream)
{
  TigHisto *histo = new TigHisto();
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = histo->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
      stream >> token;
      if ( token.compare("end") == 0)
	bail = true;
      else
	cerr << "[TigTree::ParseHisto] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  histo->SetParent(this);
   bool check = histo->Initialize();
   if (check)  mHistos.push_back(histo);
   else delete histo;
}

//---- ParseScaler
void
TigTree::ParseScaler(istream& pStream)
{
  TigScaler* scaler = new TigScaler();
  string line;
  bool	 bail = false;

  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = scaler->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
	string token;
	stream >> token;
	if ( token.compare("end") == 0)
	  bail = true;
	else if ( token.compare("signals") == 0)
	  {
	    bool bailSignals = false;
	    getline(pStream,line);
	    while ( pStream && !bailSignals)
	      {
		bool check = scaler->ParseSignal(line);
	      if (!check) {
		bailSignals = true;
		//	cout << "[TigTree::ParseScaler] " << scaler->Name() << " end of signals" << endl;
	      }	
	      if ( !bailSignals)
		getline(pStream,line);
	      }
	  }
	else
	  cerr << "[TigDetector::ParseScaler] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  scaler->SetParent(this);
  bool check = scaler->Initialize();
   if (check)  mObjects.push_back(scaler);
   else delete scaler;

}

//---- ParseScalerHisto
void 
TigTree::ParseScalerHisto(istream& pStream)
{
  TigScalerHisto *histo = new TigScalerHisto();
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = histo->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
      stream >> token;
      if ( token.compare("end") == 0)
	bail = true;
      else
	cerr << "[TigTree::ParseScalerHisto] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  histo->SetParent(this);
   bool check = histo->Initialize();
   if (check)  mHistos.push_back(histo);
   else delete histo;
}

//---- ParseSorter
void 
TigTree::ParseSorter(istream& pStream)
{
  //  cout << "[TigTree::ParseSorter]" << endl;
  TigSortObject *sort = new TigSortObject();
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = sort->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
	stream >> token;
	if ( token.compare("end") == 0)
	  bail = true;
	else
	cerr << "[TigTree::ParseSorter] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  sort->SetParent(this);
  bool check = sort->Initialize();
  if (check) mObjects.push_back(sort);
  else delete sort;
}

//---- ParseWaveform
void 
TigTree::ParseWaveform(istream& pStream)
{
  TigWaveformHisto *histo = new TigWaveformHisto();
  string line, token;
  bool	 bail = false;
  getline(pStream,line);
  while ( pStream && !bail)
    {
      bool check = histo->ParseInput(line);
      if (!check){
	istringstream stream(line.c_str());	
      stream >> token;
      if ( token.compare("end") == 0)
	bail = true;
      else
	cerr << "[TigTree::ParseHisto] Unknown token " << token << endl;
      }
      if ( !bail)
	getline(pStream,line);
    }
  histo->SetParent(this);
   bool check = histo->Initialize();
   if (check)  mWfHistos.push_back(histo);
   else delete histo;
}


//---- ProcessScaler
bool
TigTree::ProcessScaler(string pBankName, vector<int> pValues)
{
  for (int i = 0; i<mObjects.size(); i++) {
    if (mObjects.at(i)->ObjType == "Scaler") {
	TigScaler *scaler = (TigScaler*)(mObjects.at(i));
					 
	if ( pBankName.compare(scaler->Bank()) == 0 ) {
	  scaler->ProcessEvent(pValues);
	  mHasEventData = true;
	}}
  }
  return mHasEventData;
}

//---- ProcessSignal
bool
TigTree::ProcessSignal(TigEvent* pEvent)
{
  //  cout << "[TigTree::ProcessSignal] ";
  // cout << "mAssembled.size() " << mAssembled.size() << endl;
  bool result = false;
  int assembledNo = -1;
  for (int i = 0; i<mObjects.size(); i++) {
    if (mObjects.at(i)->ObjType == "Detector"){
      TigDetector *det = (TigDetector*)(mObjects.at(i));
      bool check = det->FindSignal(pEvent);
    if (check){  
      if (assembledNo <0) assembledNo = this->AddEvent( pEvent->EventID() );
      double pData[] = {-1,-1};
      det->ProcessSignal(pEvent, pData);
      if ( (pData[0] + 1) < 0.000001 ) continue;
      mAssembled.at(assembledNo)->AddData(i,pData[0],pData[1]);
      result = true;
    }
  }
  }
  //  if (!GenerateTree){
  //    bool wfCheck = false;
  for (int i = 0; i<mWfHistos.size(); i++) {
    if (mWfHistos.at(i)->ProcessSignal(pEvent) ) { //wfCheck = true;
       if (assembledNo <0) assembledNo = this->AddEvent( pEvent->EventID() );
      mAssembled.at(assembledNo)->AddWfData(i, pEvent->Waveform()); 
     }
    //  if (wfCheck) mParent->Stepped();
  }
  while (mAssembled.size() > mBuffer)
    {
      TigAssemble* assemble = mAssembled.at(0);
      this->FillTree(assemble);
      delete assemble;
      mAssembled.erase(mAssembled.begin());
    }

  return result;
}

void 
TigTree::ResetTree()
{
  //  cout << "[TigTree::ResetTree]" << endl;
  if (mTree){
  delete mTree;
  mTree = NULL;
  }
  Initialize();
}

void 
TigTree::ResetHistos()
{
  //  cout << "[TigTree::ResetHisto]" << endl;
  for (int i = 0; i<mHistos.size(); i++) mHistos[i]->Reset();
  for (int i = 0; i<mWfHistos.size(); i++) mWfHistos[i]->Reset();
}

//---- RunChain
bool 
TigTree::RunChain()
{
  Int_t entries = mInputChain->GetEntries();
  for (int j =mCurrentEntry; j<entries; j++){
    //    cout << "[TigTree::RunChain] entry " << j << endl;
    mInputChain->GetEntry(j);
    for (int i = 0; i<mObjects.size(); i++) mObjects.at(i)->Evaluate();
    bool updatedHisto = false;

    if (GenerateTree) mTree->Fill();
    else {
      for (int i =0; i<mHistos.size(); i++) if (mHistos.at(i)->WhatToDo() != SCALER_HISTO ) if (mHistos.at(i)->Evaluate()) updatedHisto = true;
    }
    for (int i = 0; i<mObjects.size(); i++) {
      if (mObjects.at(i)->ObjType == "Formula") mObjects.at(i)->Clear(); // WARNING! was just Formulas! Important? YES. IT IS!
    }
   if (mStepFile && updatedHisto) {
	  mParent->HistoUpdate();
    mCurrentEntry =  j+1;
    return 1;
  }
  if((j%mParent->GuiInterval())==0)
	{
	   gSystem->ProcessEvents();
	   // mParent->HistoUpdate();
	}      
      if((j%mParent->HistoUpdateInterval())==0)
	{
	  //   gSystem->ProcessEvents();
	  mParent->HistoUpdate();
	}  
      if((j%5000)==0)
	{
	  cout << "." ;
	  cout.flush();
	}    
      if (!mParent->KeepRunning()) {
	mCurrentEntry =  j+1;
	return 1;
      }
  }
  return 1;
}

void
TigTree::Write()
{
  FlushBuffer(0);
  mTree->Write();
}

//---- ToggleWrite
bool
TigTree::ToggleWrite(int det)
{
  //  cout << "[TigTree::ToggleWrite] det " << det << endl;
  if (det > mObjects.size() )  {
    cerr << "[TigTree::ToggleWrite] out of range" << endl; 
    return false;
  }
 mObjects.at(det)->mWriteToTree = !(mObjects.at(det)->mWriteToTree);
    /*
  cout << "[TigTree::ToggleWrite] detName " <<  mDetectors.at(det)->Name() << endl;
  cout << "[TigTree::ToggleWrite] mWriteToTree before " << mDetectors.at(det)->mWriteToTree << endl;
  mDetectors.at(det)->mWriteToTree = !(mDetectors.at(det)->mWriteToTree);
  cout << "[TigTree::ToggleWrite] mWriteToTree after " << mDetectors.at(det)->mWriteToTree << endl;
    */
 
  return true;
}

void 
TigTree::WriteHistos()
{
  for (int i =0; i<mHistos.size(); i++) mHistos.at(i)->Write();
  for (int i =0; i<mWfHistos.size(); i++) mWfHistos.at(i)->Write();
}
