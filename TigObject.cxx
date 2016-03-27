// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <sstream>

#include "TigObject.h"

TigObject::TigObject()
  :mName("tigobject")
{
  ObjType = "Object";
}

bool
TigObject::ParseInput(std::string line)
{
  //  std::cout << "[TigObject::ParseInput]" << std::endl;
  std::string token;
  std::istringstream stream(line.c_str());	
  stream >> token;
  //      std::cout << "[TigObject::ParseInput] " <<  mName << " token " << token << std::endl;
  if ( token == "" || token[0] == '#') {
    //	std::cout << "[TigObject::ParseInput] " <<  mName << " comment: " << line << std::endl;
  }  //comment or blank
  else if ( token.compare("name") == 0)
    {
      stream >> token;
      this->ChangeName(token);
      std::cout << " ..." << mName << "... ";
      return true;
    }	
  else if ( token.compare("description") == 0)
    {
      stream >> token;
      this->ChangeDescription(token);
      return true;
    }  
  else {
    //	 std::cout << "[TigObject::ParseInput] unknown token " << token << std::endl;
    return false;
  }
  return true;
}


