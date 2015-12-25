// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TigCollection.h>


//using namespace std;

//ClassImp(TigCollection)


TigCollection::TigCollection()
{
  //  cout << "[TigCollection::TigCollection]" << endl;
  NCollection = 0; 
  // Collection = new TClonesArray("TigData",gMaxSize);
  Collection.SetClass("TigData",gMaxSize);
  Initialize();
}


TigCollection::TigCollection(int ncoll)
{
  //  cout << "[TigCollection::TigCollection] " << ncoll << endl;
  NCollection = ncoll;
  Initialize(); 
}

TigCollection::~TigCollection()
{
  //  Collection->Delete();
  //  delete Collection;
}

TigData*
TigCollection::At(int i)
{
  if (i > NCollection-1){
    cout << "Requested data entry outside range" << endl;
    return NULL;
  }
  return (TigData*)Collection[i];
}

int
TigCollection::Add(TigData* data)
{
  //  cout << "[TigCollection::Add]" << endl;
  if (NCollection == gMaxSize-1){
    cout << "[TigCollection::Add] Oops! Current maximum number of data objects is " << gMaxSize << " change 'gMaxSize' in TigCollection.h for larger arrays " << endl; 
  }
  NCollection++;
  Initialize();
  //  new (Collection[NCollection]) data;
}

int
TigCollection::AddInput()
{
  //  cout << "[TigCollection::Add]" << endl;
  NCollection++;
  this->Clear();
  Initialize();
}

void
TigCollection::Clear()
{
  //    delete Collection;
    Initialize();
}

void 
TigCollection::ConnectInput(int i, TigData *data)
{
  if (i > NCollection-1){
    cout << "Requested data entry outside range" << endl;
  }
  new (Collection[i]) (TigData*)data;
}

bool 
TigCollection::Initialize()
{
  //  Collection = new TigData*[NCollection];
  for (int i = 0; i<NCollection ; i++) new (Collection[i]) TigData;
  return true;
}

void
TigCollection::SetSize(int ncoll)
{
  //  cout << "[TigCollection::SetSize] NCollection " << ncoll << endl; 
  NCollection = ncoll;
  Initialize();
}


