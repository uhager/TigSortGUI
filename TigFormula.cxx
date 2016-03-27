// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigFormula.h"

using std::string;


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
  //  std::cout << "[TigFormula::~TigFormula] " << mName << std::endl;
  delete mFormula;
  // std::cout << "[TigFormula::~TigFormula] done" << std::endl;
}

bool
TigFormula::Evaluate()
{
  // std::cout << "[TigFormula::Evaluate]" << std::endl;
  bool check = this->TigDataObject::Evaluate();
  if (!check) return false;
  int needed = mFormula->GetNpar(); 

  if (  needed != InputData.size() ) {
    std::cerr << "[TigFormula::Evaluate] parameter mismatch: needed " << needed << " received: " << InputData.size() << std::endl;
    return false ;
  }
  
  for (int i = 0; i<needed; i++ ) {
    if ( *(InputUpdated.at(i)) == false ) return false;
  }

  int combinations = 1;
  for (int i = 0; i<InputData.size(); i++)  combinations *= (InputData.at(i))->first;
  //  std::cout << "[TigFormula::Evaluate] # combinations " << combinations << std::endl;;

  if (combinations){
    double *pars;
    pars = new double[combinations];
    //  std::cout << "[TigFormula::Evaluate] " << mName << " combinations " << combinations << std::endl;

    ::memset(pars, 0,  combinations * sizeof(double));

    // std::cout << " before:  " << std::endl; ;
    // for (int j = 0; j<needed; j++)  std::cout << " &InputData.at(j)->first " << &(InputData.at(j)->first)  << " - InputData.at(j)->second " << (InputData.at(j)->second) << std::endl;
    // std::cout << std::endl;

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

    // std::cout << "after:  " << std::endl; ;
    // for (int j = 0; j<needed; j++)  std::cout << " &InputData.at(j)->size() " << &(InputData.at(j)->size())  << " - InputData.at(j)->second " << (InputData.at(j)->second) << std::endl;
    // std::cout << std::endl;

  }
 
  return true;
}

void 
TigFormula::IncreaseDataLength(int d)
{
  // std::cout << "[TigFormula::IncreaseDataLength] " << mName << " - " << d  << std::endl;
  mDataLength *= d;
  this->SetDataLength(mDataLength);
}

bool
TigFormula::Initialize()
{
  // std::cout << "[TigFormula::Initialize] " << mName << std::endl;
  if (!mFormula) return false;
  bool check =  this->TigDataObject::Initialize();
  if (!check) return false;
  if (mFormula->GetNpar() != mNeeded.size()) {
    std::cout << "[TigFormula::Initialize] parameter mismatch: needed " << mFormula->GetNpar() << " found: " <<mNeeded.size() << std::endl;
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
  std::istringstream stream(line.c_str());	
  stream >> token;
  if ( token.compare("equation") == 0)
    {
      size_t startP = line.find_first_of("\"");
      size_t endP = line.find_last_of("\"");
      string eq = line.substr(startP+1,endP-startP-1);
      //   std::cout << "[TigTree::ParseFormula] equation " << eq << std::endl;
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
    std::cerr << "[TigFormula::SetFormula] " << mName << " error compiling TFormula" << std::endl;
    delete mFormula;
    mFormula = NULL;
  }
}

