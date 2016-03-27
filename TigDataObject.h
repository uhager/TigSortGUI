// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGDATAOBJECT_H
#define TIGDATAOBJECT_H

#include <vector>
#include <string>

#include <TClonesArray.h>

#include "TigObject.h"

class TigTree;


const int gMaxEntries = 1024;

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
  ,CALIBRATION
  ,LOOKUPTABLE
};

class TigDataObject : public TigObject
{
 public:
  TigDataObject();
  TigDataObject(int num);
  ~TigDataObject();

  void AddCut(std::string name);
  void AddInput(std::string name, int id);
virtual  void Clear();
//  std::vector< pair<int,double*>* > Data;
   int DataLength();    //  size of double in pair of Data
  int DataSize();      // size of Data std::vector
  virtual bool Evaluate();
  virtual void IncreaseDataLength(int){}
  virtual bool IniInputs();
  virtual bool Initialize();
  //  std::vector<std::string> InputNames(){return mInputNames};
  virtual bool ParseInput(std::string line);
  void Update(int hits, int* channel, int *value);
  void Update(int hits, double* channel, double *value);
  void Update(int hits, double *value);
  void Update(int hits, int *value);
  virtual void Reset();
  void SetCutNames(std::vector<std::string> pN){mCutNames = pN;}
  void SetDataLength( int hits);
  void SetInputNames(std::vector<std::string> pN){mInputNames = pN;}
  void SetNumData(int i);
  void SetParent(TigTree *tree){mParent = tree;}
  int WhatToDo(){return mWhatToDo;}

  std::vector<std::pair<int,double*>* > CutData;
  std::vector<std::pair<int,double*>* >  Data;   
  std::vector<std::pair<int,double*>* >  InputData;
  bool *IsUpdated;
  std::vector<bool*> InputUpdated;
  std::vector<std::pair<std::string,int> > NeededCuts(){return mNeededCuts;}
  std::vector<std::pair<std::string,int> > NeededParameters(){return mNeeded;}
  bool SizeDetermined;
  bool mWriteToTree;

 protected:
  std::vector<std::string> mCutNames;
  int mDataLength;
  std::vector<std::string> mInputNames;
  std::vector<std::pair<std::string,int> > mNeeded; //needed parameters as pairs of detector name and channel(0) or value(1)
  std::vector<std::pair<std::string,int> > mNeededCuts; //needed parameters as pairs of detector name and channel(0) or value(1)
 TigTree *mParent;
 int mWhatToDo;

 private:
 // prevent copying
 TigDataObject(const TigDataObject&);
 TigDataObject& operator=(const TigDataObject&);

};

#endif /* TIGDATAOBJECT_H */
