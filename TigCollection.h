// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGCOLLECTION_H
#define TIGCOLLECTION_H

#include <vector>
#include <TClonesArray.h>
#include <TigData.h>

using namespace std;

const int gMaxSize = 128;

class TigCollection // : public TObject
{
public:
TigCollection();
TigCollection(int ncoll);
~TigCollection();

int Add(TigData* data);
int AddInput();
TigData* At(int index);
void Clear();
void ConnectInput(int i, TigData *data);
bool Initialize();
Int_t Size(){return NCollection;}
void SetSize(int ncoll);

private:
Int_t NCollection;
 TClonesArray Collection;  //[NCollection]

//ClassDef(TigCollection,0)
};

#endif /* TIGCOLLECTION_H */
