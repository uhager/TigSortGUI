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
  //  std::cout << "[TigHistoObject::~TigHistoObject] " << mName << std::endl; 
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
  //  std::cout << "TigHisto " << mName << " wrinting" << std::endl;
  mHisto->Write();
}
