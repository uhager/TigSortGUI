// part of TigSortGUI
// author: Ulrike Hager

#include <vector>
#include <string>

#include "TigDataObject.h"


class TigLookupTable:  public TigDataObject
{
 public:
  TigLookupTable();
 
  void AddTableLine(std::vector<double> pTable);
  virtual bool Evaluate();
  virtual void IncreaseDataLength(int);
  virtual bool Initialize();
  virtual bool ParseInput(std::string line);
  void SetXChannels(std::vector<int> pChannels);
  void SetYChannels(std::vector<int> pChannels);

 protected:
  std::vector<std::vector<double> > mLookupTable;
  std::vector<std::vector<int> > mChannels;
};
