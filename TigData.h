// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGDATA_H
#define TIGDATA_H

#include <vector>
#include <TObject.h>

using namespace std;

class TigData : public TObject
{
 public:
  TigData();
  TigData(int nmax);
  ~TigData();

  void Clear();
  void SetNMax(int nmax);

  Int_t NData;
  Int_t NMax;
  //  Int_t DataSize;
  Double32_t *Data;  //[NData]

   ClassDef(TigData,0);

};

#endif /* TIGDATA_H */
