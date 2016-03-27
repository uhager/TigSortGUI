// part of TigSortGUI
// author: Ulrike Hager

#include <vector>
#include <string>

#include <TFormula.h>

#include "TigDataObject.h"


class TigFormula :  public TigDataObject
{
 public:
  TigFormula();
  ~TigFormula();

  virtual bool Evaluate();
  virtual void IncreaseDataLength(int);
  virtual bool Initialize();
  virtual bool ParseInput(std::string line);

  void SetFormula(std::string formula);
   
 protected:
  TFormula *mFormula;

 private:
  TigFormula(const TigFormula&);
  TigFormula& operator=( const TigFormula& );
};
