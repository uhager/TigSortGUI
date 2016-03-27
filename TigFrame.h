// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGFRAME_H
#define TIGFRAME_H

//class TigFrame;

#include <set>

#include <TROOT.h>
#include <TSystem.h>
#include <TGMenu.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGLayout.h>
#include <TGTextEntry.h>
#include <TSocket.h>
#include <TMessage.h>
#include <TApplication.h>
#include <TObject.h>
#include <TGFileDialog.h>
#include <TGListBox.h>
#include <TGTab.h>
#include <TGComboBox.h>
#include <TCanvas.h>
#include <TH1.h>

#include "TigDetFrame.h"
#include "TigManager.h"



class TigDetFrame;

enum ButtonID {
  WRITE_TO_TREE
};



//---- TigFrame
class TigFrame : public TGMainFrame
{
  //RQ_OBJECT("TigFrame")
public:			//---- public ---------------
  TigFrame( const TGWindow *pWindow, UInt_t pWidth, UInt_t pHeight, UInt_t pOptions = kVerticalFrame);
 virtual ~TigFrame(){CloseWindow();};

 virtual void	CloseWindow(void);
//slots
 void GetDetectorList(char *);
 void GetDetectorList(int);
 void GetHisto(char *);
 void GetHistoList(char *);
 void GetHistoList(int);
 Int_t GetUsableHeight();
 void HandleDetFrame(int,int);
 void HandleMenu(Int_t);
 void LayoutTreeTabs();
 // virtual Bool_t HandleTimer(TTimer *pTimer);
 void MakeOutFile(char *);
 void Run();
 void SetOutFile(char *);
 void Step();
 // void SetTimer(int t){mTimerTime = t;}
 void SwitchHistoVsTree(Int_t);
 void SwitchOutfileSetting(Int_t);
 void UpdateHisto();

protected:		//---- protected -----------

 void AssembleOutFileNumbers(TList* files);
TigManager*	mManager;
 TH1 *mHisto;
 TGMenuBar *mMenuBar;
  TCanvas *mCanvas;
  TGPopupMenu *mMenuConfig, *mDetConfMenu, *mMenuFile, *mMenuCalib, *mMenuReset, *mMenuPrefs;
 TGListBox *mLBDetectors, *mLBHistos;
 TGComboBox *mComboTrees;
 TGTextEntry *mOutEntry, *mOutPostfixEntry, *mOutPrefixEntry;
 TGTextButton *mRunButton, *mTBSaveHistos, *mTBStep;
 std::set<int> mRunNumbers;
  TGFileInfo mFileInfo;
  TGButtonGroup *mBGOutfile, *mBGHisto;
  TGGroupFrame *mGFOutEntry;
  TGRadioButton *mRBOutfile[2], *mRBHisto[2];
  TGVerticalFrame *mFrameOutEntry, *mFrameTreeMain ;
  TGLayoutHints *mLHOutEntry, *mLHDetList, *mLHButtons;
  TGHorizontalFrame *mFrameMain, *mFrameButtons, *mFrameTree, *mFrameLists, *mFrameMods;
  std::string mOutFileNumbers;
  TGTab *mTab;
  //  vector<TigDetFrame*> mDetFrames; 
  std::vector<TGCompositeFrame*> mTreeTabs;

 private:
  TigFrame(const TigFrame&);
  TigFrame& operator=( const TigFrame& );

  ClassDef(TigFrame,0);

};






#endif
