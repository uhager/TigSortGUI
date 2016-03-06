// part of TigSortGUI
// author: Ulrike Hager

#include <TigObject.h>

TigObject::TigObject()
  :mName("tigobject")
{
  ObjType = "Object";
}

bool
TigObject::ParseInput(string line)
{
  //  cout << "[TigObject::ParseInput]" << endl;
  string token;
  istringstream stream(line.c_str());	
  stream >> token;
  //      cout << "[TigObject::ParseInput] " <<  mName << " token " << token << endl;
  if ( token == "" || token[0] == '#') {
    //	cout << "[TigObject::ParseInput] " <<  mName << " comment: " << line << endl;
  }  //comment or blank
  else if ( token.compare("name") == 0)
    {
      stream >> token;
      this->ChangeName(token);
      cout << " ..." << mName << "... ";
      return true;
    }	
  else if ( token.compare("description") == 0)
    {
      stream >> token;
      this->ChangeDescription(token);
      return true;
    }  
  else {
    //	 cout << "[TigObject::ParseInput] unknown token " << token << endl;
    return false;
  }
  return true;
}


