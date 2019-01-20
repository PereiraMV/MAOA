

#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include<ctime>
#include"Graph.h"


void greedy_heuristic(C_Graph &G, vector<int>& sol);


void TwoOptDescent(C_Graph &G, double& curr_length, vector<int>& curr_sol, double& best_length, list<pair<int,int> >& best_sol, int NB_ITER);
