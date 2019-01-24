#include <iostream>
#include <iomanip>
#include <cstring>
#include <sstream>
#include<ctime>

#include"Graph.h"
#include "Heuristic_TSPfunc.h"

using namespace std;

bool wayToSort(pair<int,int> i, pair<int,int> j) { return i.first > j.first; }


void First_affectation(VRP_Graph &G, vector<vector<pair<int,int> > >& sol){
	int i;
	sol.clear();
	sol.resize(G.nbTruck);
	vector<int> totalDemand(G.nbTruck);
	for(i=0;i<G.nbTruck;i++){
		totalDemand[i]=0;
	}

	vector<pair<int,int> > demandSorted;
	for(i=1;i<G.demand.size();i++){//1 pour enlever le depot
		demandSorted.push_back(make_pair(G.demand[i],i));
		
	}
	
	sort(demandSorted.begin(), demandSorted.end(), wayToSort);
	for(i=0;i<demandSorted.size();i++){
		cout<<demandSorted[i].first<<" "<<demandSorted[i].second<<endl;
		
	}
//// affectation des Demande aux camions
	vector<pair<int,int> >:: iterator it;
	for (it=demandSorted.begin();it!=demandSorted.end();it++){
		
		int indMinDemand=0;
		for(i=1;i<totalDemand.size();i++){
			if(totalDemand[i]<totalDemand[indMinDemand]){
				indMinDemand=i;
			}
		}
		if (totalDemand[indMinDemand]+it->first>G.maxCap){
			totalDemand.push_back(it->first);
			vector<pair<int,int> > tempo;
			tempo.push_back(*it);
			sol.push_back(tempo);
		}
		else{
				totalDemand[indMinDemand]+=it->first;
				sol[indMinDemand].push_back(*it);
		}
	}
	

}

void Greed_TwoOPT(VRP_Graph &G,C_Graph *Gtsp, vector<vector<pair<int,int> > >& sol,int NB_ITER,int &LocalSol,list<pair<int,int> >& best_sol){
	vector<int> curr_sol;
	curr_sol.resize(Gtsp->nb_nodes);
	int i,k;
	greedy_heuristic(*Gtsp,curr_sol);
	//for (int i=0;i<curr_sol.size();i++){
		//cout<<curr_sol[i]<<endl;
	//}
	double curr_length=0;
	best_sol.clear();
	
	for(k=1; k<Gtsp->nb_nodes;k++) {
			curr_length+=Gtsp->lengthTSP(curr_sol[k-1],curr_sol[k]);
			best_sol.push_back(make_pair(Gtsp->V_nodes[curr_sol[k-1]].num,Gtsp->V_nodes[curr_sol[k]].num));
	}
	curr_length+=Gtsp->lengthTSP(curr_sol[Gtsp->nb_nodes-1],curr_sol[0]);
	best_sol.push_back(make_pair(Gtsp->V_nodes[curr_sol[Gtsp->nb_nodes-1]].num,Gtsp->V_nodes[curr_sol[0]].num));
	double best_length=curr_length;
	//cout<<"Greedy Tour of value : "<<best_length<<endl;
	TwoOptDescent(*Gtsp, curr_length, curr_sol, best_length, best_sol,NB_ITER);
	//cout<<"Best tour value after "<<NB_ITER<<" 2-OPT: "<<best_length<<endl;
	LocalSol=best_length;

}


void Create_TSPGraph(VRP_Graph &G, vector<vector<pair<int,int> > >& sol,int NB_ITER,int &TotalSol,vector< list<pair<int,int> > > &solution){
	C_link *a;
	C_Graph *Gtsp;
	TotalSol=0;
	list<pair<int,int> > best_sol;
	vector<vector<pair<int,int> > > ::iterator it;
	for (it=sol.begin();it!=sol.end();it++){
		Gtsp= new C_Graph;
		Gtsp->directed=false;
		Gtsp->nb_nodes=it->size()+1;
		Gtsp->V_nodes.resize(it->size()+1);
		Gtsp->V_links.resize(((it->size()+1)*(it->size()))/2);
		vector<pair<int,int> >::iterator it2;
		vector<pair<int,int> >::iterator it3;
		int k=0;
		int cptnodes=0,cptnodes2=0;
		
		//on crée les noeuds
		for (it2=it->begin();it2!=it->end();it2++){
			Gtsp->V_nodes[cptnodes].num=it2->second;
			Gtsp->V_nodes[cptnodes].x=G.V_nodes[it2->second].x;
			Gtsp->V_nodes[cptnodes].y=G.V_nodes[it2->second].y;
			cptnodes++;
		}
		//on rajoute le noeud depot.
		Gtsp->V_nodes[cptnodes].num=0;
		Gtsp->V_nodes[cptnodes].x=G.V_nodes[0].x;
		Gtsp->V_nodes[cptnodes].y=G.V_nodes[0].y;
		//on crée les transitions
		cptnodes=0;

		
		for (it2=it->begin();it2!=it->end();it2++){

			int cptnodes2=1;
			
			for(it3=it2+1;it3!=it->end();it3++){
				a=new C_link;
				a->num=k;
				a->v1=cptnodes;
				a->v2=cptnodes+cptnodes2;
				a->length=G.lengthTSP(it2->second,it3->second);
				//cout<<"distance:"<<a->length<<" noeud " << Gtsp->V_nodes[cptnodes].num<<" " << Gtsp->V_nodes[cptnodes].x<<" "<<Gtsp->V_nodes[cptnodes].y<< " w noeud "<<Gtsp->V_nodes[cptnodes+cptnodes2].num<<" "<< Gtsp->V_nodes[cptnodes+cptnodes2].x<<" "<< Gtsp->V_nodes[cptnodes+cptnodes2].y<<endl;
				
				Gtsp->V_nodes[cptnodes].L_adjLinks.push_back(a);
				Gtsp->V_nodes[cptnodes+cptnodes2].L_adjLinks.push_back(a);
				Gtsp->V_links[k] = a;
				k++;
				cptnodes2++;
			
			
			}
			cptnodes++;
		}
		
				//transition vers depot 
		for (int i=0;i<it->size();i++){
			a=new C_link;
			a->num=k;
			a->v1=i;
			a->v2=it->size();

			a->length=G.lengthTSP(i,0);
			
			Gtsp->V_nodes[i].L_adjLinks.push_back(a);
			Gtsp->V_nodes[it->size()].L_adjLinks.push_back(a);
			Gtsp->V_links[k] = a;
			k++;
		}
		
		
		//Ici on a un graph TSP, on le passe a une fonction pour calculer son fitness 
		int LocalSol=0;
		

		Greed_TwoOPT(G,Gtsp,sol,NB_ITER,LocalSol,best_sol);
		TotalSol+=LocalSol;
		solution.push_back(best_sol);
	}
	

	//cout<<"Solution trouvée de valeur :"<< TotalSol<<endl;
}

int sumDemand_Truck(vector<pair<int,int> >& truck){
	vector<pair<int,int> >::iterator it;
	int sum=0;
	for (it=truck.begin();it!=truck.end();it++){
		sum+=it->first;
	}
	return sum;
}

void Change_affectation(VRP_Graph &G, vector<vector<pair<int,int> > >& sol){
	//ici sol est déja une affectation des clients aux tournées.
	int f_truck,s_truck;
	f_truck= rand()%sol.size();//on choisit deux tournée à modifier au hasard
	s_truck= rand()%sol.size();
	//cout<<f_truck <<s_truck<<endl;
	if (G.nbTruck<=1) return ;
	
	while (s_truck==f_truck ){
		s_truck= rand()%sol.size();
	}
	int ind_Client=rand()%sol[f_truck].size();
	int chang=1,j;// egal a 1 tant qu'un changement n'a pas été fait'
	vector<int>ind(sol[s_truck].size());
	for (j=0;j<sol[s_truck].size();j++){
		ind[j]=j;
	}
	while(chang!=0 && ind.size()>0){
		int indRand=rand()%ind.size();
		int ind_Client2=ind[indRand];
		ind.erase(ind.begin()+indRand);
		if(sumDemand_Truck(sol[f_truck])-sol[f_truck][ind_Client].first+ sol[s_truck][ind_Client2].first<=G.maxCap && sumDemand_Truck(sol[s_truck])-sol[s_truck][ind_Client2].first+ sol[f_truck][ind_Client].first<=G.maxCap){
			chang=0;
			pair<int,int>  tempo=sol[f_truck][ind_Client];
			pair<int,int>  tempo2=sol[s_truck][ind_Client2];
			sol[f_truck].erase(sol[f_truck].begin()+ind_Client);
			sol[s_truck].erase(sol[s_truck].begin()+ind_Client2);
			sol[f_truck].push_back(tempo2);
			sol[s_truck].push_back(tempo);
		}
		
		
	}
	
	
}



void Reduce_T(VRP_Graph &G, vector<vector<pair<int,int> > >& sol,int NB_TRY){
	//Ici on essaye de réduire le nombre de tournée si il est supérieur au nombre disponible nbTrucks.
	
}
void GeneticAlgorithm(VRP_Graph &G,vector<vector<pair<int,int> > >& curr_sol,vector<list<pair<int,int> > > & solution,int NB_ITER){

	vector<vector<pair<int,int> > >:: iterator it;
	First_affectation(G,curr_sol);
	int TotalSol=0;
	Create_TSPGraph(G,curr_sol,NB_ITER,TotalSol,solution);

}
void LocalResearch(VRP_Graph &G,vector<vector<pair<int,int> > >& curr_sol,vector<list<pair<int,int> > > & solution,int NB_ITER){
	int i,j,k;
	vector<vector<pair<int,int> > >:: iterator it;
	First_affectation(G,curr_sol);
	int TotalSol=0;
	vector<list<pair<int,int> > > temp_solution;
	temp_solution.resize(G.nbTruck);
	vector<list<pair<int,int> > > temp2_solution;
	temp2_solution.resize(G.nbTruck);
	Create_TSPGraph(G,curr_sol,NB_ITER,TotalSol,solution);
	vector<list<pair<int,int > > >:: const_iterator it5;
	for (it5=solution.begin();it5!=solution.end();it5++){
		list<pair<int,int > >:: const_iterator it3;
		for(it3=it5->begin();it3!=it5->end();it3++){
		  cout<< it3->first<<" with "<<it3->second<<", ";
			
		}
		cout<<endl;
	}
	vector<vector<pair<int,int> > > currBest_sol=curr_sol;
	int NB_GEN=80; // le nombre de generation
	int NB_IND=100; // le nombre d'individu par génération
	int bestSolFath=TotalSol,bestSolSon=TotalSol,indGen=0;
	
	
		while(bestSolSon<=bestSolFath && indGen<NB_GEN){
		int sol=0;

		
		bestSolFath=bestSolSon;
		vector<vector<pair<int,int> > > current_sol= currBest_sol;
		for (j=0;j<NB_IND;j++){
			vector<vector<pair<int,int> > > current_sol2= current_sol;

			Change_affectation(G,current_sol2);
			temp_solution.clear();
			Create_TSPGraph(G,current_sol2,NB_ITER,sol,temp_solution);

			if( bestSolSon>sol){
				bestSolSon=sol;
				currBest_sol=current_sol2;
				solution=temp_solution;
			}

		
		}


		indGen+=1;
	
	}
	

	curr_sol=currBest_sol;
	
	int cptCamion=0;
	for (it=curr_sol.begin();it!=curr_sol.end();it++){
		cout<<"Camion "<<cptCamion<<endl;
		vector<pair<int,int > >:: iterator it2;
		for(it2=it->begin();it2!=it->end();it2++){
		  cout<< it2->first<<" client "<<it2->second<<", ";
			
		}
		cptCamion++;
		cout<<endl;
	}
	cout<<solution.size()<<endl;
	vector<list<pair<int,int > > >:: const_iterator it4;
	for (it4=solution.begin();it4!=solution.end();it4++){
		list<pair<int,int > >:: const_iterator it3;
		for(it3=it4->begin();it3!=it4->end();it3++){
		  cout<< it3->first<<" with "<<it3->second<<", ";
			
		}
		cout<<endl;
	}

	//Create_TSPGraph(G,currBest_sol,NB_ITER,TotalSol,solution);
	//cout<<TotalSol<<endl;
	  float Tot=0;

	for (it4=solution.begin();it4!=solution.end();it4++){
		list<pair<int,int > >:: const_iterator it3;
		for(it3=it4->begin();it3!=it4->end();it3++){
			Tot+=G.lengthTSP(it3->first,it3->second);
				  cout<< it3->first<<" with "<<it3->second<<", ";
		}
		cout<<endl;
	}

	cout<<"Toltal somme=" << Tot<<endl;
	
	}


//int main(int argc, char**argv){
//	string name, nameext, nameextsol;
//  int i,j,k,NB_ITER;
//  
//  clock_t time_initial, /* Time initial en micro-secondes */
//          time_final;   /* Time final en micro-secondes */
//  float   time_cpu;     /* Time total en secondes */
//	srand(time(0));
//  //////////////
//  //////  DATA
//  //////////////
//	
//  if(argc!=3){
//    cerr<<"usage: "<<argv[0]<<" <vrp file name (without .vrp)> <nb_iteration>"<<endl;
//    cerr<<"       0: only greedy / nb>0: greedy+TwoOptDescent on nb iterations"<<endl;
//    return 1;
//  }
//  name=argv[1];
//  nameext=name+".vrp";
//  nameextsol=name+".sol2";
//  NB_ITER=atoi(argv[2]);

//  
//  ifstream fic(nameext.c_str());

//  if (fic==NULL){
//    cerr<<"file "<<nameext<<" not found"<<endl;
//    return 1;
//  }

//  VRP_Graph G;

//  G.read_undirected_VRP(fic);

//  fic.close();

//  //////////////
//  //////  Glouton HEURISTIC
//  //////////////
//  time_initial = clock ();

//  vector<vector<pair<int,int> > > curr_sol;
//  curr_sol.resize(G.nbTruck);
//  vector<list<pair<int,int> > >solution;

//  LocalResearch(G,curr_sol,solution,NB_ITER);
  
//  First_affectation(G,curr_sol);

//	cout<<"L'affectation est la suivante : "<<endl;

//	vector<vector<pair<int,int> > >:: iterator it;
//	int cptCamion=0;
//	for (it=curr_sol.begin();it!=curr_sol.end();it++){
//		cout<<"Camion "<<cptCamion<<endl;
//		vector<pair<int,int > >:: iterator it2;
//		for(it2=it->begin();it2!=it->end();it2++){
//		  cout<< it2->first<<" client "<<it2->second<<", ";
//		}
//		cptCamion++;
//		cout<<endl;
//	}
//	int TotalSol=0;
//	Create_TSPGraph(G,curr_sol,NB_ITER,TotalSol);
//	
//	
//	cout<< TotalSol<<endl;
//	
//	
//	
//	//Permet de voir si Change_affectation marche
////	Change_affectation(G,curr_sol);
////	cptCamion=0;
////	for (it=curr_sol.begin();it!=curr_sol.end();it++){
////	cout<<"Camion "<<cptCamion<<endl;
////	vector<pair<int,int > >:: iterator it2;
////	for(it2=it->begin();it2!=it->end();it2++){
////	  cout<< it2->first<<" client "<<it2->second<<", ";
////	}
////	cptCamion++;
////	cout<<endl;
////	}
//	
//	
//	
//	int NB_GEN=50; // le nombre de generation
//	int NB_IND=100; // le nombre d'individu par génération
//	int bestSolFath=TotalSol,bestSolSon=TotalSol,indGen=0;

//	vector<vector<pair<int,int> > > currBest_sol=curr_sol;

//	vector<pair<int,vector<vector<pair<int,int> > > > > *tableauInd;
//	pair<int,vector<vector<pair<int,int> > > >  ind=make_pair(bestSolFath,currBest_sol);

//	//tableauInd->push_back(ind);

//	while(bestSolSon<=bestSolFath && indGen<NB_GEN){
//		int sol=0;
//		
//		bestSolFath=bestSolSon;
//		vector<vector<pair<int,int> > > current_sol= currBest_sol;
//		for (j=0;j<NB_IND;j++){
//			vector<vector<pair<int,int> > > current_sol2= current_sol;

//			Change_affectation(G,current_sol2);

//			Create_TSPGraph(G,current_sol2,NB_ITER,sol);

//			if( bestSolSon>sol){
//				bestSolSon=sol;
//				currBest_sol=current_sol2;
//				
//				
//			}
//			
//		}
//		cout<<bestSolSon <<" "<<bestSolFath<<endl;
//		indGen+=1;
//	
//	}
//	
//	cptCamion=0;
//	for (it=currBest_sol.begin();it!=currBest_sol.end();it++){
//		cout<<"Camion "<<cptCamion<<endl;
//		vector<pair<int,int > >:: iterator it2;
//		for(it2=it->begin()+1;it2!=it->end();it2++){
//		  cout<< it2->first<<" client "<<it2->second<<", ";
//			
//		}
//		cptCamion++;
//		cout<<endl;
//	}
//	Create_TSPGraph(G,currBest_sol,NB_ITER,TotalSol);
//	cout<<TotalSol<<endl;
//	





//La solution trouvé peut être différente de celle avant car 2 OPt varie aléatoirement
//	return 0;
//}
