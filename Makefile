#------------------------------------------------------------
#
# Please define an environnement variable PATHTUTOMIP
# equal to the XXX directory where Lemon,Graphviz and Cplex have been installed
# In order to these:
# edit ~/.bashrc add "export PATHTUTOMIP=XXX"
# close your xterm and star new ones
#
#------------------------------------------------------------

CPLEXDIR      = $(PATHTUTOMIP)/cplex-12.7.1/cplex
CONCERTDIR    = $(PATHTUTOMIP)/cplex-12.7.1/concert
LEMONDIR      = $(PATHTUTOMIP)/lemon-1.3.1


SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic


# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0


# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
LEMONLIBDIR	= $(LEMONDIR)/lib

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) -L$(LEMONLIBDIR)


CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
LEMONINCDIR   = $(LEMONDIR)/include

CCLNFLAGS = -lconcert -lilocplex -lcplex -lemon -lm -lpthread

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) -I$(LEMONINCDIR)




# ---------------------------------------------------------------------
# Your part
# ---------------------------------------------------------------------



all: CompactMIP_VRP NoCompactMIP_VRP Heuristic_VRP.o Heuristic_TSPfunc.o

Graph.o: Graph.cpp Graph.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c Graph.cpp -o Graph.o

CompactMIP_VRP.o: CompactMIP_VRP.cpp
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) CompactMIP_VRP.cpp

CompactMIP_VRP: CompactMIP_VRP.o  Graph.o 
	$(CCC) $(CCFLAGS) $(CCLNDIRS) CompactMIP_VRP.o Graph.o   -o CompactMIP_VRP $(CCLNFLAGS)
	
MengerCutSeparation.o: MengerCutSeparation.cpp Graph.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) -c MengerCutSeparation.cpp
	
NoCompactMIP_VRP.o: NoCompactMIP_VRP.cpp Graph.h Heuristic_VRP.h Heuristic_TSPfunc.h
	$(CCC) -c $(CCFLAGS) $(CCLNDIRS) NoCompactMIP_VRP.cpp

NoCompactMIP_VRP: NoCompactMIP_VRP.o  Graph.o MengerCutSeparation.o Heuristic_VRP.o Heuristic_TSPfunc.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) NoCompactMIP_VRP.o Graph.o Heuristic_TSPfunc.o MengerCutSeparation.o Heuristic_VRP.o  -o NoCompactMIP_VRP $(CCLNFLAGS)



Heuristic_VRP.o: Heuristic_VRP.cpp Graph.h Heuristic_TSPfunc.h Heuristic_VRP.h
	g++ -c $(CCFLAGS) $(CCLNDIRS) Heuristic_VRP.cpp 
	
Heuristic_TSPfunc.o:Heuristic_TSPfunc.cpp Graph.h Heuristic_TSPfunc.h 
	g++ -c $(CCFLAGS) $(CCLNDIRS) Heuristic_TSPfunc.cpp 

clean:
	rm -f *.o CompactMIP_VRP NoCompactMIP_VRP Heuristic_TSPfunc MengerCutSeparation Heuristic_VRP
