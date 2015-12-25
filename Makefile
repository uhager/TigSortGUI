## Makefile for TigSortGUI
## author: Ulrike Hager
## Make sure ROOTSYS and ROOTANA are set correctly!

ROOT_INCLUDES += -DHAVE_ROOT `root-config --cflags`

ROOTLIBS  = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs)  -lXMLParser -lThread
ROOTGLIBS = -Wall -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs) -lXMLParser -lThread
#RPATH    += -Wl,-rpath,$(ROOTSYS)/lib
CXXFLAGS += -DHAVE_ROOT $(shell $(ROOTSYS)/bin/root-config --cflags) -I.
#OBJS = TigTree.o TigEvent.o TigDetector.o TigTimer.o TigUnpack.o TigSort.o TigManager.o TigScaler.o  TigMidasHandler.o  TigManagerDICT.o TigFrameDICT.o TigFrame.o
OBJS =   TigDICT.o TigTree.o TigDataObject.o TigLookupTable.o TigHistoObject.o TigEvent.o TigDetector.o  TigUnpack.o TigSort.o TigManager.o TigObject.o TigScaler.o  TigMidasHandler.o TigDetFrame.o TigFrame.o TigFormula.o TigHisto.o TigScalerHisto.o  TigSortObject.o TigCut.o TigWaveformHisto.o TigCalibration.o
HDRS = TigFrame.h TigDetFrame.h TigManager.h

ifdef MIDASSYS
MIDASLIBS = $(MIDASSYS)/linux/lib/libmidas.a -lutil -lrt
CXXFLAGS += -DHAVE_MIDAS -DOS_LINUX -Dextname -I$(MIDASSYS)/include
endif

ROOTANA_INCLUDES = `root-config --cflags` -I${ROOTANA}/include/
ROOTANA_LIBS =  -L${ROOTANA} ${ROOTANA}/lib/librootana.a 

#all: tigmanager tigframe $(OBJS) tigsort
#all:  TigFrameDICT.cxx TigDetFrameDICT.cxx TigManagerDICT.cxx TigLinkDefDICT.cxx TigDataDICT.cxx $(OBJS)  tigsortGUI
all:  TigDICT.cxx  $(OBJS) tigsortGUI

TigDICT.cxx: $(HDRS)
	rootcint -f $@ -c $(CXXFLAGS) -p -I. $(ROOTANA_INCLUDES) $^ 
%.o: %.cxx 
	$(CXX) $(CXXFLAGS) $(ROOTANA_INCLUDES) -o $@ -c $<
lib%.so: %.cxx 
	$(CXX) -shared -o$@ `root-config --ldflags` $(CXXFLAGS) -fPIC $(ROOTANA_INCLUDES)  $^
%.a: %.o
	ar rvs $@ $^
tigsortGUI: $(OBJS)
	$(CXX) $(CXXFLAGS)  $(OBJS) $(ROOTANA_LIBS) $(MIDASLIBS)  $(ROOTLIBS) $(ROOTGLIBS) -lz -o $@ 

clean:
	rm -f *.o tigsort tigsortGUI *DICT.cxx *DICT.h *.so G__auto* 

