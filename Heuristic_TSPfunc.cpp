
#include"Heuristic_TSP.h"



using namespace std;


//#define OUTPUT 

///////////////////////////////
///////////////////////////////

// Greedy Heuristic + 2-OPT descent for the TSP problem
// Given a complete undirected graph G=(V,E)

// Greedy Algorithm
// S <- 0
// While |S|<n
//   Found the closest node from S
//   Add it to S

// 2-OPT descent from the greedy solution
// For NB-ITER iteration
//    Randomly choose one edge ij of the solution
//    Randomly choose one non-incident edge kl of the solution
//    Stochatic test for accepting ik jl instead of ij kl

///////////////////////////////
///////////////////////////////


void greedy_heuristic(C_Graph &G, vector<int>& sol){
  int i,k;
  
  vector<int> added;
  added.resize(G.nb_nodes);    
  for (i=0;i<G.nb_nodes;i++) added[i]=0;
  
  int curr=0;
  float next;
  int nexti;
  
  sol[0]=0;
  added[0]=1;
  k=1;
  while (k<G.nb_nodes){

    next=10e6;
    for (i=0;i<G.nb_nodes;i++)
      if (added[i]==0)
	if (next>G.lengthTSP(curr,i)){
	  next=G.lengthTSP(curr,i);
	  nexti=i;
	}
    sol[k]=nexti;
    added[nexti]=1;
    curr=nexti;
    k++;
  }

}

void TwoOptDescent(C_Graph &G, double& curr_length, vector<int>& curr_sol, double& best_length, list<pair<int,int> >& best_sol, int NB_ITER){
  int k;
  int nbiter;
  int k1,k2;
  int jm,j,jp,lm,l,lp;
  bool found;
  double v1,v2;
  
  for (nbiter=0;nbiter<NB_ITER;nbiter++){

    k1=rand()%G.nb_nodes;
    k2=rand()%(G.nb_nodes-1)+1;
    k2=(k1+k2)%G.nb_nodes;

    if (k1==0) jm=curr_sol[G.nb_nodes-1];
       else jm=curr_sol[(k1-1)%G.nb_nodes];
    j=curr_sol[k1];
    jp=curr_sol[(k1+1)%G.nb_nodes];
    if (k2==0) lm=curr_sol[G.nb_nodes-1];
       else lm=curr_sol[(k2-1)%G.nb_nodes];
    l=curr_sol[k2];
    lp=curr_sol[(k2+1)%G.nb_nodes];
    
    if (k2==(k1+1)%G.nb_nodes){
      v1=G.lengthTSP(jm,j) + G.lengthTSP(l,lp);
      v2=G.lengthTSP(jm,l) + G.lengthTSP(j,lp);
    }
    else
      if (k1==(k2+1)%G.nb_nodes){
	v1=G.lengthTSP(lm,l) + G.lengthTSP(j,jp);
	v2=G.lengthTSP(lm,j) + G.lengthTSP(l,jp);
      }
      else{
	v1=G.lengthTSP(jm,j) + G.lengthTSP(j,jp) + G.lengthTSP(lm,l) + G.lengthTSP(l,lp) ;
	v2=G.lengthTSP(jm,l) + G.lengthTSP(l,jp) + G.lengthTSP(lm,j) + G.lengthTSP(j,lp);
      }

    if (v1>v2){
      curr_length=curr_length-v1+v2;
      curr_sol[k1]=l;
      curr_sol[k2]=j;	    
    }
    else{
      if (rand()%1000<1){
    	curr_length=curr_length-v1+v2;
  	curr_sol[k1]=l;
  	curr_sol[k2]=j;	    
      }      
    }

    double test=0;
    for(k=1; k<G.nb_nodes;k++)
      test+=G.lengthTSP(curr_sol[k-1],curr_sol[k]);
    test+=G.lengthTSP(curr_sol[G.nb_nodes-1],curr_sol[0]);
    
    if (best_length>curr_length){
      best_sol.clear();
      for(k=1; k<G.nb_nodes;k++) {
 	best_sol.push_back(make_pair(G.V_nodes[curr_sol[k-1]].num,G.V_nodes[curr_sol[k]].num));
      }  
      best_sol.push_back(make_pair(G.V_nodes[curr_sol[G.nb_nodes-1]].num,G.V_nodes[curr_sol[0]].num));
      best_length=curr_length;    
    }
    
    #ifdef OUTPUT
    cout<<"Curr sol: "<<curr_length<<" /" <<best_length<<endl;
    #endif
  }

}

