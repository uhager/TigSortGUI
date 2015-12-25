// part of TigSortGUI
// author: Ulrike Hager

#include <vector>
#include <string>
#include <TFormula.h>
//#include <TigObject.h>
//#include <TigDataObject.h>
#include <TigDataObject.h>

using namespace std;

class TigFormula :  public TigDataObject
{
 public:
  TigFormula();
  ~TigFormula();

 virtual bool Evaluate();
  virtual void IncreaseDataLength(int);
 virtual bool Initialize();
  virtual bool ParseInput(string line);

   void SetFormula(string formula);
 protected:
  TFormula *mFormula;
};
