// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <TigFormula.h>

TigFormula::TigFormula()
  :  TigDataObject(1)
  ,mFormula(NULL)
{
  mName = "formula";
  mWhatToDo = TFORMULA;
  ObjType = "Formula";
}

TigFormula::~TigFormula()
{
  //  cout << "[TigFormula::~TigFormula] " << mName << endl;
   delete mFormula;
  // cout << "[TigFormula::~TigFormula] done" << endl;
}

bool
TigFormula::Evaluate()
{
  // cout << "[TigFormula::Evaluate]" << endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;
  int needed = mFormula->GetNpar(); 

  if (  needed != InputData.size() ) {
    cerr << "[TigFormula::Evaluate] parameter mismatch: needed " << needed << " received: " << InputData.size() << endl;
    return false ;
  }
  
 for (int i = 0; i<needed; i++ ) {
   if ( *(InputUpdated.at(i)) == false ) return false;
  }

  int combinations = 1;
  for (int i = 0; i<InputData.size(); i++)  combinations *= (InputData.at(i))->first;
  //  cout << "[TigFormula::Evaluate] # combinations " << combinations << endl;;

  if (combinations){
  double *pars;
  pars = new double[combinations];
  //  cout << "[TigFormula::Evaluate] " << mName << " combinations " << combinations << endl;

   ::memset(pars, 0,  combinations * sizeof(double));

  // cout << " before:  " << endl; ;
  // for (int j = 0; j<needed; j++)  cout << " &InputData.at(j)->first " << &(InputData.at(j)->first)  << " - InputData.at(j)->second " << (InputData.at(j)->second) << endl;
  // cout << endl;

  double sorted[combinations][needed];

  for (int i = 0; i<needed; i++ ) {
     int remainComb = 1;
     for (int j = i+1; j<needed; j++)  remainComb *=  (InputData.at(j))->first;
     for (int j = 0; j < (InputData.at(i))->first; j++) {
      int oncount = 0;
      int iniOff = remainComb * j;
      int offcount = remainComb * (  (InputData.at(i))->first  - 1 );
      for (int k = iniOff; k<combinations; k++){
	bool fill = false;
	if (oncount < remainComb) fill = true;
	if (fill){
	  sorted[k][i] =  (InputData.at(i))->second[j];
	oncount++;
	if (oncount == remainComb) offcount = remainComb * ( (InputData.at(i))->first -1 ) ;
	}
	else {
	  offcount--;
	  if (offcount == 0) oncount = 0;
	}
      }
    }
  }

    //  return;

    for (int i=0; i<combinations; i++)  pars[i] = mFormula->EvalPar(0,sorted[i]);
    if (combinations>mDataLength) this->SetDataLength(combinations);
    this->Update(combinations, pars );

       // cout << "after:  " << endl; ;
  // for (int j = 0; j<needed; j++)  cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << endl;
  // cout << endl;

  }
 
  return true;
}

void 
TigFormula::IncreaseDataLength(int d)
{
  // cout << "[TigFormula::IncreaseDataLength] " << mName << " - " << d  << endl;
  mDataLength *= d;
  this->SetDataLength(mDataLength);
}

bool
TigFormula::Initialize()
{
  // cout << "[TigFormula::Initialize] " << mName << endl;
  if (!mFormula) return false;
  bool check =  this->TigDataObject::Initialize();
  if (!check) return false;
  if (mFormula->GetNpar() != mNeeded.size()) {
    cout << "[TigFormula::Initialize] parameter mismatch: needed " << mFormula->GetNpar() << " found: " <<mNeeded.size() << endl;
    return false;
  }
  this->SetDataLength(1);
  return true;
}

bool
TigFormula::ParseInput(string line)
{
  bool result = false;
  string token;
  istringstream stream(line.c_str());	
  stream >> token;
  if ( token.compare("equation") == 0)
    {
      size_t startP = line.find_first_of("\"");
      size_t endP = line.find_last_of("\"");
      string eq = line.substr(startP+1,endP-startP-1);
      //   cout << "[TigTree::ParseFormula] equation " << eq << endl;
      this->SetFormula(eq);
      result = true;
    }
  else result = this->TigDataObject::ParseInput(line);
  return result;
}

void
TigFormula::SetFormula(string form)
{
  //  mFormula = new TFormula(mName.c_str(), form.c_str());

  mFormula = new TFormula();
    mFormula->SetName(mName.c_str());
  int check = mFormula->Compile(form.c_str());
  if (check == 1) {
    cerr << "[TigFormula::SetFormula] " << mName << " error compiling TFormula" << endl;
    delete mFormula;
    mFormula = NULL;
  }
}

