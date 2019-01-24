#include <ilcplex/ilocplex.h>
#include <vector>
#include"Graph.h"

#define epsilon 0.01
#define ulimit 0.3
#define llimit 0.1
#define tll 5
#define per 0
#define tope 10

using namespace::std;

void randomNodes(VRP_Graph & G, vector<int> & St){
	vector<int> startingNodes;
	for(int i=0;i<G.nb_nodes;i++) St[i]=0;
	int nbstart=0;
	
	while (nbstart<int(G.nb_nodes/2)){
		int node=rand()%(G.nb_nodes-nbstart-1)+1; 
		
		int it=0;
		
		for (int temp=0;temp<G.nb_nodes;temp++){
			if (it==node && St[temp]==0) {
				St[temp]=1;
				startingNodes.push_back(temp);
				nbstart+=1;
				break;
				}
			if (St[temp]==1){
				continue;
			}
			it++;
		}

	}
//	affichage
//	for(vector<int>::iterator it=startingNodes.begin();it!=startingNodes.end();it++) cout<<*it<<"  ";
//	cout<<endl;
//	cout<<endl;
//	for(vector<int>::iterator it=St.begin();it!=St.end();it++) cout<<*it<<"  ";
//	cout<<endl;
//	cout<<endl;

}

vector<int > computeCadd(VRP_Graph & G,float smax,vector<int > & S){
	vector<int > ret;
	int node;
	for (node=1;node<G.nb_nodes;node++){
		if (S[node]==0 && G.demand[node]<=smax){
			ret.push_back(node);
		}
	
	}
	return ret;
}

vector<int > computeCremove(VRP_Graph & G,float smin,vector<int > & S){
	vector<int > ret;
	int node;
	for (node=1;node<G.nb_nodes;node++){
		if (S[node]==1 && G.demand[node]<=smin){
			ret.push_back(node);
		}
	}
	return ret;
}

float computX (VRP_Graph & G,vector<int > St){
	float value=0;
	list<C_link *>::iterator ita;
	int i;
	for (i=0; i<St.size();i++){
		if (St[i]==0) continue;
		for(ita=G.V_nodes[i].L_adjLinks.begin();ita!=G.V_nodes[i].L_adjLinks.end();ita++){
			if (St[(*ita)->return_other_extrem(i)]==0){
				value+=(*ita)->algo_cost;
			}
		}
	}
   return value;
}

float computeStooX(VRP_Graph & G,vector<int > St,int c){
	list<C_link *>::iterator ita;
	float value=0;
	for(ita=G.V_nodes[c].L_adjLinks.begin();ita!=G.V_nodes[c].L_adjLinks.end();ita++){
		if (St[(*ita)->return_other_extrem(c)]==1 ){
			value+=(*ita)->algo_cost;

		}
	}
	return value;
	
}

int computeMaxCaddS(VRP_Graph & G,vector<int > S,vector<int > Cadd){
	float max=-1000;
	int node=0;

	vector<int > Stemp;
	for(vector<int >::iterator c=Cadd.begin();c!=Cadd.end();c++){
		Stemp=S;
		float value=computeStooX(G, Stemp,*c);

		if (value>max){
			max=value;
			node=*c;

		}
	}
	return node;
}

int computeMaxCremoveS(VRP_Graph & G,vector<int > S,vector<int > Cremove){
	float max=-1000;
	int node=0;
	int i;
	vector<int > Stemp;
	for(vector<int >::iterator c=Cremove.begin();c!=Cremove.end();c++){
		Stemp=S;
		for(int t=0;t<Stemp.size();t++){
			if (Stemp[t]==0) Stemp[t]=1;
			else  Stemp[t]=0;
		}
		float value=computeStooX(G, Stemp,*c);
		if (value>max){
			max=value;
			node=*c;
		}
	}
	return node;
}



int sumDemand(VRP_Graph & G,vector<int > & client){
	int sum=0;
	int clientInd;
	for (clientInd=0;clientInd<client.size();clientInd++){
		if(client[clientInd]==1) sum+=G.demand[clientInd];
	}
	return sum;
}

void checkEq(IloEnv env,VRP_Graph & G,vector<int> & S,vector<IloRange > & ViolatedCst,vector<vector<IloNumVar> >& x){
	float demand=sumDemand(G,S);
	float b=demand/float(G.maxCap);
	b=ceil(b);
	float tot =computX ( G, S);
	//cout<<tot<<" avant"<<2*b<<endl;
	if (tot+epsilon<2*b){
		//cout<<tot<<" "<<2*b<<endl;
		//add constraint
		int ind;
		IloExpr expr(env);
		for (ind=0;ind<S.size();ind++){
			for (int ind2=0;ind2<S.size();ind2++){
				if (S[ind]==1 && S[ind2]==0){
					if (ind < ind2) expr+=x[ind][ind2];
					else{
						expr+=x[ind2][ind];
					}
				}
			}
		}
	float value=0;
	list<C_link *>::iterator ita;
	int i;
	for (i=0; i<S.size();i++){
		if (S[i]==0) continue;
		for(ita=G.V_nodes[i].L_adjLinks.begin();ita!=G.V_nodes[i].L_adjLinks.end();ita++){
			if (S[(*ita)->return_other_extrem(i)]==0){
				value+=(*ita)->algo_cost;
			}
		}
	}
	//cout<<value<<endl;
		//cout<<"Houra"<<endl;
		ViolatedCst.push_back(IloRange(expr >= 2*b));
	}
	
}
void interchange(IloEnv env,int p, VRP_Graph & G,vector<int> & S,vector<IloRange > & ViolatedCst,vector<vector<IloNumVar> >& x){
	int iter=1;
	vector<pair<int,pair<int,int > > > tabu;//node (forbidden add=1,remove=0) number of iteration remaining.
	while(iter<tope){
		int demand=sumDemand( G, S);
		float smax=G.maxCap*(p+ulimit)-demand;
		float smin=demand-G.maxCap*(p-llimit);
		vector<int> cAdd=computeCadd(G,smax,S);
		vector<int> cRemove=computeCremove(G,smin,S);
		vector<int> cAddtabu;
		vector<int> cRemovetabu;
//				cout<<"S "<<S.size()<<endl;
//		for (int o =0;o<S.size();o++){
//				cout<<" "<<S[o];
//				}
//				cout<<endl;
//		cout<<"cremove size "<<cRemove.size()<<" "<<smin<<endl;
//		for (int u =0;u<cRemove.size();u++){
//				cout<<" "<<cRemove[u];
//				}
//				cout<<endl;
//				cout<<"cadd size "<<cAdd.size()<<" "<<smax<<endl;
//		for (int r =0;r<cAdd.size();r++){
//				cout<<" "<<cAdd[r];
//				}
//				cout<<endl;
//		cout<<"tabu "<<tabu.size()<<endl;
//		for (int g =0;g<tabu.size();g++){
//				cout<<" "<<tabu[g].first<<"  "<<tabu[g].second.first;
//				}
//				cout<<endl;
//				

		for(int j=0;j<cAdd.size();j++){
				bool pres=false;
			for(int i=0;i<tabu.size();i++){
				if (cAdd[j]==tabu[i].first && tabu[i].second.first==1){
					pres=true ;
					break;
				}
			}
			if (pres==false) cAddtabu.push_back(cAdd[j]);
		}
		//cout << "hehe"<<endl;
		for(int z=0;z<cRemove.size();z++){
				bool pres=false;
			for(int i=0;i<tabu.size();i++){
				if (cRemove[z]==tabu[i].first && tabu[i].second.first==0){
					pres=true ;
					break;
				}
			}
			if (pres==false) cRemovetabu.push_back(cRemove[z]);
		}
			
		//cout << "hehe2"<<endl;
		if (cAddtabu.size()==0 && cRemovetabu.size()==0) return;
		
		int select=computeMaxCaddS( G, S, cAddtabu);
		int select2=computeMaxCremoveS( G, S, cRemovetabu);
//		cout<<"select "<<select<<"  "<<select2;
		vector<int > S1=S;
		S1[select]=1;
		vector<int > S2=S;
		S2[select2]=0;
		
//		cout<<"cremovetabu size "<<cRemovetabu.size()<<" "<<smin<<endl;
//		for (int n =0;n<cRemovetabu.size();n++){
//				cout<<" "<<cRemovetabu[n];
//				}
//				cout<<endl;
//				cout<<"caddtabu size "<<cAddtabu.size()<<" "<<smax<<endl;
//		for (int b =0;b<cAddtabu.size();b++){
//				cout<<" "<<cAddtabu[b];
//				}
//				cout<<"a";
		if (cAddtabu.size()==0 || cRemovetabu.size()==0){
		
			if (cAddtabu.size()==0){
				S[select2]=0;
				pair<int,int> a =make_pair(1,tll+1);
				tabu.push_back(make_pair(select2,a));
				//cout<<"yyyyyyyyyyyyyyyyyy"<<endl;
				//cout << select2<<endl;
			}
			if (cRemovetabu.size()==0){
				S[select]=1;
				pair<int,int> a =make_pair(0,tll+1);
				tabu.push_back(make_pair(select,a));
//				cout<<"zzzzzzzzzzzzzzzzzzzzzzzzzz"<<endl;
//				cout << select<<endl;
			
			}
		
		}
		else{
			if (computX ( G, S1)>computX ( G, S2)){
				S[select]=1;
				pair<int,int> a =make_pair(0,tll+1);
				tabu.push_back(make_pair(select,a));
//				cout<<"zzzzzzzzzzzzzzzzzzzzzzzzzz"<<endl;
//				cout << select<<endl;
			}
			else{
				S[select2]=0;
				pair<int,int> a =make_pair(1,tll+1);
				tabu.push_back(make_pair(select2,a));
//				cout<<"yyyyyyyyyyyyyyyyyy"<<endl;
//				cout << select2<<endl;
			}
		}
//		cout<<"b";
		checkEq(env,G,S,ViolatedCst,x);
		
		for(int i=0;i<tabu.size();i++){
			tabu[i].second.second-=1;
			
		}
		bool bo=true;
		while(bo && tabu.size()>0){
			if ( tabu[0].second.second==0){
//				cout<< tabu.size();
				tabu.erase(tabu.begin());
			}
			else{
				bo=false;
			}
		}
//		cout<<"c";
		iter+=1;

	}
}


//Find a violated  Cut by a separation algorithm
bool  find_ViolatedMengerCutCst(IloEnv env, VRP_Graph & G,  vector<vector<IloNumVar> >& x,  vector<IloRange > & ViolatedCst){
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
	
	if (test+epsilon<2*b){

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

		
    
  ViolatedCst.push_back(IloRange(expr >= 2*b));
  //return true;
	}
	//return false;

	vector<int> St;
	St.resize(G.nb_nodes);
	randomNodes(G,St);
	int z;
	for(z=0;z<St.size();z++){
		if (St[z]==0) continue;
		vector<int> S;
		S.resize(G.nb_nodes);
		int d;
		for(d=0;d<S.size();d++) S[d]=0;
		S[z]=1;
		
		
		int p=1;
		for (p=1;p<G.nbTruck;p++){
		
			
			float smax=G.maxCap*(p+ulimit)-sumDemand( G, S);
			vector<int> cAdd=computeCadd(G,smax,S);
			


			while(cAdd.size()!=0){
				int select=computeMaxCaddS( G, S, cAdd);
				
				//cout <<"noeud choisi "<<select<<endl;
				
				S[select]=1;
//							for (int u =0;u<S.size();u++){
//				cout<<" "<<S[u];
//				}
//				cout<<endl;
				
				checkEq( env,G,S,ViolatedCst,x);

				float smax=G.maxCap*(p+ulimit)-sumDemand( G, S);
				cAdd.clear();
				cAdd=computeCadd(G,smax,S);
				
//							for (int y =0;y<cAdd.size();y++){
//			cout<<" "<<cAdd[y];
//			}
			//cout<<" "<<cAdd.size()<<"  "<<smax<<" "<<sumDemand( G, S)<<" "<<G.maxCap*(p+ulimit)<<endl;
			}
			interchange(env,p,G,S,ViolatedCst,x);

		}
	}
	
	if (ViolatedCst.size()==0) return false;

	return true;

}
