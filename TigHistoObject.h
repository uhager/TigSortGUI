// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGHISTOOBJECT_H
#define TIGHISTOOBJECT_H

#include <TH1.h>
#include <TH2.h>
//#include <TigObject.h>
#include <TigDataObject.h>

using namespace std;

class TigHistoObject : public TigDataObject
{
 public:
  TigHistoObject();
virtual   ~TigHistoObject();
  TH1 *Histogram(){return mHisto;}
 void Reset();
 void Write();

 protected:
  TH1* mHisto;
 
 
};



#endif /* TIGHISTOOBJECT_H */
