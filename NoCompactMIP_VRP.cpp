#include <ilcplex/ilocplex.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include"Graph.h"
#include "MengerCutSeparation.h"
#include "Heuristic_VRP.h"

#define epsilon 0.0001

using namespace std;

#define OUTPUT

///////////////////////////////
///////////////////////////////
//Separation//
// Usefull inequalities 


// Usefull inequalities 
ILOUSERCUTCALLBACK2(UsercutMengerCutSeparation,VRP_Graph &, G,vector<vector<IloNumVar> >&,x){
#ifdef OUTPUT
  //cout<<"********* UserCut separation Callback *************"<<endl;
  #endif

  int i;
  list<C_link *>::const_iterator it;
  vector<IloRange > ViolatedCst;
  
  // Put the linear relaxation values on the edges of graph G

	
  for (i=0;i<G.nb_nodes;i++){
  	//cout <<"Voici i "<<i<<"  "<< (*G.V_nodes[i].L_adjLinks.begin())->return_other_extrem(i)<<endl;
    for (it=G.V_nodes[i].L_adjLinks.begin();it!=G.V_nodes[i].L_adjLinks.end();it++){
    		if (i<(*it)->return_other_extrem(i)){
		  		int j=(*it)->return_other_extrem(i);
		  		(*it)->algo_cost=getValue(x[i][j]);
		  		//cout << i<<" "<< j<<" " <<getValue(x[i][j])<<endl;
					if((*it)->algo_cost<epsilon)(*it)->algo_cost=0 ;
      }
    }
  }
    
  /* Separation of Cut inequalities */

  if (find_ViolatedMengerCutCst(getEnv(),G,x, ViolatedCst)){

    #ifdef OUTPUT
    //cout << "Adding constraint : "<<endl;
    //cout<< ViolatedCst << endl;
    #endif

    for(vector<IloRange >::iterator j=ViolatedCst.begin();j!=ViolatedCst.end();j++){
    	add(*j,IloCplex::UseCutPurge);   // UseCutForce UseCutPurge UseCutFilter
    }
    
  }
  #ifdef OUTPUT
    else {
//      cout<<"No Cst found"<<endl;
    }
  #endif
}


bool check_Cycle(IloEnv env, VRP_Graph & G,  vector<vector<IloNumVar> >& x,  IloRange & ViolatedCst,vector<int> cycle){
		int TotalDemand=0;
		int zero=false;
	for (vector<int> ::iterator it=cycle.begin();it!=cycle.end();it++){
		TotalDemand+=G.demand[*it];
		if (*it==0) zero=true;
	}
	float b=TotalDemand/float(G.maxCap);
	b=ceil(b);
	//cout<<TotalDemand<<" "<<float(G.maxCap)<<"  "<<b<<" "<<int(b)<<endl;
	
	if (TotalDemand<= G.maxCap && zero==true){
		return false;
	}
	
	IloExpr expr(env);
	vector<int> nonW;
	nonW.push_back(0);
	for(int i=1;i<G.nb_nodes;i++){
		bool present=false;
		for (vector<int> ::iterator it=cycle.begin();it!=cycle.end();it++){
			if (*it==i) present=true;
		}
		if (!present) nonW.push_back(i);
	}
//	for (vector<int>::iterator it=cycle.begin();it!=cycle.end();it++){
//		cout<<*it<<" ";
//	}
//	cout<<endl;
//	for (vector<int>::iterator it=nonW.begin();it!=nonW.end();it++){
//		cout<<*it<<" ";
//	}
//	cout<<endl;
	
	
	for (vector<int>::iterator it=cycle.begin();it!=cycle.end();it++){
		if (*it==0) continue;
		for (vector<int>::iterator it2=nonW.begin();it2!=nonW.end();it2++){
			if (*it>*it2) expr+=x[*it2][*it];
			else expr+=x[*it][*it2];
		
		}
	}

	ViolatedCst=IloRange(expr >= 2*b);
	return true;

}





bool  find_ViolatedMengerCutCst_INTEGER(IloEnv env, VRP_Graph & G,  vector<vector<IloNumVar> >& x,  IloRange & ViolatedCst){
  int i,u,v, start;
  list<int>::const_iterator it;
  list<C_link *>::iterator ita;
  vector<int> visited;
	vector<int > cycle;
  bool allVisited=false;
  for (i=0;i<G.nb_nodes;i++) visited.push_back(0);
  start=rand()%(G.nb_nodes-1)+1; //tout les noeud possible sauf le 0
  u=start;
  while (!allVisited){
		visited[u]=1;
  	cycle.push_back(u);

  	bool noNeigbor=true;
  	for(ita=G.V_nodes[u].L_adjLinks.begin();ita!=G.V_nodes[u].L_adjLinks.end();ita++){
  		//cout<<(*ita)->algo_cost<<u<<"   "<<(*ita)->return_other_extrem(u)<< visited[(*ita)->return_other_extrem(u)]<<visited[u]<<endl;
  		if((*ita)->algo_cost==1 && visited[(*ita)->return_other_extrem(u)]==0){
  			noNeigbor=false;
  			v=u;
  			u=(*ita)->return_other_extrem(u);
  			if (u==0){
  				bool visit=true;
  				for (i=G.nb_nodes-1;i>=1;i--){
  					
						if (visited[i]==0) {
							visit=false;
							start=i;
							break;
						}
					}
					allVisited=visit;
					u=start;
					cycle.clear();
					break;
  			}
  			break;
  		}
  		if ((*ita)->algo_cost==1 &&  (*ita)->return_other_extrem(u) == start && (*ita)->return_other_extrem(u)!=v ){
  			
  		  //un cycle trouvé .
//  		  cout<<"Debut cycle"<<start<<endl;
//				for (vector<int>::iterator it=cycle.begin();it!=cycle.end();it++){
//					
//					cout<<*it<<"  ";
//				}
//				cout<<endl;
				if (check_Cycle(env, G,  x, ViolatedCst,cycle)) return true;
				break;
  		}

  	}
  	if (noNeigbor){
			bool visit=true;
			for (i=G.nb_nodes-1;i>=1;i--){
			
				if (visited[i]==0) {
					visit=false;
					start=i;
					break;
				}
			}
			allVisited=visit;
			u=start;
			cycle.clear();
		
		}
	}
	//Ici normalement on a que des cycles passant par 0



  for (i=0;i<G.nb_nodes;i++) visited[i]=0;
  start=0;
  u=start;
  v=0;
  int cptvisit=1;
	cycle.clear();
	allVisited=false;
  
  while (!allVisited){
		visited[u]=1;
  	cycle.push_back(u);
  	bool noNeigbor=true;
		for(ita=G.V_nodes[u].L_adjLinks.begin();ita!=G.V_nodes[u].L_adjLinks.end();ita++){
			//cout<<(*ita)->algo_cost<<" u="<<u<<"  b="<<(*ita)->return_other_extrem(u)<<" visited(b)="<< visited[(*ita)->return_other_extrem(u)]<<endl;
			
			if((*ita)->algo_cost==1 && visited[(*ita)->return_other_extrem(u)]==0){
				noNeigbor=false;
  			v=u;
  			u=(*ita)->return_other_extrem(u);
  			//cout<<v<<" "<<u<<" "<<endl;
  			break;
  		}
  		if ((*ita)->algo_cost==1 &&  (*ita)->return_other_extrem(u) == 0 && (*ita)->return_other_extrem(u)!=v ){
  			
  		  //un cycle trouvé .
//  		  cout<<"Debut cycle"<<start<<endl;
//				for (vector<int>::iterator it=cycle.begin();it!=cycle.end();it++){
//					
//					cout<<*it<<"  ";
//				}
//				cout<<endl;
				if (check_Cycle(env, G,  x, ViolatedCst,cycle)){
					 return true;
				}
				u=0;
				cycle.clear();
				noNeigbor=true;
				break;
  		}
  		
		
		
		}
		if (noNeigbor){
			bool visit=true;
			for (i=G.nb_nodes-1;i>=1;i--){
				if (visited[i]==0) {
					visit=false;
					start=i;
					break;
				}
			}
			allVisited=visit;
			u=0;
			cycle.clear();
		}
		
	}
	

	
	return false;
  
  
}



ILOLAZYCONSTRAINTCALLBACK2(LazyMengerCutSeparation,
			   VRP_Graph &, G,			 
			   vector<vector<IloNumVar> >&,x
		    ){
  #ifdef OUTPUT
  //cout<<"*********** Lazy separation Callback *************"<<endl;
  #endif

  int i;
  list<C_link *>::const_iterator it;
  IloRange ViolatedCst;
  // Put the INTEGER values x on the edges of graph G
  // Be carefull... Cplex can send -0.00001 for 0 or 0.000099 for 1

  for (i=0;i<G.nb_nodes;i++)
    for (it=G.V_nodes[i].L_adjLinks.begin();it!=G.V_nodes[i].L_adjLinks.end();it++){
      if (i<(*it)->return_other_extrem(i)){
	(*it)->algo_cost= getValue(x[i][(*it)->return_other_extrem(i)]);
	if((*it)->algo_cost<epsilon) (*it)->algo_cost=0 ;
	else (*it)->algo_cost=1 ;
	//cout<<i<<"  "<<(*it)->return_other_extrem(i)<<"  value:"<<(*it)->algo_cost<<endl;

      }
    }

  /* Separation of Cut inequalities */

  if (find_ViolatedMengerCutCst_INTEGER(getEnv(),G,x, ViolatedCst)){
    #ifdef OUTPUT
//    cout << "Adding constraint : "<<endl;
//    cout<< ViolatedCst << endl;
    #endif
    add(ViolatedCst,IloCplex::UseCutPurge);   // UseCutForce UseCutPurge UseCutFilter
  }
  #ifdef OUTPUT
    else {
      //cout<<"No Cst found"<<endl;
    }
  #endif 

}


int main (int argc, char**argv){

  string name, nameext, nameextsol;
  int i,j,k;

  vector<int> sol;


  //////////////
  //////  DATA
  //////////////

  if(argc!=2){
    cerr<<"usage: "<<argv[0]<<" <TSP file name>   (without .vrp)"<<endl; 
    return 1;
  }

  name=argv[1];
  nameext=name+".vrp";
  nameextsol=name+".vrpsol";

  ifstream fic(nameext.c_str());

  if (fic==NULL){
    cerr<<"file "<<nameext<<" not found"<<endl;
    return 1;
  }

  VRP_Graph G;
  G.read_undirected_VRP(fic);

  fic.close();


  //////////////
  //////  CPLEX INITIALIZATION
  //////////////


  IloEnv   env;
  IloModel model(env);



  ////////////////////////
  //////  VAR
  ////////////////////////


  vector<vector<IloNumVar> > x;
  
  
  x.resize(G.nb_nodes);
  

  for (i=0;i<G.nb_nodes;i++)
    x[i].resize(G.nb_nodes);

  	

    for (i=0;i<G.nb_nodes;i++){
    for (j=i+1;j<G.nb_nodes;j++) {
    if (i==0){
    	x[i][j]=IloNumVar(env, 0.0, 2.0, ILOINT);
    }
    else{
    
	x[i][j]=IloNumVar(env, 0.0, 1.0, ILOINT);
	}
	ostringstream varname;
	varname.str("");
	varname<<"x_"<<i<<"_"<<j;
	x[i][j].setName(varname.str().c_str());
    }
  }



  
  //////////////
  //////  CST
  //////////////

  IloRangeArray CC(env);
  int nbcst=0;
  //Contrainte 1 
  IloExpr c1(env);
	for (j=1;j<G.nb_nodes;j++){
		c1+=x[0][j];
	}
	CC.add(c1<=2*G.nbTruck);


	nbcst=1;


  // Contraite 3 et 4 sum_{j=1 to n, j!=i} x_ij = 1   for all node i=1 to n
  for (i=1;i<G.nb_nodes;i++){
    IloExpr c1(env);
    for (j=0;j<G.nb_nodes;j++){
				if (i<j) c1+=x[i][j];
				if (i>j) c1+=x[j][i];
			}
		//cout<<endl;
    CC.add(c1==2);
    ostringstream nomcst;
    nomcst.str("");
    nomcst<<"CstDeg"<<i;
    CC[nbcst].setName(nomcst.str().c_str());
    nbcst++;
  }






	
  model.add(CC);
  
  
	IloCplex cplex(model);
	//cplex.setParam(IloCplex::TiLim,50);
	cplex.use(LazyMengerCutSeparation(env,G,x));
	cplex.use(UsercutMengerCutSeparation(env,G,x));
  //////////////
  ////// OBJ
  //////////////

	
  
  IloObjective obj=IloAdd(model, IloMinimize(env, 0.0));
  
  for (i=0;i<G.nb_nodes;i++)
    for (j=i+1;j<G.nb_nodes;j++)
    	if (i!=j)
			obj.setLinearCoef(x[i][j],G.lengthTSP(i,j));
 

  ///////////
  //// RESOLUTION
  //////////



  // cplex.setParam(IloCplex::Cliques,-1);
  // cplex.setParam(IloCplex::Covers,-1);
  // cplex.setParam(IloCplex::DisjCuts,-1);
  // cplex.setParam(IloCplex::FlowCovers,-1);
  // cplex.setParam(IloCplex::FlowPaths,-1);
  // cplex.setParam(IloCplex::FracCuts,-1);
  // cplex.setParam(IloCplex::GUBCovers,-1);
  // cplex.setParam(IloCplex::ImplBd,-1);
  // cplex.setParam(IloCplex::MIRCuts,-1);
  // cplex.setParam(IloCplex::ZeroHalfCuts,-1);
  // cplex.setParam(IloCplex::MCFCuts,-1);
  // cplex.setParam(IloCplex::MIPInterval,1);
  // cplex.setParam(IloCplex::HeurFreq,-1);
  // cplex.setParam(IloCplex::ClockType,1);
  // cplex.setParam(IloCplex::RINSHeur,-1);


  #ifdef OUTPUT
  cout<<"Wrote LP on file"<<endl;
  cplex.exportModel("sortie.lp");
  #endif
  
    #ifdef OUTPUT
  cout<<"Wrote LP on file"<<endl;
  cplex.exportModel("sortie.lp");
  #endif

  //START FROM A HEURISTIC SOLUTION
  srand(time(NULL));
  vector<vector<pair<int,int> > > curr_sol;
  //curr_sol is the affectation
  curr_sol.resize(G.nbTruck);
  vector<list<pair<int,int> > >solution;
  //solution is the affectation and also salesman done, vector=all trucks;list=all pairs in a truck.
	int NB_ITER=100;
	GeneticAlgorithm(G,curr_sol,solution,NB_ITER);
  //LocalResearch(G,curr_sol,solution,NB_ITER);
  // Translate from encoding by a list of nodes to variable x
  vector<vector<int> > startx;
  startx.resize(G.nb_nodes);

  for (i=0;i<G.nb_nodes;i++)  startx[i].resize(G.nb_nodes);

  for (i=0;i<G.nb_nodes;i++)
    for (j=i+1;j<G.nb_nodes;j++)
      startx[i][j]=0;
	for (vector<list<pair<int,int> > >::const_iterator itsol=solution.begin();itsol!=solution.end();itsol++){
		list<pair<int,int> >::const_iterator pair;
		for(pair=itsol->begin();pair!=itsol->end() ;pair++) {
		  if (pair->first<pair->second)
		    startx[pair->first][pair->second]=1;
		  else
		    startx[pair->second][pair->first]=1;
		}
		
	}

 
  IloNumVarArray startVar(env);
  IloNumArray startVal(env);
  for (i=0;i<G.nb_nodes;i++)
    for (j=i+1;j<G.nb_nodes;j++) {
             startVar.add(x[i][j]);
             startVal.add(startx[i][j]); // startx is your heuristic values
         }
  cplex.addMIPStart(startVar, startVal, IloCplex::MIPStartCheckFeas);
  startVal.end();
	startVar.end();
  if ( !cplex.solve() ) {
    env.error() << "Failed to optimize LP" << endl;
    exit(1);
  }

 
  env.out() << "Solution status = " << cplex.getStatus() << endl;
  env.out() << "Solution value  = " << cplex.getObjValue() << endl;


  vector<pair<int,int> >   Lsol;
  for(i = 0; i < G.nb_nodes; i++)
     for (j=i+1;j<G.nb_nodes;j++)
			if (i!=j && cplex.getValue(x[i][j])>1-epsilon ){
				cout<<"chemin "<<i<<" "<<j<<endl;
	  		Lsol.push_back(make_pair(i,j));
	  	}

	  	



  //////////////
  //////  CPLEX's ENDING
  //////////////

  env.end();

  //////////////
  //////  OUTPUT
  //////////////


  vector<pair<int,int> >::const_iterator itp;
 
  ofstream ficsol(nameextsol.c_str());
  double best_length=0;
  for(itp = Lsol.begin(); itp!=Lsol.end();itp++) {
    best_length+=G.lengthTSP(itp->first,itp->second);
    ficsol<<itp->first<<" "<<itp->second<<endl;
  }
 
  ficsol.close();

  cout<<"Tour found of value : "<<best_length<<endl;
  
  cout<<endl;
  //affichage
  vector<int> nodes;
  nodes.resize(G.nb_nodes);
  for(int no=0;no<G.nb_nodes;no++) nodes[no]=1;
	vector<pair<int,int> >   Ls=Lsol;
	pair<int,int> start=Ls[0];
	pair<int,int> v;
	int u=start.second;
	Ls.erase(Ls.begin());
	cout<<start.first<<" ";
	while(Ls.size()>0){
		//cout<<u<<"  ";
		for (vector<pair<int,int> >::iterator it=Ls.begin(); it!=Ls.end();it++){
			if (it->first==u) {
				v=*it;
				cout<<u<<" ";
				u=v.second;
				Ls.erase(it);
				if (u==start.first && Ls.size()!=0){
					cout<<endl;
					start=Ls[0];
					
					Ls.erase(Ls.begin());
					
					cout<<start.first <<" ";
					u=start.second;
				}
				break;
			}
			if (it->second==u) {
				v=*it;
				cout<<u<<" ";
				u=v.first;
				Ls.erase(it);
				if (u==start.first && Ls.size()!=0){
					cout<<endl;
					start=Ls[0];
					Ls.erase(Ls.begin());
					
					cout<<start.first <<" ";
					u=start.second;
				}
				break;
			}
			
		
		}
		
	
	}

  return 0;
}
