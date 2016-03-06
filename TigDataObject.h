// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGDATAOBJECT_H
#define TIGDATAOBJECT_H

#include <vector>
#include <string>
#include <TClonesArray.h>
#include <TigObject.h>

class TigTree;

using namespace std;

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

  void AddCut(string name);
  void AddInput(string name, int id);
virtual  void Clear();
//  vector< pair<int,double*>* > Data;
   int DataLength();    //  size of double in pair of Data
  int DataSize();      // size of Data vector
  virtual bool Evaluate();
  virtual void IncreaseDataLength(int){}
  virtual bool IniInputs();
  virtual bool Initialize();
  //  vector<string> InputNames(){return mInputNames};
  virtual bool ParseInput(string line);
  void Update(int hits, int* channel, int *value);
  void Update(int hits, double* channel, double *value);
  void Update(int hits, double *value);
  void Update(int hits, int *value);
  virtual void Reset();
  void SetCutNames(vector<string> pN){mCutNames = pN;}
  void SetDataLength( int hits);
  void SetInputNames(vector<string> pN){mInputNames = pN;}
  void SetNumData(int i);
  void SetParent(TigTree *tree){mParent = tree;}
  int WhatToDo(){return mWhatToDo;}

  vector<pair<int,double*>* > CutData;
  vector<pair<int,double*>* >  Data;   
  vector<pair<int,double*>* >  InputData;
  bool *IsUpdated;
  vector<bool*> InputUpdated;
  vector<pair<string,int> > NeededCuts(){return mNeededCuts;}
  vector<pair<string,int> > NeededParameters(){return mNeeded;}
  bool SizeDetermined;
  bool mWriteToTree;

 protected:
  vector<string> mCutNames;
  int mDataLength;
  vector<string> mInputNames;
  vector<pair<string,int> > mNeeded; //needed parameters as pairs of detector name and channel(0) or value(1)
  vector<pair<string,int> > mNeededCuts; //needed parameters as pairs of detector name and channel(0) or value(1)
 TigTree *mParent;
 int mWhatToDo;

 private:
 // prevent copying
 TigDataObject(const TigDataObject&);
 TigDataObject& operator=(const TigDataObject&);

};

#endif /* TIGDATAOBJECT_H */
