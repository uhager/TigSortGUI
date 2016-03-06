// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGOBJECT_H
#define TIGOBJECT_H


#include <string>
#include <vector>
#include <sstream>
#include <iostream>
//#include <TObject.h>

using namespace std;

class TigObject
{
 public:
  TigObject();

  void ChangeDescription(string pDesc) { mDescription = pDesc; }
  void ChangeName(string pName)	{ mName = pName; }
  string Description(){return mDescription;}
  string Name(void) { return mName; }
  virtual bool ParseInput(string line);
  string ObjType;

 protected:
  string mDescription;
  string mName;

  //  ClassDef(TigObject,0);
};


#endif /* TIGOBJECT_H */
