#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include<ctime>
#include"Graph.h"
#include "Heuristic_TSPfunc.h"



void LocalResearch(VRP_Graph &G,vector<vector<pair<int,int> > >& curr_sol,vector<list<pair<int,int> > > & solution,int NB_ITER);

