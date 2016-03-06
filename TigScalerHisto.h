// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGSCALERHISTO_H
#define TIGSCALERHISTO_H

#include <TH1.h>
#include <TH2.h>
//#include <TigObject.h>
//#include <TigTree.h>
//#include <TigDataObject.h>
#include <TigHistoObject.h>

using namespace std;

class TigScalerHisto :  public TigHistoObject //, public TigDataObject
{
 public:
  TigScalerHisto();
  virtual bool Evaluate();
  virtual bool Initialize();
  virtual bool ParseInput(string line);
 void SetChannel(int ch){mChannel= ch;}
 void SetChannelName(string ch){mChannelName = ch;}
  void SetXMax(int);

 protected:
   string mChannelName; 
  int mChannel;
  //  Bool_t is2D;
  int xMax;


};



#endif /* TIGSCALERHISTO_H */
