// part of TigSortGUI
// author: Ulrike Hager

#include <TigHistoObject.h>

TigHistoObject::TigHistoObject()
  :mHisto(NULL)
{
  ObjType = "Histo";
}

TigHistoObject::~TigHistoObject()
{
  if (mHisto) delete mHisto;
  //  cout << "[TigHistoObject::~TigHistoObject] " << mName << endl; 
}

//---- Reset
void
TigHistoObject::Reset()
{
  if (mHisto) mHisto->Reset();
}


void
TigHistoObject::Write()
{
  //  cout << "TigHisto " << mName << " wrinting" << endl;
  mHisto->Write();
}
