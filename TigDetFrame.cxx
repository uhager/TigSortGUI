// part of TigSortGUI
// author: Ulrike Hager


#include <iostream>
#include <TigFrame.h>
#include <TigDetFrame.h>


using namespace std;



TigDetFrame::TigDetFrame()
{
  //  this->Init();
  mContainer = NULL;
  mLayout = NULL;
  mViewer = NULL;
  mCompFrame = NULL;
}

TigDetFrame::TigDetFrame(const TGWindow *pWindow)
  :TGCompositeFrame(pWindow)
{
  mNumCols = 2;
  //  this->Init();
  mContainer = NULL;
  mLayout = NULL;
  mViewer = NULL;
  mCompFrame = NULL;
}


TigDetFrame::~TigDetFrame()
{
  if (mCompFrame)
    delete mCompFrame;
  if (mContainer)
    delete mContainer;
  if (mViewer)
    delete mViewer;
  if (mLayout)
    delete mLayout;
}


void
TigDetFrame::AddEntry(TigDataObject* obj, int id)
{
  //  cout << "[TigDetFrame::AddEntry] name " << obj->Name() << " id " << id << endl; 
  TGLabel *lab = new TGLabel(mCompFrame,obj->Name().c_str());
   mCompFrame->AddFrame(lab);
   //   int height = lab->GetDefaultHeight();
    for (int i =1; i<mNumCols; i++) 
     {
       TGCheckButton *button = new TGCheckButton(mCompFrame, "", 100*id+i-1);
       mCompFrame->AddFrame(button);
       if ( (i==1) && obj->mWriteToTree ) button->SetState(kButtonDown);
	button->Connect("Clicked()","TigDetFrame",this,"HandleButtons()" );
     }

    this->MapSubwindows();
   this->MapWindow();  

}

void
TigDetFrame::HandleButtons()
{

  TGButton *btn = (TGButton *) gTQSender;
  int buttonID = btn->WidgetId();

  //  cout << "[TigDetFrame::HandleButtons] buttonID " << buttonID << endl;
  int button = buttonID % 100;
  int det = buttonID /100;

  mParent->HandleDetFrame(det,button);
}

void
TigDetFrame::SetParent(TigFrame *p)
{
  mParent = p;
   TGString columnName[] = {"Det","Write"};
   TGLayoutHints *layout1 = new TGLayoutHints(kLHintsTop, 20, 20, 10, 0);
   mLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX| kLHintsCenterX, 1, 5, 1, 5);
   mViewer = new TGCanvas(this,mParent->GetUsableHeight()-10, mParent->GetWidth()-5);
   mContainer = new TGCompositeFrame(mViewer->GetViewPort());
   mViewer->SetContainer(mContainer);
   this->AddFrame(mViewer, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   mCompFrame = new TGCompositeFrame(mContainer,mParent->GetUsableHeight()-15, mParent->GetWidth()-15);
   mCompFrame->SetLayoutManager(new TGMatrixLayout(mCompFrame, 0, mNumCols,1,1));
   mCompFrame->SetMinWidth( mParent->GetWidth());
   mViewer->SetMinWidth( mParent->GetWidth()-10);
   for (int i =0; i<mNumCols; i++) 
     {
       TGLabel *label = new TGLabel(mCompFrame,columnName[i]);
       mCompFrame->AddFrame(label);
     }

   mContainer->AddFrame(mCompFrame, layout1);
   
    this->MapSubwindows();
   this->MapWindow();  

}
