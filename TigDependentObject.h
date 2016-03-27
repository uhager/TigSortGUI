// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGDEPENDENTOBJECT_H
#define TIGDEPENDENTOBJECT_H

#include <string>
#include <vector>

#include "TigDataObject.h"


enum WhatToDos {
  TFORMULA
  ,MAXVALUE
  ,SELECTCHANNEL
  ,COMBINE
  ,TCUTG
  ,RANGECUT
  ,ONE_D
  ,TWO_D 
  ,SCALER_HISTO
};

class TigTree;

class TigDependentObject : public TigDataObject
{
 public:
  TigDependentObject();
  TigDependentObject(int num);
  virtual ~TigDependentObject();

  void AddCut(std::string name);
  void AddInput(std::string name, int id);
  virtual bool Evaluate();
  virtual bool IniInputs();
  virtual bool Initialize();
 std::vector< std::vector<double>* > InputData;
  std::vector<bool*> InputUpdated;
 std::vector< std::vector<double>* > CutData;
  std::vector<pair<std::string,int> > NeededCuts(){return mNeededCuts;}
  std::vector<pair<std::string,int> > NeededParameters(){return mNeeded;}
  virtual bool ParseInput(std::string line);

  //  void SetInput
  void SetCutNames(std::vector<std::string> pN){mCutNames = pN;}
  void SetInputNames(std::vector<std::string> pN){mInputNames = pN;}
 bool SizeDetermined;
  int WhatToDo(){return mWhatToDo;}

 protected:
  std::vector<std::string> mCutNames;
  std::vector<std::string> mInputNames;
  std::vector<pair<std::string,int> > mNeeded; //needed parameters as pairs of detector name and channel(0) or value(1)
  std::vector<pair<std::string,int> > mNeededCuts; //needed parameters as pairs of detector name and channel(0) or value(1)
  int mWhatToDo;

 private:
  TigDependentObject(const TigDependentObject&);
  TigDependentObject& operator=(const TigDependentObject&);
};


#endif /* TIGDEPENDENTOBJECT_H */
