// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGSCALERHISTO_H
#define TIGSCALERHISTO_H

#include <TH1.h>
#include <TH2.h>

#include "TigHistoObject.h"



class TigScalerHisto :  public TigHistoObject //, public TigDataObject
{
 public:
  TigScalerHisto();
  virtual bool Evaluate();
  virtual bool Initialize();
  virtual bool ParseInput(std::string line);
  void SetChannel(int ch){mChannel= ch;}
  void SetChannelName(std::string ch){mChannelName = ch;}
  void SetXMax(int);

 protected:
  std::string mChannelName; 
  int mChannel;
  //  Bool_t is2D;
  int xMax;


};



#endif /* TIGSCALERHISTO_H */
