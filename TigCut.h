// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGCUT_H
#define TIGCUT_H

#include <TCutG.h>
#include <TigDataObject.h>

class TigCut : public TigDataObject
{
 public:
  TigCut();
  ~TigCut();

  virtual bool Evaluate();
  virtual bool Initialize();
  void SetCut(TCutG* pCut){mCut = pCut;}
  void SetCutName(string pFile,string pObject);
  virtual bool ParseInput(string line);
  void SetRange(double min, double max);

 protected:

  TCutG* mCut;
  string mFile;
  string mCutName;
  int mMinRange, mMaxRange;

 private:
  //! no copying, doesn't make sense to have the same cut
  TigCut(const TigCut&);
  TigCut& operator=(const TigCut&);
};


#endif  /* TIGCUT_H */
