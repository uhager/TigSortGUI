// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGUNPACK_H
#define TIGUNPACK_H

#include <stdint.h>

#include "TigEvent.h"

#ifndef WORD
#define WORD uint32_t
#endif

class TigUnpack
{
 public:
  TigUnpack();
  virtual ~TigUnpack(){}; 
   int ProcessData(WORD* pData, long pMaxLength);

 private:			//----- private --------------------
   TigUnpack(const TigUnpack& );
   TigUnpack& operator=(const TigUnpack& );

};


class TigMCSUnpack
{
 public:
  TigMCSUnpack();
  virtual ~TigMCSUnpack(){}; 
  std::vector<int> ProcessData(WORD* pData, long pMaxLength);

 private:			//----- private --------------------
   TigMCSUnpack(const TigMCSUnpack& );
   TigMCSUnpack& operator=(const TigMCSUnpack& );

};


#endif /* TIGUNPACK_H*/
