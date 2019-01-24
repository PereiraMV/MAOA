
#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include<ctime>
#include"Graph.h"

bool  find_ViolatedMengerCutCst(IloEnv env, VRP_Graph & G,  vector<vector<IloNumVar> >& x,  vector<IloRange > & ViolatedCst);


