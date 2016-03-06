// part of TigSortGUI
// author: Ulrike Hager

#include <vector>
#include <string>
#include <TigDataObject.h>

using namespace std;

class TigLookupTable:  public TigDataObject
{
 public:
  TigLookupTable();
 
  void AddTableLine(vector<double> pTable);
  virtual bool Evaluate();
  virtual void IncreaseDataLength(int);
  virtual bool Initialize();
  virtual bool ParseInput(string line);
  void SetXChannels(vector<int> pChannels);
  void SetYChannels(vector<int> pChannels);

 protected:
  vector<vector<double> > mLookupTable;
  vector<vector<int> > mChannels;
};
