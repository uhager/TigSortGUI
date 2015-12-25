// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGDETFRAME_H
#define TIGDETFRAME_H

#include <string>
#include <TGButton.h>
#include <TGLayout.h>
#include <TGLabel.h>
#include <TGListBox.h>
#include <TigDataObject.h>

using namespace std;

class TigFrame;

class TigDetFrame : public TGCompositeFrame
{

 public:
  TigDetFrame();
  TigDetFrame( const TGWindow *pWindow);
  ~TigDetFrame(){}
  void AddEntry(TigDataObject *obj, int id);
   void HandleButtons();
   void SetParent(TigFrame *p);

   TGCompositeFrame *mContainer;
   TGCanvas *mViewer;

 protected:
   TigFrame *mParent;
   UInt_t mNumCols;
   TGLayoutHints *mLayout;
   TGCompositeFrame *mCompFrame; 

  ClassDef(TigDetFrame,0);

};




#endif  /*TIGDETFRAME_H*/
