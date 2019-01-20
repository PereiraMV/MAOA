#include <ilcplex/ilocplex.h>
#include <vector>
#include"Graph.h"

#define epsilon 0.01

using namespace::std;


//Find a violated "Menger" Cut by a separation algorithm
bool  find_ViolatedMengerCutCst(IloEnv env, VRP_Graph & G,  vector<vector<IloNumVar> >& x,  IloRange & ViolatedCst){
	  int i,j;
  list<int> W;
  list<int>::const_iterator it,it2;
  vector<int> V_W;
  float test;

  V_W.resize(G.nb_nodes);

  // Find a minimum cut


  test=G.Undirected_MinimumCut(W);

  //cout<<"test = "<<test<<endl;
  
    
  IloExpr expr(env);
  
  bool Wclient=true;
  for (it=W.begin();it!=W.end();it++){
  	if (*it==0){
  		Wclient=false;
  		break;
  	}
  }
  for (i=0;i<G.nb_nodes;i++){
  if (!Wclient) V_W[i]=0;
  else V_W[i]=1;
}


  

  for (it=W.begin();it!=W.end();it++) {
  if (!Wclient) V_W[*it]=1;
  else V_W[*it]=0;
  }
  
//   cout<<"Set W found :";
//   for (it=W.begin();it!=W.end();it++)
//     cout<<*it<<" ";
//   cout<<endl;
//   for (vector<int >::iterator it3=V_W.begin();it3!=V_W.end();it3++)
//     cout<<*it3<<" ";
//   cout<<endl;

  int totalDemand=0;
  
  for (int unsigned w=0;w<V_W.size();w++){
    if (V_W[w]==0){
			totalDemand+=G.demand[w];
     }
	}
	float b=totalDemand/float(G.maxCap);
	b=ceil(b);
	
	if (test>=2*b){
		return false;
	}
	else{


		for (i=0;i<G.nb_nodes;i++){
		  for (j=0;j<G.nb_nodes;j++){
				if (V_W[i]==0 && V_W[j]==1 ) {
					if (i<j){
					expr+=x[i][j];
					}
					else{
					expr+=x[j][i];
					}
				
				}
				
			}
		}

		
    
  ViolatedCst=IloRange(expr >= 2*b);
  return true;
	}

//	int p=1;
//	vector<int> St;
//	St.resize(G.nb_nodes);
//	for(int i=0;i<G.nb_nodes;i++) St[i]=0;
//	vector<int> startingNodes;
//	int nbstart=0;
//	
//	while (nbstart!=int(G.nb_nodes/2)){
//		int node=rand()%(G.nb_nodes-nbstart); 
//		int tp=0;
//		for( int i =0;i<G.nb_nodes;i++){
//			if (St[i]=0 && i==node) {
//				St[i]=1;
//				startingNodes.push_back(i);
//				break;
//			}
//			if (St[i]=0) tp++;
//		}
//	}
//	for(vector<int>::iterator it=startingNodes.begin();it!=startingNodes.end();it++) cout<< startingNodes[*it]<<"  ";

}
