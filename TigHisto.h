// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGHISTO_H
#define TIGHISTO_H

#include <TH2.h>

#include "TigHistoObject.h"


class TigHisto :  public TigHistoObject //  public TigDataObject,
{
 public:
  TigHisto();

  virtual bool Evaluate();
  virtual bool Initialize();
  virtual bool ParseInput(std::string line);
  void SetXBinning(int,double,double);
  void SetYBinning(int,double,double);

 protected:
  int xBins, yBins;
  double xMin, xMax, yMin, yMax;

};



#endif /* TIGHISTO_H */
