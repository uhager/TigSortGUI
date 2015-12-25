// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGCALIBRATION_H
#define TIGCALIBRATION_H

#include <vector>
#include <string>
#include <map>
#include <TigDataObject.h>

using namespace std;

class TigCalibration:  public TigDataObject
{
 public:
  TigCalibration();
  ~TigCalibration();

 virtual bool Evaluate();
 virtual void IncreaseDataLength(int);
 virtual bool Initialize();
  virtual bool ParseInput(string line);

 protected:
  map< int,pair < double,double > > mCalibration;

};

#endif  /* TIGCALIBRATION_H */
