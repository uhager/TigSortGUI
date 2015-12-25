// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGDEPENDENTOBJECT_H
#define TIGDEPENDENTOBJECT_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <TigDataObject.h>
//#include <TigTree.h>

using namespace std;

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

  void AddCut(string name);
  void AddInput(string name, int id);
  virtual bool Evaluate();
  virtual bool IniInputs();
  virtual bool Initialize();
 vector< vector<double>* > InputData;
  vector<bool*> InputUpdated;
 vector< vector<double>* > CutData;
  vector<pair<string,int> > NeededCuts(){return mNeededCuts;}
  vector<pair<string,int> > NeededParameters(){return mNeeded;}
  virtual bool ParseInput(string line);

  //  void SetInput
  void SetCutNames(vector<string> pN){mCutNames = pN;}
  void SetInputNames(vector<string> pN){mInputNames = pN;}
 bool SizeDetermined;
  int WhatToDo(){return mWhatToDo;}

 protected:
  vector<string> mCutNames;
  vector<string> mInputNames;
  vector<pair<string,int> > mNeeded; //needed parameters as pairs of detector name and channel(0) or value(1)
  vector<pair<string,int> > mNeededCuts; //needed parameters as pairs of detector name and channel(0) or value(1)
 int mWhatToDo;
};


#endif /* TIGDEPENDENTOBJECT_H */
