// part of TigSortGUI
// author: Ulrike Hager

#include <TigManager.h>
#include <TigMidasHandler.h>

//VirtualOdb* gOdb = NULL;

TigMidasHandler::TigMidasHandler(TigManager *pParent)
  : mParent(pParent)
  ,mOdb(NULL)
  ,mFile(NULL)
{
  mGuiInterval = 100;
  mHistoUpdateInterval = 500;
}

void 
TigMidasHandler::CloseMidasFile()
{
  if (mOdb) {
      delete mOdb;
      mOdb = NULL;
  }
  if (mFile) {
    mFile->Close();
    delete mFile;
    mFile = NULL;
  }
}

void 
TigMidasHandler::HandleMidasEvent(TMidasEvent& pEvent)
{

  int eventId = pEvent.GetEventId();
  if ( eventId == 1)   //Tig64 data
    {
      mParent->ProcessTig64(&pEvent,"WFDN");
    }
  else if ( eventId == 3)   //scaler data
    {
      mParent->ProcessScaler(&pEvent);
    }
  else
    {
      // unknown event type
      //  cout << "unknown event" << endl;
      //      pEvent.Print();
    }
}

void
TigMidasHandler::EventHandler(const void*pheader,const void*pdata,int size)
{
  TMidasEvent event;
  memcpy(event.GetEventHeader(), pheader, sizeof(TMidas_EVENT_HEADER));
  event.SetData(size, (char*)pdata);
  event.SetBankList();
  HandleMidasEvent(event);
}

int 
TigMidasHandler::OpenMidasFile(const char*fname)
{
  //  cout << "[TigMidasHandler::OpenMidasFile] " << fname << endl; 
  mFile = new TMidasFile();
 bool tryOpen =  mFile->Open(fname);
  if (!tryOpen)
    {
      printf("Cannot open input file \"%s\"\n",fname);
      return -1;
    }
  if (mOdb) {
   delete mOdb;
   mOdb = NULL;
  }
  while (!mOdb )
    {
      TMidasEvent event;
      if (!mFile->Read(&event))
	break;
      int eventId = event.GetEventId();
      if ((eventId & 0xFFFF) == 0x8000)
     	{	  
	  mOdb = new XmlOdb(event.GetData(),event.GetDataSize()) ;
	  mParent->SetRunNumber(event.GetSerialNumber() );
	  mRunNumber = event.GetSerialNumber();
	}
    }
  return mRunNumber;
}

int 
TigMidasHandler::ProcessMidasFile(const char *fname, int eventCutoff)
{
  //  cout << "[TigMidasHandler::ProcessMidasFile] " << fname << endl;
  if (!mFile) OpenMidasFile(fname);
  else if (strcmp(mFile->GetFilename(),fname)!=0) {
    cout << "[TigMidasHandler::ProcessMidasFile] currently open file does not match requested file, opening requested file " << fname << endl;
    CloseMidasFile();
    OpenMidasFile(fname);
    //    return 1;
  }

  //  mTimer->Start(TimerTime, kTRUE);
  int i=0;
  while (mParent->KeepRunning() )
    {
      TMidasEvent event;
      if (!mFile->Read(&event))
	return 1;

      int eventId = event.GetEventId();
      //    printf("Have an event of type %d\n",eventId);
      if ((eventId & 0xFFFF) == 0x8000)
     	{
	  if (mOdb) {
	    delete mOdb;
	  }
	  cout << "[TigMidasHandler::ProcessMidasFile] header" << endl ;
	  mOdb = new XmlOdb(event.GetData(),event.GetDataSize()) ;
	  mParent->SetRunNumber(event.GetSerialNumber() );

	  cout << "[TigMidasHandler::ProcessMidasFile] event run#  " ;
	  cout  << event.GetSerialNumber() << endl;
	  cout << "[TigMidasHandler::ProcessMidasFile] parent run#  " ;
	  cout  << mParent->RunNumber() << endl;
	  cout.flush();
	}
      else if ((eventId & 0xFFFF) == 0x8001) {}
      else
	{
	  event.SetBankList();
	  //event.Print();
	  HandleMidasEvent(event);
	}
	
      if((i%5000)==0)
	{
	  cout << "." ;
	  cout.flush();
	}
      if((i%mGuiInterval)==0)
	{
	   gSystem->ProcessEvents();
	   // mParent->HistoUpdate();
	}      
      if((i%mHistoUpdateInterval)==0)
	{
	  //   gSystem->ProcessEvents();
	  mParent->HistoUpdate();
	}      

      i++;
      if ((eventCutoff!=0)&&(i>=eventCutoff))
	{
	  //	  printf("Reached event %d, exiting loop.\n",i);
	  break;
	}
    }

  //  CloseMidasFile();
    return 0;
  }

void
TigMidasHandler::Update()
{
 mParent->HistoUpdate();
 // gSystem->ProcessEvents();
}
