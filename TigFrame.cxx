// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include <TSystemFile.h>
#include <TMath.h>
#include <TGInputDialog.h>

#include "TigFrame.h"

using std::string;
using std::vector;

enum CommandIDs {
  LOAD_CONFIG_FILE 
  ,LOAD_ROOT_FILE
  ,SELECT_MIDAS_FILES
  ,SELECT_ROOT_FILES
  ,LOAD_CALIB_FILE
  ,LOAD_FORMHIST_FILE
  , RESET_ALL_HISTOS
  ,DELETE_ALL_HISTOS
  ,DELETE_ALL_FORMULAS
  ,DELETE_ALL_INOUT
  , DELETE_TREES
, HISTO_UPDATE_FREQ
, GUI_UPDATE_FREQ
};

const char *allfiles[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            "Text files",    "*.[tT][xX][tT]",
                            0,               0 };
const char *rootfiles[] = {"ROOT files",    "*.root",
                            0,               0 };
const char *midasfiles[] = { "MIDAS files",   "*.mid*" ,
			     "MIDAS files",   "*.mid",
			     "MIDAS files",   "*.mid.bz2",
			     "All files",     "*",
                            0,               0 };
const char *macrofiles[] = { "ROOT macros",   "*.C",
                            0,               0};
const char *tigsortfiles[] = { "TigSort files",    "*.tigsort",
                            "All files",     "*",
                              "Text files",    "*.[tT][xX][tT]",
                            "Text files",    "*.dat",
                            0,               0 };

TigFrame::TigFrame(const TGWindow *pWindow, UInt_t pWidth, UInt_t pHeight, UInt_t pOptions)
: TGMainFrame(pWindow, pWidth, pHeight, pOptions)
, mCanvas(NULL)
  //,mTimerTime(300)
{
  //    std::cout << "[TigFrame::TigFrame]" << std::endl;

    mManager = new  TigManager;
    mManager->Connect("HistoUpdate()","TigFrame", this, "UpdateHisto()");

    // TString dir =  "/home/ulrike/Projects/sharc/s1201/";
    // mFileInfo.fIniDir = StrDup(dir);
  this->SetCleanup(kDeepCleanup);

  this->ChangeBackground(kWhite);

   mMenuFile = new TGPopupMenu(gClient->GetRoot());
   mMenuConfig = new TGPopupMenu(gClient->GetRoot());
  mDetConfMenu = new TGPopupMenu(gClient->GetRoot());
  mMenuReset = new TGPopupMenu(gClient->GetRoot());
  mMenuCalib = new TGPopupMenu(gClient->GetRoot());
  mMenuPrefs = new TGPopupMenu(gClient->GetRoot());
   mMenuFile->AddEntry("Select midas files", SELECT_MIDAS_FILES);
   mMenuFile->AddEntry("Select root files", SELECT_ROOT_FILES);
   mDetConfMenu->AddEntry("Load from .tigsort file", LOAD_CONFIG_FILE);
   mDetConfMenu->AddEntry("Load from .root file", LOAD_ROOT_FILE);
   mMenuCalib->AddEntry("Load from .tigsort file", LOAD_CALIB_FILE);
   mMenuConfig->AddPopup("DAQ addresses" , mDetConfMenu, NULL);
   //   mMenuConfig->AddPopup("Calibration" , mMenuCalib, NULL);
   mMenuConfig->AddEntry("Formulas/Histograms", LOAD_FORMHIST_FILE);
   mMenuReset->AddEntry("Reset all histograms", RESET_ALL_HISTOS);
   mMenuReset->AddEntry("Delete all histograms", DELETE_ALL_HISTOS);
    mMenuReset->AddEntry("Delete all formulas/cuts/histograms", DELETE_ALL_INOUT);
   mMenuReset->AddEntry("Clear all (start over)", DELETE_TREES);
   mMenuPrefs->AddEntry("Set histogram update frequency", HISTO_UPDATE_FREQ);
   mMenuPrefs->AddEntry("Set GUI update frequency (when running)", GUI_UPDATE_FREQ);
   mMenuBar = new TGMenuBar(this,1,1,kHorizontalFrame);
   mMenuBar->AddPopup("Configuration", mMenuConfig, NULL);
   mMenuBar->AddPopup("Data", mMenuFile, NULL);
   mMenuBar->AddPopup("Reset", mMenuReset, NULL);
   mMenuBar->AddPopup("Preferences", mMenuPrefs, NULL);
    mMenuConfig->Connect("Activated(Int_t)", "TigFrame", this, "HandleMenu(Int_t)");
    mMenuFile->Connect("Activated(Int_t)", "TigFrame", this, "HandleMenu(Int_t)");
    mMenuReset->Connect("Activated(Int_t)", "TigFrame", this, "HandleMenu(Int_t)");
    mMenuPrefs->Connect("Activated(Int_t)", "TigFrame", this, "HandleMenu(Int_t)");
  mMenuFile->DisableEntry(0);
     this->AddFrame(mMenuBar,  new TGLayoutHints(kLHintsTop | kLHintsExpandX) );


     mTab = new TGTab(this,400,600);

   TGCompositeFrame *mainTab = mTab->AddTab("Main");
   this->AddFrame(mTab, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0,0,0,0)); 
  
    mFrameMain = new TGHorizontalFrame(mainTab);
    mainTab->AddFrame(mFrameMain, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 0,0,0,0)); 
   mFrameOutEntry = new TGVerticalFrame(mFrameMain, 230, 50, kVerticalFrame | kFixedWidth);
   mFrameTreeMain = new TGVerticalFrame(mFrameMain, 60, 20);
   mFrameButtons = new TGHorizontalFrame(mFrameOutEntry, 60, 20);
   mFrameTree = new TGHorizontalFrame(mFrameTreeMain, 60, 20);
   mFrameLists = new TGHorizontalFrame(mFrameTreeMain, 60, 20);
   mLHButtons = new TGLayoutHints(kLHintsTop, 2, 2, 3, 0);
   mLHOutEntry = new TGLayoutHints(kLHintsTop | kLHintsLeft  | kLHintsExpandY, 0, 1, 1, 1);
   mLHDetList = new TGLayoutHints(kLHintsTop | kLHintsLeft  | kLHintsExpandX  | kLHintsExpandY, 1, 1, 1, 1);
 
  mFrameMain->SetBackgroundColor(kWhite);

     mGFOutEntry = new TGGroupFrame(mFrameOutEntry,"Output file name",kVerticalFrame);

    mOutPrefixEntry = new TGTextEntry(mGFOutEntry, new TGTextBuffer(6));
    //   mOutPrefixEntry->SetToolTipText("Output file default prefix");
   mOutPrefixEntry->Resize(100, mOutPrefixEntry->GetDefaultHeight());
   mOutPrefixEntry->SetText(mManager->GetOutPrefix());
  mOutPrefixEntry->Connect("TextChanged(const char*)","TigFrame",this, "MakeOutFile( char *)" );

   mOutPostfixEntry = new TGTextEntry(mGFOutEntry, new TGTextBuffer(6));
   mOutPostfixEntry->Resize(100, mOutPostfixEntry->GetDefaultHeight());
   mOutPostfixEntry->SetText(mManager->GetOutPostfix());
  mOutPostfixEntry->Connect("TextChanged(const char*)","TigFrame",this, "MakeOutFile( char *)" );

    mOutEntry = new TGTextEntry(mGFOutEntry, new TGTextBuffer(6));
    //   mOutEntry->SetToolTipText("Output file default file");
   mOutEntry->Resize(200, mOutEntry->GetDefaultHeight());
   mOutEntry->SetText(mManager->GetOutFile());
   mOutEntry->Connect("TextChanged(const char*)","TigFrame",this, "SetOutFile( char *)" );

   mGFOutEntry->AddFrame(mOutPrefixEntry,mLHButtons);
   mGFOutEntry->AddFrame(mOutPostfixEntry,mLHButtons);
   mGFOutEntry->AddFrame(mOutEntry,mLHButtons);
   //   mGFOutEntry->Resize(100,mGFOutEntry->GetDefaultHeight());

     mBGOutfile = new TGButtonGroup(mFrameOutEntry,"RootTree output file",kVerticalFrame);
     mRBOutfile[0] = new TGRadioButton(mBGOutfile,"All-in-one");
     mRBOutfile[1] = new TGRadioButton(mBGOutfile,"1 file per run");
     mRBOutfile[ mManager->GetOutfileSetting() ]->SetState(kButtonDown);
     mBGOutfile->Connect("Clicked(Int_t)", "TigFrame",this,"SwitchOutfileSetting(Int_t)");

     mBGHisto = new TGButtonGroup(mFrameOutEntry,"Fill trees or histograms",kVerticalFrame);
     mRBHisto[0] = new TGRadioButton(mBGHisto,"Fill Histograms");
     mRBHisto[1] = new TGRadioButton(mBGHisto,"Fill trees");
     mRBHisto[ mManager->GetGenerateTrees() ]->SetState(kButtonDown);
     mBGHisto->Connect("Clicked(Int_t)", "TigFrame",this,"SwitchHistoVsTree(Int_t)");

     mRunButton = new TGTextButton(mFrameButtons,"&Run");
     mRunButton->Connect("Clicked()","TigFrame",this,"Run()");
     mRunButton->SetEnabled(false);
     mFrameButtons->AddFrame(mRunButton, mLHButtons);

     mTBStep = new TGTextButton(mFrameButtons,"Step");
     mTBStep->Connect("Clicked()","TigFrame",this,"Step()");
     mTBStep->SetEnabled(kFALSE);
     mFrameButtons->AddFrame(mTBStep, mLHButtons);

     mTBSaveHistos = new TGTextButton(mFrameButtons,"Save histos");
     mTBSaveHistos->Connect("Clicked()","TigManager",mManager,"SaveHistos()");
     mFrameButtons->AddFrame(mTBSaveHistos, mLHButtons);
     //     mTBSaveHistos->SetEnabled(kFALSE);

     mFrameOutEntry->AddFrame(mGFOutEntry, mLHButtons);
     mFrameOutEntry->AddFrame(mBGOutfile, mLHButtons );
     mFrameOutEntry->AddFrame(mBGHisto, mLHButtons );
     mFrameOutEntry->AddFrame(mFrameButtons, mLHButtons);

     mLBDetectors = new TGListBox(mFrameLists);
     mLBDetectors->Resize(150,100);
     mLBHistos = new TGListBox(mFrameLists);
     mLBHistos->Resize(150,100);
     mLBHistos->Connect("Selected(const char *)","TigFrame",this,"GetHisto(char *)");
     mFrameLists->AddFrame(mLBDetectors, mLHDetList);
     mFrameLists->AddFrame(mLBHistos, mLHDetList);
     mComboTrees = new TGComboBox(mFrameTreeMain);
     mComboTrees->AddEntry("Trees",-1);
     mComboTrees->Selected(-1);
     mComboTrees->Resize(100, 20);
     mComboTrees->Layout();
     mComboTrees->Connect("Selected(const char *)", "TigFrame", this, "GetDetectorList(char *)" );

     mFrameTree->AddFrame(mComboTrees, mLHButtons);
     //     mFrameTree->AddFrame(mCheckTreeCalib, mLHButtons);
     mFrameTreeMain->AddFrame(mFrameTree, mLHButtons);
     mFrameTreeMain->AddFrame(mFrameLists, mLHDetList);

     mFrameMain->AddFrame(mFrameOutEntry,  mLHOutEntry);
     mFrameMain->AddFrame(mFrameTreeMain, mLHDetList);

     


	this->MapSubwindows(); 
	this->Resize(this->GetDefaultSize()); 
	this->MapWindow();

	if (!mCanvas) mCanvas= new TCanvas("TigSortHistogram","TigSortHistogram");
     mCanvas->cd();

}

//---- AssembleOutFileNumbers
void 
TigFrame::AssembleOutFileNumbers(TList * files)
{
	TSystemFile *file;
	TString fname;
	string base;
	TIter next(files);
	while ((file=(TSystemFile*)next())) {
	  fname = file->GetName();
	  base = gSystem->BaseName(fname.Data() );
	  int firstNum = base.find_first_of("0123456789");
	  string numbers = base.substr(firstNum, base.find(".")-firstNum);
	  size_t dash = numbers.find('-');
	  if (dash != string::npos ) {
	    string number = numbers.substr(0, dash);
	    std::istringstream(number) >> firstNum;
	    mRunNumbers.insert(firstNum);
	    number = numbers.substr(dash+1);
	    std::istringstream(number) >> firstNum;
	    mRunNumbers.insert(firstNum);
	  }
	  else {
	    std::istringstream(numbers) >> firstNum;
	  //	  std::cout << base << " - " << firstNum << std::endl;
	    mRunNumbers.insert(firstNum);
	  }
	}
	if (base.find("sort") == 0 ) 	mOutPostfixEntry->SetText("_step2");

	std::stringstream ss;
	if (mRunNumbers.size()>1) ss <<  *mRunNumbers.begin() <<"-" << *mRunNumbers.rbegin();
	else if (mRunNumbers.size()==1) ss << *mRunNumbers.begin();
	mOutFileNumbers = ss.str();
	std::string text = mOutPrefixEntry->GetText() + mOutFileNumbers +  mOutPostfixEntry->GetText() + ".root" ; 
	mOutEntry->SetText(text.c_str());

}



//---- CloseWindow
void	
TigFrame::CloseWindow(void)
{
  delete mMenuBar;
  delete mFrameMain;
  
  if (mCanvas) delete mCanvas;
	
  delete mManager;
  gApplication->Terminate(0);
}


//---- GetDetectorList
void
TigFrame::GetDetectorList(char * na)
{
  string name = na;
  vector<string> dets = mManager->GetDetectors(name);
  mLBDetectors->RemoveAll();
  for (int i = 0; i<dets.size(); i++) mLBDetectors->AddEntry((dets.at(i)).c_str(), i);
  mLBDetectors->Layout();
  GetHistoList(na);
}

void
TigFrame::GetDetectorList(int index)
{
  if (index>-1){
  string name = mManager->GetTreeName(index);
  if (name.size()>0) GetDetectorList((char*)name.c_str());
  }
}

//---- GetHistoList
void
TigFrame::GetHisto(char * na)
{
  //  std::cout << "[TigFrame::GetHisto]" << std::endl;
  mHisto = mManager->Histogram( mComboTrees->GetSelected(), mLBHistos->GetSelected() );
  if ((mHisto) && (mCanvas)) {
    mHisto->Draw();
    mCanvas->Update();
  }
}

//---- GetHistoList
void
TigFrame::GetHistoList(char * na)
{
  string name = na;
  vector<string> dets = mManager->GetHistos(name);
  mLBHistos->RemoveAll();
  for (int i = 0; i<dets.size(); i++) mLBHistos->AddEntry((dets.at(i)).c_str(), i);
  mLBHistos->Layout();
}

void
TigFrame::GetHistoList(int index)
{
  if (index>-1){
  string name = mManager->GetTreeName(index);
  if (name.size()>0) GetHistoList((char*)name.c_str());
  }
}

Int_t
TigFrame::GetUsableHeight()
{
  Int_t result;
  result = this->GetHeight();
  result -= mMenuBar->GetHeight();
  result -= mTab->GetTabHeight();
  result -=5;
  //  result = mTab->GetTabContainer(0)->GetHeight();
  return result;

}

//---- HandleDetFrame
void 
TigFrame::HandleDetFrame(int det, int button)
{
  //  std::cout << "[TigFrame::HandleDetFrame] options: USE_CALIB " << USE_CALIB << ", WRITE_TO_TREE " << WRITE_TO_TREE << std::endl;
  int treeN = mTab->GetCurrent()-1;
  //  std::cout << "[TigFrame::HandleDetFrame] button " << button << std::endl;
  switch (button)
    {
    case WRITE_TO_TREE: {
      //      std::cout << "[TigFrame::HandleDetFrame] write to tree" << std::endl; 
      mManager->SetWriteDet(treeN,det);
    }

    }

}


//---- HandleMenu
void	
TigFrame::HandleMenu(Int_t ID)
{
  switch (ID) 
    {
    case LOAD_CONFIG_FILE:
      mManager->DeleteTrees();
      mFileInfo.fFileTypes = tigsortfiles;
      mFileInfo.fMultipleSelection = false;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&mFileInfo );
      if (mFileInfo.fFilename ){
	std::cout << mFileInfo.fFilename << " - " << mFileInfo.fIniDir<<  std::endl;
	mManager->ParseInputFile(mFileInfo.fFilename);
	mComboTrees->RemoveAll();
	mLBDetectors->RemoveAll();
	vector<string> trees = mManager->TreeNames();
	for (int i = 0; i<trees.size(); i++) mComboTrees->AddEntry((trees.at(i)).c_str(), i);
	if (mManager->GetRunFiles()) mRunButton->SetEnabled(true);
	mComboTrees->Select(0);
	this->GetDetectorList(0);
	this->LayoutTreeTabs();
      }
      break;
    case LOAD_ROOT_FILE: {
      mManager->DeleteTrees();
      mFileInfo.fFileTypes = rootfiles;
      mFileInfo.fMultipleSelection = false;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&mFileInfo );
      if (mFileInfo.fFilename ){
	std::cout << mFileInfo.fFilename << " - " << mFileInfo.fIniDir<<  std::endl;
	mManager->ConfigFromRootFile(mFileInfo.fFilename);
	mComboTrees->RemoveAll();
	mLBDetectors->RemoveAll();
	vector<string> trees = mManager->TreeNames();
	for (int i = 0; i<trees.size(); i++) mComboTrees->AddEntry((trees.at(i)).c_str(), i);
	//	if (mManager->GetRunFiles()) mRunButton->SetEnabled(true);
	mComboTrees->Select(0);
	this->GetDetectorList(0);
	this->LayoutTreeTabs();
      }
    } break;


    case SELECT_MIDAS_FILES: {
      if (!(mManager->mHasRootConfig)){
      mFileInfo.fFileTypes = midasfiles;
      mFileInfo.fMultipleSelection = true;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&mFileInfo );
      if (mFileInfo.fFileNamesList ){
	mRunNumbers.clear();
	mManager->SetRunFiles(mFileInfo.fFileNamesList);
	mManager->mHasRootFiles = false;
	mManager->mHasMidasFiles = true;
	mOutPrefixEntry->SetText("asm");
	AssembleOutFileNumbers(mFileInfo.fFileNamesList);
	if (mManager->mHasConfig) mRunButton->SetEnabled(true);
      }}
    }    break;
    case SELECT_ROOT_FILES: {
      if (mManager->mHasRootConfig){
      mFileInfo.fFileTypes = rootfiles;
      mFileInfo.fMultipleSelection = true;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&mFileInfo );
      if (mFileInfo.fFileNamesList ){
	mRunNumbers.clear();
	mManager->SetRunFiles(mFileInfo.fFileNamesList);
	mManager->mHasRootFiles = true;
	mManager->mHasMidasFiles = false;
    mOutPrefixEntry->SetText("sort");
	AssembleOutFileNumbers(mFileInfo.fFileNamesList);
	mRunButton->SetEnabled(true);
	}
      }
    }    break;

    case LOAD_CALIB_FILE: {
      mFileInfo.fFileTypes = tigsortfiles;
      mFileInfo.fMultipleSelection = false;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&mFileInfo );
      if (mFileInfo.fFilename ){
	std::cout << mFileInfo.fFilename << " - " << mFileInfo.fIniDir<<  std::endl;
	mManager->ParseCalibrationFile(mFileInfo.fFilename); 
	LayoutTreeTabs();
      }
    }      break;
    case LOAD_FORMHIST_FILE: {
      mFileInfo.fFileTypes = tigsortfiles;
      mFileInfo.fMultipleSelection = true;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&mFileInfo );
      if (mFileInfo.fFileNamesList ){
	//	std::cout << mFileInfo.fFilename << " - " << mFileInfo.fIniDir<<  std::endl;
	TSystemFile *file;
	TIter next(mFileInfo.fFileNamesList);
	while ( file=(TSystemFile*)next() )  mManager->ParseFormHistFile(file->GetName()); 
	//	mManager->ConnectFormulas();
	GetDetectorList( mComboTrees->GetSelected() );
	GetHistoList( mComboTrees->GetSelected() );
      LayoutTreeTabs();
      }
    }break;
    case RESET_ALL_HISTOS: {
      mManager->ResetHistos();
      this->UpdateHisto();
    }break;
    case DELETE_ALL_HISTOS: {
      if (mHisto){
	//	delete mHisto;
	mHisto = NULL;
      }
      mManager->DeleteHistos();
      mCanvas->Update();
      this->GetHistoList(0);
    }break;
    case DELETE_ALL_FORMULAS: {
      std::cout << "[TigFrame::HandleMenu] delete formulas" << std::endl;
      if (mHisto){
	mHisto = NULL;
      }
      mManager->DeleteFormulas();
      this->GetDetectorList(0);
      this->GetHistoList(0);
      LayoutTreeTabs();
    }break;
    case DELETE_ALL_INOUT: {
      //      std::cout << "[TigFrame::HandleMenu] delete formulas" << std::endl;
      if (mHisto){
	mHisto = NULL;
      }
      mManager->DeleteHistos();
      mCanvas->Update();
      mManager->DeleteFormulas();
      this->GetDetectorList(0);
      this->GetHistoList(0);
      LayoutTreeTabs();
    }break;
    case DELETE_TREES: {
      //      std::cout << "[TigFrame::HandleMenu] delete trees" << std::endl;
      if (mHisto){
	mHisto = NULL;
      }
      mManager->DeleteHistos(); 
     mCanvas->Update();
      mManager->DeleteFormulas();
      mManager->DeleteTrees();
      mManager->mHasConfig = false;
      mManager->mHasRootConfig = false;
      mLBHistos->RemoveAll();
      mLBDetectors->RemoveAll();
      mLBHistos->Layout();
      mLBDetectors->Layout();
      LayoutTreeTabs();
     }break;
    case HISTO_UPDATE_FREQ :{
      std::stringstream ss;
      ss << mManager->HistoUpdateInterval();
      char *retstr = new char;
      new TGInputDialog(gClient->GetRoot(),this,"Update every  ... events" ,ss.str().c_str(), retstr );
      //      std::cout << "new interval " << retstr << " - int " << atoi(retstr) << std::endl;
          mManager->SetHistoUpdateInterval( atoi(retstr));      
    }break;
    case GUI_UPDATE_FREQ :{
      std::stringstream ss;
      ss << mManager->GuiInterval();
      char *retstr = new char;
      new TGInputDialog(gClient->GetRoot(),this,"Update every  ... events" ,ss.str().c_str(), retstr );
      //      std::cout << "new interval " << retstr << " - int " << atoi(retstr) << std::endl;
          mManager->SetGuiInterval( atoi(retstr));      
    }break;
    }
}


//---- LayoutTreeTabs
void
TigFrame::LayoutTreeTabs()
{
  int curTab = mTab->GetCurrent();
  for (int i=mTab->GetNumberOfTabs(); i>0; i--)mTab->RemoveTab(i);
  mTab->SetTab(0);
  vector<string> trees = mManager->TreeNames();
  for (int i = 0; i<trees.size(); i++) {
    TGCompositeFrame *tab = mTab->AddTab(trees.at(i).c_str());
    TigDetFrame *detFrame = new TigDetFrame(tab);
    detFrame->SetParent(this);
    vector<TigDataObject*> dets = mManager->GetDataObjects(trees.at(i));
    for (int j = 0; j<dets.size(); j++) {
      detFrame->AddEntry(dets.at(j),j);
    }
    tab->AddFrame(detFrame);
    tab->MapSubwindows();
    tab->MapWindow();
    mTreeTabs.push_back(tab);
 }

  if (curTab < mTab->GetNumberOfTabs() ) mTab->SetTab(curTab);
  else mTab->SetTab(0);
  this->Resize(this->GetDefaultSize()); 
  this->MapSubwindows(); 
  this->MapWindow();
  this->Layout();
}

//---- MakeOutFile
void
TigFrame::MakeOutFile(char *entry)
{
  mManager->SetOutPrefix( (char*)mOutPrefixEntry->GetText() );
  mManager->SetOutPostfix(  (char*)mOutPostfixEntry->GetText() );
  if (mRunNumbers.size() >0 ){
    string text = mOutPrefixEntry->GetText() + mOutFileNumbers +  mOutPostfixEntry->GetText() + ".root" ; 
    mOutEntry->SetText(text.c_str());
    mManager->SetOutFile(  (char*)text.c_str() );
  }
}



//---- Run
void
TigFrame::Run()
{
  if (mManager->IsRunning() ) {
    mManager->StopRunning();
  }
  else {
    mRunButton->SetText("&Stop");
     mTBStep->SetEnabled(kFALSE);
     if (mManager->mHasConfig) mManager->Run( );
     else if (mManager->mHasRootConfig) mManager->RunRoot( mComboTrees->GetSelected() );
  }
    UpdateHisto();
     mTBStep->SetEnabled(kTRUE);
    mRunButton->SetText("&Run");
}

//---- SetOutFile
void
TigFrame::SetOutFile(char *entry)
{
    mManager->SetOutFile( entry );
}

//---- Step
void 
TigFrame::Step()
{
  mTBStep->SetDown(kTRUE);
  mManager->StepTrees(true);
  if (mManager->mHasConfig) mManager->StepMidasFile();
  else if (mManager->mHasRootConfig) mManager->RunRoot( mComboTrees->GetSelected());
  mManager->StepTrees(false);
  mTBStep->SetDown(kFALSE);

}

//---- SwitchOutfileSetting
void
TigFrame::SwitchHistoVsTree(Int_t index)
{
  if (index == 1 ){
    mManager->GenerateTrees(false);
    mBGOutfile->SetState(kFALSE);
     mTBStep->SetEnabled(kTRUE);
    mOutPrefixEntry->SetText("hist");
  }
  else if (index == 2) {
    mManager->GenerateTrees(true);
    mBGOutfile->SetState(kTRUE);
     mTBStep->SetEnabled(kFALSE);
     if (mManager->mHasRootConfig) mOutPrefixEntry->SetText("sort");
     else mOutPrefixEntry->SetText("asm");
  }
}

//---- SwitchOutfileSetting
void
TigFrame::SwitchOutfileSetting(Int_t index)
{
  mManager->SwitchOutfileSetting(index);
  if (index ==1) mManager->SetOutFile( (char*)mOutEntry->GetText() );
}


//---- UpdateHisto
void
TigFrame::UpdateHisto()
{
  // std::cout << "[TigFrame::UpdateHisto]" << std::endl;
  if ( (mHisto) && (mCanvas) &&  (mComboTrees->GetSelected()>-1) && mLBHistos->GetSelected()>-1 ){
    mHisto->Draw();
    mCanvas->Update();
  }
}
