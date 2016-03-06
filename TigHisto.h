// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGHISTO_H
#define TIGHISTO_H

//#include <TH1.h>
#include <TH2.h>
//#include <TigObject.h>
//#include <TigDataObject.h>
#include <TigHistoObject.h>

using namespace std;

class TigHisto :  public TigHistoObject //  public TigDataObject,
{
 public:
  TigHisto();

  virtual bool Evaluate();
  virtual bool Initialize();
  virtual bool ParseInput(string line);
  void SetXBinning(int,double,double);
  void SetYBinning(int,double,double);

 protected:
 
  //  Bool_t is2D;
  int xBins, yBins;
  double xMin, xMax, yMin, yMax;

};



#endif /* TIGHISTO_H */
