ObjSuf		= o
SrcSuf		= C
DllSuf		= so
OutPutOpt	= -o
ROOTLIBS	= $(shell root-config --libs)
ROOTGLIBS	= $(shell root-config --glibs)
ROOTCFLAGS	= $(shell root-config --cflags)


CXX		= g++
CXXFLAGS	= -O2 -Wall -fPIC
LD		= g++
SOFLAGS		= -shared
LIBS		= $(ROOTLIBS) $(ROOTGLIBS)
CXXFLAGS       += $(ROOTCFLAGS) -Iinclude
LIBS	       += -lSpectrum -lMinuit

.PHONY: all
all: MWPC_pos_landau.exe correctData.exe posMap.exe simData.exe gainFactors.exe

MWPC_pos_landau.exe: MWPC_pos_landau.o
	$(CXX) $(CXXFLAGS) -o MWPC_pos_landau.exe MWPC_pos_landau.o $(LIBS) 
	@echo "Done"

correctData.exe: correctData.o
	$(CXX) $(CXXFLAGS) -o correctData.exe correctData.o $(LIBS) 
	@echo "Done"

posMap.exe: posMap.o
	$(CXX) $(CXXFLAGS) -o posMap.exe posMap.o $(LIBS) 
	@echo "Done"

simData.exe: simData.o
	$(CXX) $(CXXFLAGS) -o simData.exe simData.o $(LIBS) 
	@echo "Done"

gainFactors.exe: gainFactors.o
	$(CXX) $(CXXFLAGS) -o gainFactors.exe gainFactors.o $(LIBS) 
	@echo "Done"

clean:
		@rm -f *.o *~  core
