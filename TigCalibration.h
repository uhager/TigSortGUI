// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGCALIBRATION_H
#define TIGCALIBRATION_H

#include <vector>
#include <string>
#include <map>

#include "TigDataObject.h"


class TigCalibration:  public TigDataObject
{
 public:
  TigCalibration();

  virtual bool Evaluate();
  virtual void IncreaseDataLength(int);
  virtual bool Initialize();
  virtual bool ParseInput(std::string line);

 protected:
  std::map< int,std::pair < double,double > > mCalibration;

};

#endif  /* TIGCALIBRATION_H */
