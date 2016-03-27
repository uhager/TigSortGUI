// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGHISTOOBJECT_H
#define TIGHISTOOBJECT_H

#include <TH1.h>
#include <TH2.h>

#include "TigDataObject.h"


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

 private:
  TigHistoObject( const TigHistoObject& );
  TigHistoObject& operator=( const TigHistoObject& );
 
};



#endif /* TIGHISTOOBJECT_H */
