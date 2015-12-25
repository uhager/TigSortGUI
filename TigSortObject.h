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
#include <TigDataObject.h>

using namespace std;

class TigSortObject : public TigDataObject
{
 public:
  TigSortObject();
  //  ~TigSortObject();

  //virtual  void Clear();
 virtual bool Evaluate();
 virtual void IncreaseDataLength(int);
  virtual bool Initialize();
  virtual bool ParseInput(string line);
 void SetParameters(vector<string> spec){mParameters = spec;}

 protected:
 vector<string> mParameters;
 set<int> mChannels;
};


#endif /*TIGSORTOBJECT_H */
