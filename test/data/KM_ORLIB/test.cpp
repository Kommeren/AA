#include <iostream>
#include <vector>
#include <queue>
#include <algorithm> 
#include <map>
using namespace std;
vector<pair<int,int> >tab[1000];
priority_queue<pair<int,int> > kolejka;
pair<int,int> pom;
bool odwiedzone[1000];
vector<pair<int,int> >kra;
int wynik,m,n,k,a,b,d,j;
int main(){
  cin>>n>>m>>k;
  std::map<std::pair<int,int> ,int> mapa;
  for(int i=0;i<m;i++){
    cin>>a>>b>>d;
    if(a<b)
	  std::swap(a,b);
    //if(mapa[std::make_pair(a,b)]>i+1)
	//  LOG("error");
	if(mapa[std::make_pair(a,b)]==0){
	  
	tab[a].push_back(make_pair(b,d));
	tab[b].push_back(make_pair(a,d));
	mapa[std::make_pair(a,b)]=tab[a].size();
	mapa[std::make_pair(b,a)]=tab[b].size();
        //edges.push_back(Edge(a-1,b-1));
	kra.push_back(make_pair(min(a,b),max(a,b)));
	}else{
	  //edges[mapa[std::make_pair(a,b)]-1]=Edge(a-1,b-1);
	  //if(weight[mapa[std::make_pair(a,b)]-1]<l)
	  tab[a][(mapa[std::make_pair(a,b)]-1)]=make_pair(b,d);
	  tab[b][(mapa[std::make_pair(b,a)]-1)]=make_pair(a,d);
	  //LOG(a<<" "<<b);
	}
    
    
    
  }
  sort(kra.begin(),kra.end());
  for(int i=0;i<kra.size();i++){
    cout<<kra[i].first<<" "<<kra[i].second<<endl;
    if(i>0)
    if(kra[i-1].first==kra[i].first && kra[i-1].second==kra[i].second)
      cout<<"powturka!!!!!!!!!!!!!!!!"<<endl;
    
    
  }
  for(int i=0;i<k;i++){
    cin>>a;
    kolejka.push(make_pair(0,a+1));
  }
  while(!kolejka.empty()){
    pom=kolejka.top();
    kolejka.pop();
    if(!odwiedzone[pom.second]){
	cout<<pom.first<<endl;
	wynik-=pom.first;
	odwiedzone[pom.second]=true;
	for(int i=0;i<tab[pom.second].size();i++)
	  kolejka.push(make_pair(pom.first-tab[pom.second][i].second,tab[pom.second][i].first));
    }
    
  }
  cout<<wynik<<endl;

}