// TigSortGUI
// author: Ulrike Hager
// based on ROOT analyzer by K.Olchanski

#include <stdio.h>
#include <sys/time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <signal.h>

#include <TMidasOnline.h>
#include <TMidasEvent.h>
#include <TMidasFile.h>
#include <XmlOdb.h>
#ifdef OLD_SERVER
#include <midasServer.h>
#endif
#ifdef HAVE_LIBNETDIRECTORY
#include <libNetDirectory/netDirectoryServer.h>
#endif

#include <TSystem.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TTimer.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TFolder.h>

#include "TigFrame.h"

// data sizes

// Global Variables
int  gRunNumber = 0;
bool gIsRunning = false;
bool gIsPedestalsRun = false;
int  gEventCutoff = 0;

std::string configFile = "-";
TDirectory* gOnlineHistDir = NULL;
TFile* gOutputFile = NULL;


//TigManager gCatalystMgr;
#define M_FILE_EXIT 0



void help()
{
  printf("\nUsage:\n");
  printf("\n./analyzer.exe [-h] [-Hhostname] [-Eexptname] [-eMaxEvents] [-P9091] [-p9090] [-m] [-g] [file1 file2 ...]\n");
  printf("\n");
  printf("\t-h: print this help message\n");
  printf("\t-c: config file (mapping)\n");
  printf("\t-T: test mode - start and serve a test histogram\n");
  printf("\t-Hhostname: connect to MIDAS experiment on given host\n");
  printf("\t-Eexptname: connect to this MIDAS experiment\n");
  printf("\t-P: Start the TNetDirectory server on specified tcp port (for use with roody -Plocalhost:9091)\n");
  printf("\t-p: Start the old midas histogram server on specified tcp port (for use with roody -Hlocalhost:9090)\n");
  printf("\t-e: Number of events to read from input data files\n");
  printf("\n");
  printf("Example1: analyze online data: ./analyzer.exe -P9091\n");
  printf("Example2: analyze existing data: ./analyzer.exe /data/alpha/current/run00500.mid\n");
  exit(1);
}

// Main function call
void tigsort()
{
  new TigFrame(gClient->GetRoot(), 400, 220);
}

int main(int argc, char *argv[])
{
   setbuf(stdout,NULL);
   setbuf(stderr,NULL);

   signal(SIGILL,  SIG_DFL);
   signal(SIGBUS,  SIG_DFL);
   signal(SIGSEGV, SIG_DFL);

   std::vector<std::string> args;
   for (int i=0; i<argc; i++)
     {
       if (strcmp(argv[i],"-h")==0)
	 help(); // does not return
       args.push_back(argv[i]);
     }

  
   TApplication *rootApp = new TApplication("TigSort", &argc, argv);

   //   TigManager *manager = new TigManager();

   if(gROOT->IsBatch()) {
   	printf("Cannot run in batch mode\n");
	return 1;
   }

   int  oldTcpPort = 0;
   int  tcpPort = 0;
   const char* hostname = NULL;
   const char* exptname = NULL;

   for (unsigned int i=1; i<args.size(); i++) // loop over the commandline options
     {
       const char* arg = args[i].c_str();
       //printf("argv[%d] is %s\n",i,arg);
	   
       if (strncmp(arg,"-e",2)==0)  // Event cutoff flag (only applicable in offline mode)
	 gEventCutoff = atoi(arg+2);
       else if (strncmp(arg,"-p",2)==0) // Set the histogram server port
	 oldTcpPort = atoi(arg+2);
       else if (strncmp(arg,"-P",2)==0) // Set the histogram server port
	 tcpPort = atoi(arg+2);
       else if (strncmp(arg,"-H",2)==0)
	 hostname = strdup(arg+2);
       else if (strncmp(arg,"-E",2)==0)
	 exptname = strdup(arg+2);
       else if (strncmp(arg,"-c",2)==0)
	 configFile = strdup(arg+2);
       else if (strcmp(arg,"-h")==0)
	 help(); // does not return
       else if (arg[0] == '-')
	 help(); // does not return
    }

   tigsort();    
   rootApp->Run(); 
   //  TigManager::Instance().Clear();
   // gApplication->Terminate(0);


   return 0;
}

//end



