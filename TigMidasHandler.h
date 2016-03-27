// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGMIDASHANDLER_H
#define TIGMIDASHANDLER_H

class TigMidasHandler;

#include <TMidasEvent.h>
#include <TMidasFile.h>
#include <XmlOdb.h>
/* #ifdef OLD_SERVER */
/* #include <midasServer.h> */
/* #endif */
/* #ifdef HAVE_LIBNETDIRECTORY */
/* #include <libNetDirectory/netDirectoryServer.h> */
/* #endif */

#include <TSystem.h>
#include <TROOT.h>
#include <TTimer.h> 


class TigManager;


class TigMidasHandler
{
  //  RQ_OBJECT("TigMidasHandler")
 public:
  TigMidasHandler(TigManager *pParent);
  virtual ~TigMidasHandler() {
    CloseMidasFile();
      }
  void CloseMidasFile();
  void EventHandler(const void *pheader,const void *pdata,int size);
  int OpenMidasFile(const char*fname);
  int ProcessMidasFile(const char*fname, int eventCutoff);
  int RunNumber() {return mRunNumber;}
  void SetGuiInterval(int time){mGuiInterval = time;}
  void SetHistoUpdateInterval(int time){mHistoUpdateInterval = time;}
  void Update();

 protected:
  VirtualOdb *mOdb;
 TigManager *mParent;
  void HandleMidasEvent(TMidasEvent& pEvent);
  TMidasFile *mFile;
  int mRunNumber;
  int mGuiInterval;
  int mHistoUpdateInterval;

 private:
  TigMidasHandler( const TigMidasHandler& );
  TigMidasHandler& operator=( const TigMidasHandler& );
};

#endif
