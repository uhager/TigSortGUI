// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGOBJECT_H
#define TIGOBJECT_H


#include <string>
#include <vector>


class TigObject
{
 public:
  TigObject();

  void ChangeDescription(std::string pDesc) { mDescription = pDesc; }
  void ChangeName(std::string pName)	{ mName = pName; }
  std::string Description(){return mDescription;}
  std::string Name(void) { return mName; }
  virtual bool ParseInput(std::string line);
  std::string ObjType;

 protected:
  std::string mDescription;
  std::string mName;

};


#endif /* TIGOBJECT_H */
