// part of TigSortGUI
// author: Ulrike Hager

/*
Object for all kind of manipulation that can't be done by TFormula. mWhatToDo determines evaluation
 ( (0) TigFormula )
(1) max value (keyword _maxValue) finds max value for detector, gives channel and value
(2) select channels (keyword _selectCh),   gives selected channels and values
 */

#ifndef TIGSORTOBJECT_H
#define TIGSORTOBJECT_H

#include <set>
#include "TigDataObject.h"


class TigSortObject : public TigDataObject
{
 public:
  TigSortObject();

  //virtual  void Clear();
 virtual bool Evaluate();
 virtual void IncreaseDataLength(int);
  virtual bool Initialize();
  virtual bool ParseInput(std::string line);
 void SetParameters(std::vector<std::string> spec){mParameters = spec;}

 protected:
 std::vector<std::string> mParameters;
 std::set<int> mChannels;
};


#endif /*TIGSORTOBJECT_H */
