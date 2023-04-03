#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <map>
#include <queue>
#include <utility>


#define F(i,l) for( i = 0; i < l; i++)
#define Fi(i,s,l) for( i = s; i < j; i++)
#define SF(sit, st) for( sit = st.begin(); sit != st.end(); sit++)
#define MF(mit, mp) for( mit = mp.begin(); mit != mp.end(); mit++)
#define EPNT(s) fprintf(stderr,"%s\n",s)
#define pmsg(s) printf("%s\n",s)

#define SZ 256
#define MX 6000000

const int PNT = 0;

using namespace std;




class node
{
	public:
		vector<int>  adj;
		vector<vector<int> > sp;
		int v;
		int num;
		int Ival;
		multimap<int ,node *> kids;
		float fval;
		double dval;
		long lval;
		char cval;
		string name;
		class node * parent;
		node(int num);
		node(int num, int Nnodes);
		void ADDCHILD(int id, int dis, node * kp);
}*NODE;


void node::ADDCHILD(int id, int dis, node * kp)
{
	map<int, node *>::iterator mit;
	map<int, node *>::iterator kmit;

	adj[id] = dis;

		mit = kids.begin();

		while( mit != kids.end())
		{
			if( (*mit).second == kp)
			{
				kids.erase(mit);
				break;
			}

			mit++;
		}

		printf("adding dis %d to node %d toward %d \n", dis, num, kp->num);

		kids.insert(make_pair(dis,kp));
}


node::node(int number)
{
	num = number;
	v = 0;
	adj.clear();
}

node::node(int number, int Nnodes)
{
	num = number;
	v = 0;
	adj.clear();
	adj.resize(Nnodes,MX);

	sp.resize(Nnodes);

	for(int i = 0; i < Nnodes; i++)
	{
		sp.at(i).resize(0);
	}

}





class graph
{
	public:
		vector<vector<int> > gph;
		vector<vector<int> > Sgph;
		vector<vector<int> > Lgph;
		vector<vector<int> > A;
		vector<vector<long> > AC;
		set<node * > G;
		int nodecnt;
		int max;
		int min;
		int source;
		int sink;
		int root;
		set<int> VS;
		set<int> US;
		void printGAry(int t);
		void printGSet();
		graph(int size,int initval);
		graph(vector<int> f, vector<int> t, vector<int> wt, int N, int type);
		void SetNodes(int &i, int &j, int &wt);
		node * GETNODE(int num);
		void MakeA();
		vector<vector<long> >  MakeAchgs( int chgs);
		void ResetG();
		int GDIKSTRA(int &sn, queue<int> &Q, int &dist);
		void MINDIKSTRA();
		queue<int> GBFS( queue<int> &Q, int from,queue<int> D, int sk);
		vector <int> BFSFIND(int sk);
		int MXST();
}*GRAPH;

void graph::ResetG()
{
	set<node *>::iterator sit;

	SF(sit,G)
	{
		(*sit)->v = 0;
	}
}


vector<vector<long> >  graph::MakeAchgs( int chgs)
{

	int i, j, k, f, z, t, num = Sgph.size();

	vector<vector<long> > ACC(num), Ai(num);
	vector<vector<long> > AC1(num), AC8, AC16, AC32;

	vector<vector<long> > AMX(num);
	vector< vector<vector<long> > > all;
	all.resize(7);

	AC.resize(A.size());


	F(i,num)
	{
		AMX[i].resize(A[i].size(),MX);
		ACC[i].resize(A[i].size());
		AC[i].resize(A[i].size());
		AC1[i].resize(A[i].size());
	}



	for(i = 0; i < A.size(); i++)
	{
		for(j = 0; j < A[i].size(); j++)
		{
			AC[i][j] = (long)A[i][j];
			ACC[i][j] = (long)A[i][j];
			AC1[i][j] = (long)A[i][j];
		}
	}


	int sofar, next, l, cnt = 1;;
	int fr = 4, h = 128, eight = 8, K = 1024, OK = 131072, OM = 1048576, HM = 524288;



	int a = 1, b = 1, limit = chgs; ;

	Ai = AMX;

	for(sofar = 2; sofar <= limit; sofar = next)
	{
		Ai = AMX;

		F(f,Sgph.size() )
		{
			F(t,Sgph.size())
			{
				F(z,Sgph.size())
				{
					if(1)
					{
						long fz = AC[f][z];
						long zt = ACC[z][t];

						if(Ai[f][t] > fz + zt)
						{
							Ai[f][t] = fz + zt;
						}
					}
				}
			}

		}

		if( sofar == OM)
		{
			all[6] = Ai;;
		}
		else if( sofar == HM)
		{
			all[5] = Ai;;

		}
		else if( sofar ==  OK)
		{
			all[4] = Ai;;

		}
		else if( sofar == K)
		{
			all[3] = Ai;;

		}
		else if( sofar == h)
		{
			all[2] = Ai;;

		}
		else if( sofar == eight)
		{
			all[1] = Ai;;

		}
		else if( sofar == fr)
		{
			all[0] = Ai;;

		}


		if( sofar * 2 < limit)
		{
			next = sofar*2;
			AC = Ai;
			ACC = Ai;
			a = sofar;
			b = sofar;

		}
		else
		{
			ACC = Ai;
			b = sofar;

			if( chgs - sofar >= OM)
			{
				AC = all[6];
				next = sofar + OM;
				a = OM;
			}
			else if( chgs - sofar >= HM)
			{
				AC = all[5];
				next = sofar + HM;
				a = HM;

			}
			else if( chgs - sofar >=  OK)
			{
				AC = all[4];
				next = sofar + OK;
				a = OK;

			}
			else if( chgs - sofar >= K)
			{
				AC = all[3];
				next = sofar + K;
				a = K;

			}
			else if( chgs - sofar >= h)
			{
				AC = all[2];
				next = sofar + h;
				a = h;

			}
			else if( chgs - sofar >= eight)
			{
				AC = all[1];
				next = sofar + eight;;
				a = eight;

			}
			else if( chgs - sofar >= fr)
			{
				AC = all[0];
				next = sofar + fr;
				a = fr;

			}
			else
			{
				AC = AC1;
				next = sofar+1;;
				a = 1;;
			}
		}
	}


	return Ai;
}

void graph::MakeA()
{
	int i, j, k, f, t;

	A.resize(Sgph.size());


	F(i,Sgph.size() )
	{
		A[i].resize(Sgph[i].size(),MX);
	}

	int fi, ij, jt ,ft, min = -MX;;

	F(f, Sgph.size())
	{

		F(t,Sgph.size())
		{
			min = MX;

			F(i,Sgph.size())
			{

				F(j, Sgph.size())
				{
					if(1)
					{
						fi = Sgph[f][i];
						ij = Lgph[i][j];
						jt = Sgph[j][t];

						if(PNT)	printf("f %d, t %d,=%d  i %d,  j %d, = %d\n", f, t, A[f][t],i, j , ij);
						if(PNT)	printf("fi %d, ij %d, jt %d, total %d\n",fi , -ij, jt, fi-ij+jt);


						if( min > fi-ij+jt)
						{
							A[f][t] = fi-ij+jt;
							min = fi-ij+jt;
						}
					}
				}
			}

		}
	}

	return ;
}


void graph::printGAry(int t)
{
	int i = 0, j = 0;;


	vector<vector<int> > gh;

	if(t == 0) gh = gph;
	else if(t == 1) gh = Sgph;
	else if(t == 2) gh = Lgph;
	else gh = A;

	if(t == 0) printf("printing Gph\n");
	else if(t == 1) printf("printing SGph\n");
	else if(t == 2) printf("printing LGph\n");
	else printf("printing A\n");

	F( i, gh.size())
	{
		printf("NODE %d:\n",i+1);
		F(j, gh[i].size())
		{
			printf("%d ",gh[i][j]);
		}
		printf("\n");
	}
}

void graph::printGSet()
{

	int i = 0, j, k;
	node * np;

	set<node *>::iterator sit;
	map<int, node * >::iterator mit;

	for(sit = G.begin() ; sit != G.end(); sit++)
	{
		np = (*sit);
		printf("Node: %d:\n", np->num);

		for(mit = np->kids.begin(); mit != np->kids.end(); mit++)
		{
			printf("child %d:--- %d--->NODE %d\n",i, mit->first,mit->second->num);
		}

	}
}


graph::graph(vector<int> f, vector<int> t, vector<int> w, int N, int type)
{
	int i = 0, j = 0;

	gph.resize(N);
	Sgph.resize(N);
	Lgph.resize(N);

	F(i,N)
	{
		gph[i].resize(N,0);
		Sgph[i].resize(N,MX);
		Lgph[i].resize(N,-MX);
	}

	node * nd;


	F(i,N)
	{
		F(j,N)
		{
			if(i == j)
			{
				gph[i][j] = 0;
				Sgph[i][j] = 0;
				Lgph[i][j] = 0;
			}
		}
	}


	set<node *>::iterator sit;
	set<int>::iterator isit;

	set<int> sh;

	F(i,f.size())
	{
		if( sh.find(f[i]) == sh.end())
		{
			sh.insert(f[i]);
			nd = new node(f[i],N);
			G.insert(nd);
		}

		if( sh.find(t[i]) == sh.end())
		{
			sh.insert(t[i]);
			nd = new node(t[i],N);
			G.insert(nd);
		}
	}

	F(i,f.size())
	{
		int fm, to, wt;
		fm = f[i];
		to = t[i];
		wt = w[i];

		node * fr, * tw;


		sit = G.begin();
		fr = (*sit);

		while( sit != G.end()&& fr->num != fm)
		{
			sit++;
			fr = (*sit);
		}

		sit = G.begin();
		tw = (*sit);
		while(sit != G.end() && tw->num != to)
		{
			sit++;
			tw = (*sit);
		}

		fr->kids.insert(make_pair(wt,tw));

		gph[fm-1][to-1] = wt;
		if(Sgph[fm-1][to-1] > wt) Sgph[fm-1][to-1] = wt;
		if(Lgph[fm-1][to-1] < wt ) Lgph[fm-1][to-1] = wt;


		if(type == 1)
		{
			tw->kids.insert(make_pair(wt,fr));
			if(gph[to-1][fm-1] < wt) gph[to-1][fm-1] = wt;
			if(Sgph[to-1][fm-1] > wt) Sgph[to-1][fm-1] = wt;
			if(Lgph[to-1][fm-1] < wt) Lgph[to-1][fm-1] = wt;
		}

	}





}


graph::graph(int size, int initval)
{
	int i = 0;

	if(size <= 0)
	{
		EPNT("Bad size given to graph initializer\n");
		exit(0);
	}

	gph.resize(size);

	F(i,size)
	{
		gph[i].resize(size,initval);
	}
}

void graph::SetNodes(int &i, int &j, int &wt)
{
	gph[i][j] = wt;
	return;
}

node * graph::GETNODE(int id)
{
	set<node*>::iterator sit;

	for(sit = G.begin(); sit != G.end();sit++)
	{
		node * np = (*sit);

		if( np->num == id) return np;

	}
	return NULL;
}



void makegraph(vector<int> &f, vector<int> &t, vector<int> &w, graph &gph)
{
	int i, j, z = 0;;

	F(i, f.size())
	{
		if(f[i] == t[i]) gph.SetNodes(f[i],f[i],z);
		else gph.SetNodes( f[i] , t[i], w[i]);
	}

	return;
}


int graph::GDIKSTRA(int &sn, queue<int> &Q, int &DIST)
{

	if(Q.empty()) return -1;

	int cn = Q.front();
	int dist = DIST;
	Q.pop();

	if(PNT) printf("-----------processing node %d from %d, current D %d\n",cn,sn,dist);

	set<int>::iterator sit;
	map<int, node *>::iterator kmit;
	map<int, node *>::iterator it;

	node * SN = GETNODE(sn);
	node * CN = GETNODE(cn);
	node * child;

	if(CN->v ==1)
	{
		return -1;
	}

	CN->v = 1;

	MF(kmit, CN->kids)
	{

		child = GETNODE((*kmit).second->num);

		int D = (*kmit).first + dist;


		printf("str %d , cn %d child %d\n", sn, cn, child->num);
		printf("possible new dis is %d\n", D);

		if(child->num != sn && Sgph[sn-1][child->num-1] > D)
		{
			printf("new min dist of %d vs %d from %d to %d\n", D, Sgph[sn-1][child->num-1],sn, child->num);
			Sgph[sn-1][child->num-1] = D;


			MF(it, SN->kids)
			{
				if((*it).second == child)
				{
					SN->ADDCHILD( child->num, D, child);
				}
			}
		}
	}


	MF(kmit, CN->kids)
	{
		child = GETNODE((*kmit).second->num);

		int D = (*kmit).first + dist;

		if(child->v == 0)
		{
			Q.push(child->num);
			printf("adding dist %d\n",Sgph[sn-1][child->num-1]);
		}
	}


	return 1;
}

void graph::MINDIKSTRA()
{

	set<node *>::iterator sit;

	queue<int> Q, D;

	SF(sit, G)
	{
		Q.push((*sit)->num);
		D.push(0);

		while(!Q.empty())
		{
			GDIKSTRA((*sit)->num, Q, Sgph[(*sit)->num-1][Q.front()-1] );
		}

		ResetG();
	}
}

int graph::MXST()
{
	map<int, set<int> > MS;
	map<int, set<int> >::iterator mit;
	set<int> MSET;
	set<int>::iterator sit;;
	MSET.clear();

	int i = 0, j = 0, dis= 0;

	int mx = -MX;


	F(i, Sgph.size())
	{
		int cnode = i;
		MS.clear();
		F(j, Sgph[i].size())
		{

			int to = j;

			dis = Sgph[cnode][to];

			if( dis != MX)
			{
				if(PNT) printf("dist from %d to %d is %d\n", cnode, to , dis);

				mit = MS.find( dis );

				if(mit == MS.end())
				{
					if(PNT)printf("dist %d not found\n", dis);
					MS[dis].insert(i);;
					MS[dis].insert(j);;
					MSET.clear();

					int szs = (int)MS[dis].size();

					if(PNT)printf("the size of the set at %d is now %d\n", dis, szs);


					if(PNT)printf("mx is %d\n",mx);

					if( mx < szs )
					{
						mx = szs;
						if(PNT)printf("-------------------------adjusting max to %d\n",mx);
					}
				}
				else
				{
					MSET = (*mit).second;

					int bdm = 0;

					SF(sit, MSET)
					{
						if( Sgph[(*sit)][j] != dis)
						{
							bdm = 1;
						}
					}

					if(bdm == 0) MS[dis].insert(j);
					if( mx < MS[dis].size() ) mx = MS[dis].size();
					MSET.clear();
				}
			}
		}
	}


	printf("max is %d\n",mx);
}


queue<int> graph::GBFS( queue<int> &Q, int from,queue<int> D, int sk)
{
	if(Q.empty())
	{
		return Q;
	}

	int i, j, k, num, dis;

	queue <int> rq;

	set<int>::iterator sit;
	map<int, node *>::iterator mit, mit2;

	int nde = Q.front();
	int dist = D.front();
	Q.pop();
	D.pop();

	if(PNT) printf("processeing node: %d\n",nde);
	if(PNT) printf("dist from start node %d is %d\n",from, dist);


	if(nde == sk)
	{
		printf("check path\n");
		return Q;
	}

	node * nd = GETNODE(nde);

	node * nf, * kid;

	if(nd == NULL)
	{
		printf("node %d NOT FOUND\n",nde);
		exit(0);
	}

	if(nd->v == 1)
	{
		return Q;
	}

	nd->v = 1;

	MF(mit, nd->kids)
	{
		k = (*mit).second->num;
		int dis = (*mit).first;

		if(dis + dist < Sgph[from][k])
		{
			kid = GETNODE(k);

			Sgph[from][k] = dis + dist;

			nf = GETNODE(from);

			MF(mit2,nf->kids)
			{
				if((*mit).second == GETNODE(k))
				{
					nf->kids.erase(mit);
				}
			}

			kid->parent = nd;

			nf->kids.insert(make_pair(dis+dist,kid));
		}

		if(kid->v == 0)
		{
			Q.push(k);
			D.push(dis+dist);
		}
	}

	return Q;
}


vector <int> graph::BFSFIND(int sk)
{
	int i ,j, k, num, dis;

	node * nd, *kid;

	set<node *>::iterator sit;
	map<int, node *>::iterator mit;
	queue<int> Q;
	queue<int> D;
	vector<int> path;

	SF(sit, G)
	{
		nd = (*sit);

		Q.push(nd->num);
		D.push(0);
		path.push_back(nd->num);

		printf("adding node %d\n", nd->num);
		while(!Q.empty())
		{
			GBFS(Q, nd->num, D, sk);

		}


		path.clear();
	}


	return path;

}



int FLOYDW(int chg, vector<vector<int> >&G, int t)
{
	int i, j, k;

	F(k, G.size())
	{
		F(i, G.size())
		{
			F(j, G.size())
			{
				int ij = G[i][j];
				int ik = G[i][k];
				int kj = G[k][j];

				if(PNT)	printf("G[%d][%d] %d > G[%d][%d]= %d + G[%d][%d]= %d\n",i,j,G[i][j], i,k,G[i][k], k, j, G[k][j]);

				if( t == 0 && ik < MX && kj < MX && ij > ik + kj)
				{
					if(PNT)printf("new shortest path %d\n", ik+kj);
					G[i][j] = G[i][k] + G[k][j];
				}
				else if( t == 1 && i != j && i != k && k != j && ik > -MX && kj > -MX )
				{
					int f = (ik < kj ) ? ik : kj;

					if(PNT)printf("new longest path %d\n", ik+kj);

					if( ik+kj > G[i][j])
					{
						G[i][j] = ik+kj;;

					}
				}
				else if(t == 0) if(PNT) pmsg("no new sp\n");
				else if(PNT)pmsg("no new lp\n");

			}
		}
	}

	return chg;
}


int GFW(int &i,int &j,int &k,graph  &gph, int chg)
{

	int c, d, e, inum = 0;

	if( k == 0)
	{
		return gph.gph[i][j];
	}

	F(c, gph.gph.size())
	{
		F(d, gph.gph[i].size())
		{
			if(c == d)
			{
				gph.gph[c][d] = 0;
			}
			else
			{

			}
		}
	}


	int rtn = 0;


	return rtn;
}

void PNTSV( vector<string> sv, string name)
{

		printf("%s....\n",name.c_str());

		for(int i = 0; i < sv.size();i++)
		{
			printf("%s  ", sv[i].c_str());
		}

		printf("\n");

		return;
}

vector<string> StoSV( string s, string name)
{
	int i = 0, j = 0;

	vector<string> sv;
	sv.clear();

	char * buff[SZ];

	string cs;

	while(i < s.size())
	{
		j = 0;
		while( i < s.size() && s[i] != ' ')
		{
			cs.push_back(s[i]);
			i++;
			j++;
		}


		sv.push_back(cs);
		cs.clear();


		while(i < s.size() && s[i] == ' ')
		{
			i++;
		}
	}

	return sv;
}


vector<int> SVtoIV(vector<string> sv)
{
	int i = 0;

	vector<int> rtv;
	rtv.clear();

	F(i, sv.size())
	{
		rtv.push_back( atoi(sv[i].c_str()));
	}

	return rtv;
}



int main(int argc, char ** argv)
{

	if(argc < 0)
	{
		EPNT("usage: a.out stuff\n");
		exit(0);
	}

	int num = 10;

	string N,from,to,wt,chg;

	getline(cin,N);
	getline(cin,from);
	getline(cin,to);

	vector<int> fv,tv,wtv;
	string fm = "from", tt = "to", ww = "wt";

	fv = (SVtoIV( StoSV(from,fm) ));
	tv = (SVtoIV( StoSV(to  ,tt) ));
	int numb = 0;

	int cnt = 0;

	while(cnt < fv.size())
	{
		cin>>numb;
		cnt++;
		wtv.push_back(numb);
	}

	cin>>chg;

	int n = atoi(N.c_str());
	int chn = atoi(chg.c_str());

	graph gph(fv, tv, wtv,n,0);

	gph.max = SZ;
	gph.min = -SZ;

	int i= 0, j = 0, k = 0, fd = 0, *ip= NULL;

	double dbl = 0, *dp = NULL;

	float fl = 0.0, *flp = NULL;

	FILE * fp;

	set<int>::iterator sit;
	set<int> IS;
	vector<int> IV;
	vector<vector<int> > IVV;

	string str;

	FLOYDW(chn,gph.Sgph,0);


	gph.MakeA();


	if( chn == 0)
	{
		printf("%d\n",gph.Sgph[0][n-1]);
		return gph.Sgph[0][n-1];
	}
	else if( n == 1)
	{
		printf("%d\n", -1*gph.gph[0][0]*chn);
		return -1*gph.gph[0][0]*chn;
	}
	else if(chn == 1 || wtv.size() == 1)
	{
		printf("%d\n",gph.A[0][n-1]);
		return gph.A[0][n-1];
	}
	else
	{
		vector<vector<long> >  ans = gph.MakeAchgs(chn);

		printf("%ld\n",ans[0][n-1]);
		return ans[0][n-1];
	}

}
