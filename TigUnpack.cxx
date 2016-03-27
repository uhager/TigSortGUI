// part of TigSortGUI
// author: Ulrike Hager

#include <iostream>
#include <vector>
#include <string>

#include "TigUnpack.h"
#include "TigManager.h"


//----- TigUnpack
TigUnpack::TigUnpack(void)
{
}


//---- ProcessData
int
TigUnpack::ProcessData(WORD* pData, long pMaxLength)
{
  //  std::cout << "[TigUnpack::ProcessData]" << std::endl;
  bool bail = false;
  long pChannel, pValue;
  int eventID = 0;
  long index;
  uint32_t type, value, trigger_num;
  TigEvent* event = new TigEvent;

  for ( index = 0; index < pMaxLength; index++)
    {
      type = pData[index] >> 28;
      value = pData[index] & 0x0fffffff;
      //	  std::cout << "data[" << index << "] " << hex<< pData[index] << std::endl;
      switch( type ){
      case 0x0: 
	pValue = value & 0x3fff;
	event->AddWfSample(pValue);
	pValue = ((value & 0x0fffffff)>>14) & 0x3fff;
	event->AddWfSample(pValue);
	break;
      case 0x1:            break;           /*  trapeze data */
      case 0x4:                             /*      CFD Time */
	pValue = value & 0x07ffffff;
	event->SetCFD(pValue);
	break;
      case 0x5:                             /*        Charge */
	pValue = value & 0x3ffffff; // 27th bit is pile-up
	// if (pValue > 65500){
	//   std::cout << "[TigUnpack::ProcessData] large charge value " << pValue << std::endl;
	// }
	if (event->Charge() == -1) {
	  event->SetCharge(pValue); 
	  // if (event->Address() == ) std::cout << "[TigUnpack::ProcessData] eventID " << event->EventID() << " address " << event->Address() << " charge " << event->Charge() << std::endl;
	}else {
	  // corrupted event, discard
	  delete event;
	  return -1;
	}
	break;
      case 0x8:                        /*  Event header */
	pValue = value & 0x00ffffff;
	eventID = pValue;
	if (event->EventID() == 0) {
	  event->SetEventID(pValue);
	}else {
	  TigManager::Instance().ProcessSignal(event);
	  delete event;
	  event = new TigEvent;
	  event->SetEventID(eventID);
	}
	break;
      case 0xa:
	uint32_t subtype;
	subtype = (pData[index] & 0x0f000000) >> 24;
	pValue = value & 0x00ffffff;
	if (subtype==0) event->SetTimestamp(pValue);
	else if (subtype==1) event->SetTimestampUp(pValue);
	else if (subtype==2) event->SetLifetime(pValue);
	else if (subtype==4) event->SetTriggersRequested(pValue);
	else if (subtype==5) event->SetTriggersAccepted(pValue);
	break;
      case 0xb: break;                      /*  Trigger Pattern*/
      case 0xc:                            /*   New Channel */
	if (event->Address() == -1) event->SetAddress(value);
	else {
	  TigManager::Instance().ProcessSignal(event);
	  delete event;
	  event = new TigEvent;
	  event->SetEventID(eventID);
	  event->SetAddress(value);
	}
	break;
      case 0xe:                                     /* Event Trailer */ 
	TigManager::Instance().ProcessSignal(event);
	// std::cout << "processing event, channel " <<  pChannel << dec<<" value "<< pValue<< std::endl;
	delete event;
	event = new TigEvent;
	break;
      case 0x6:                            /*   LED */
	pValue = value & 0x07ffffff;
	event->SetLED(pValue);
	break;
      default: {
	std::cerr << "unknown entry " << type << std::endl;
	return(-1);
      }
      }
    }
  delete event;
  return index;
}

/************ TigMCSUnpack ****************/

//----- TigUnpack
TigMCSUnpack::TigMCSUnpack(void)
{
}


//---- ProcessData
std::vector<int>
TigMCSUnpack::ProcessData(WORD* pData, long pMaxLength)
{
 long pValue;
 long index;
 std::vector<int> values;

 for ( index = 0; index < pMaxLength; index++)
   {
     pValue = pData[index] & 0xffffffff;
     values.push_back(pValue);
   }
 // TigManager::Instance().ProcessScaler(values);
 return values;
}	  
	  

