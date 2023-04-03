#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <set>

#include "The-Tips.h"
#include "MOA.h"
#include <emmintrin.h>

//macros:
#define F(i,l) for( i = 0; i < l; i++)
#define Fs(i,s,l) for( i = s; i < j; i++)
#define EPNT(s) fprintf(stderr,"%s\n",s)

#define SZ 256

const int PNT = 1;

using namespace std;

class clue
{
	public:
		int prob;
		vector<int> adj;

};

class room
{
	public:
		vector<int> Gph;
		int N;
		vector<double> p;
};

void READ_GRAPH(void *reg, int size)
{
	uint32_t *r32;
	uint8_t *r8;
	int i;

	r32 = (uint32_t *) reg;
	r8 = (uint8_t *) reg;
	//	for (i = 0; i < size/4; i++) r32[i] = MOA_Random_32();
	//	for (i *= 4; i < size; i++) r8[i] = MOA_Random_W(8, 1);
}


double TheTips::solve(vector <string> clues, vector <int> probability, int print)
{
	uint8_t *buf1, *buf2, *buf3, *Cb, *Ca,  *con, **C2, *cp1, *cp2, *vnd;  
	__m128i *b1, *b2, *b3, *R, *Ra, *CP1, *CP2, *Cn, one =  _mm_set_epi32(0,0,0,1), ze = _mm_setzero_si128 ();
	__m128i shft =  _mm_set_epi32(0,0,0,0);
	__m128i *VND;

	//*VND = _mm_setzero_si128 ();;


	int N = clues.size();

	shft = _mm_set_epi32(0,0,0,N);

	vector<int> prob = probability;
	vector<double> p(N,0.0);

	double rtnd;

	int i, j, k, v, num;

//	printf("N is %d\n", N);

	vector<char> room(N, 0);

	vector<vector<char> > C(N);


//	printf("the size of and uint8 is %d\n",sizeof(uint8_t));

	int bufsize = N;

	int adjs = 1;

	int ndbit = N*N;

	printf("need %d bits\n", ndbit);

	int req = 128;

	while(req < N*N)
	{
		req += 128;
	}

	int ndbyt = req/8;

	printf("need %d bytes\n", ndbyt);



	if( N > 8)
	{
//		printf("must decide how may bytes each row gets\n");

		while( adjs*8 < N)
		{	
			adjs++;
		}

//		printf("Need %d bytes per node\n",adjs);

	}


	int bytes = adjs*N;

//	printf("Need %d bytes \n",bytes);

	bufsize = bytes;

	int bfsz = ndbyt;

	if (bufsize % 16 != 0) bufsize += (16 - bufsize%16);
	if (bfsz % 16 != 0) bfsz += (16 - bfsz%16);

//	printf("buff size is %d\n", bufsize);
	printf("New buff size is %d\n", bfsz);

	buf1 = (uint8_t *) calloc(0,bufsize);
	buf2 = (uint8_t *) calloc(0,bufsize);
	buf3 = (uint8_t *) calloc(0,bufsize);
	cp1 = (uint8_t *) calloc(0,bufsize);
	cp2 = (uint8_t *) calloc(0,bufsize);
	Cb   = (uint8_t *) calloc(0,bufsize);
	Ca   = (uint8_t *) calloc(0,bufsize);
	con  = (uint8_t *) calloc(0,bfsz);
	vnd   = (uint8_t *) calloc(0,bufsize);

	//allows me to acces the buffers as 128 bit registers
	b1 = (__m128i *) buf1;
	b2 = (__m128i *) buf2;
	b3 = (__m128i *) buf3;
	R  = (__m128i * )Cb;
	Ra  = (__m128i * )Ca;
	CP1 =  (__m128i *)cp1;
	CP2 = (__m128i *)cp2;
	Cn = (__m128i *)con;
	VND = (__m128i *)vnd;
	*VND = _mm_setzero_si128 ();;


	*Ra = _mm_set_epi32(0,0,0,0);
	*(Ra + 1) = _mm_set_epi32(0,0,0,0);

	int id = 0; 

	for(i = 0, id = 0; id < adjs ; i++, id++)
	{



		for(j = 0; j < N ; j++)
		{
			if( j != 0 && j%8 == 0)
			{
				id++;
			}
			cp1[id]	|= (1<<j%8); 
			cp2[id]	|= (1<<j%8); 
		}
	}

	int mul = 1;

	char cpp;

	*CP2 = *CP1;

	char   Nc = (char ) N;
	shft = _mm_set_epi32(1,1,3,2);

	k = -1;

	int num128 = bufsize/16;

//	printf("need %d 128 vect's\n", num128);

	int nm = 0x0f, sh= 0;

	int set = 0;

	k = 0;

	int cnm = 0, jnm = 0;
	for( i = 0; i < N*N; i++, jnm++)
	{
		if(i > 0 && i%8 == 0) k++;
		if( i > 0 && i%N == 0) cnm++;
		con[k] |= (clues[cnm][jnm%N] == 'Y') ? (1<<(i%8)) : 0;

	}

	printf("hi\n");

	//graph setting up loop
	for( i = 0; i < N ; i++, k++)
	{
		C[i].resize(N);
		set = 0;
		int s = 0;
		
		//		   printf("node %d has a prob of %d\n", i, prob[i]); 
		for(j = 0; j < N; j++)
		{

			//			   if( clues[i][j] == 'Y') printf("node %d can get to %d\n", i, j);
			//			   else printf("node %d can NOT get to %d\n", i, j);
			C[i][j] = (clues[i][j] == 'Y') ? 1 : 0;
			//Cb[i*N+j] = (clues[i][j] == 'Y') ? 1 : 0;
			if(j != 0 && j% 8 == 0 ) 
			{
				k++;
			}

			//				printf("K is %d\n",k);	

			Cb[k] |= (clues[i][j] == 'Y') ? (1<<(j%8)) : 0;
			
			//				*Ra =  (clues[i][j] == 'Y') ? _mm_or_si128(*Ra, one<<(i*N+j)) :_mm_or_si128(*Ra, ze);
			//*Ra |= (clues[i][j] == 'Y') ? (one<<(i*N+j)) : ze;
			//				s++;
		}
	}

	///		printf("\n");


	for(i = 0, k = 0; i < N*N; i++)
	{
		if( i > 0 && i%8 == 0) k++; 
		printf("%d ", (con[k]>>i%8 & 1));
		if(i > 0 &&  (i+1)%N == 0) printf("\n");
	}

	printf("\n");

	exit(0);

	//		for(i = 0, k= 0; i < N ;i++, k++)
	//		{
	//			printf("---------------------------------Node %d\n",i);
	//			for(j = 0; j < N; j++)
	//			{
	//				if(j!= 0 && j%8 == 0) k++;
	//				printf("%d ", (Cb[k]>>j%8 & 1));
	//			}
	//			printf("\n");
	//		}

	///		printf("\n\n");




	int lim = k-1;

	k =0;
	int s=0;
	/*Prints the nodes for debugging*/
	//		for(i = 0; i < N; i++)
	//		{
	//			//printf("\nk is %d\n", k);
	//			printf("\nnode is %d\n", i);
	//			for(j = 0, s = 0; j < N; ++j, s++)
	//			{
	//				if( j != 0 && j %8 == 0) 
	//				{
	//					k++;
	//					s=0;
	//					//printf("\nk is %d\n", k);
	//				}
	//				//els/e printf("\nk is %d\n", k);
	//				printf("%d",((Cb[k] >> s) & 1));
	///			}
	//			k++;
	//			printf("\n");
	//		}

	//		printf("\n");
	//		printf("\n");

	//exit(0);

	int iv = 0, ij = 0, vj = 0;


	double prb = 0.0;


	//floyd warshall
	//intermediate node V
	//from node I 
	for (i = 0, iv = 0; i < N; i++,iv++) 
	{
		for (v = 0, vj = 0; v < N; v++, vj++) 
		{
			if(v != 0 && v %8 == 0)
			{
				vj++;
			}
			if(v != 0 && v %8 == 0)
			{
				iv++;
			}
			//if (C[i][v] )
			//if I can get to intermediate node v
			//or row I with row V and store in row i
			if ((Cb[iv]>>v%8)&1 == 1) 
			{

				//					printf("NODE %d can get to %d, byte is %d\n", i, v, iv);

				b1 = (__m128i *) Cb;   //iv

				VND = (__m128i *)vnd;
				*VND = _mm_setzero_si128();

				for(int l = 0; l < adjs; l++)
				{
					vnd[i*adjs+l] = Cb[v*adjs+l]; 

				}


				for( j = 0; j < bufsize; j += 16)
				{

					*b1 = _mm_or_si128(*b1, *VND);
					b1++;
					VND++;
				}


				for (j = 0; j < C.size(); j++) 
				{
					C[i][j] |= (C[v][j]);
				}

			}
		}
	}


	double x = 0.0;

	for (i = 0, k = 0; i < N; i++, k++) 
	{
		x = (double)probability[i];

		x = x/100.0;

		for (j = 0; j < N; j++) 
		{  
			if( j != 0 && j % 8 == 0)
			{
				k++;
			}

			//printf("--------------------j is %d\n",j); 
			//if (C[i][j] == 1) 
			if (Cb[k]&(1<<j%8)) 
				//if ((Cb[k]>>j%8)&1 == 1) 
			{
				//	printf("node %d can get to %d\n",i, j);
				//	printf("node %d has  %.3f\n",j, (float)p[j]);
				p[j] += ((1 - p[j]) * x);
				//printf("now is %.3f\n",(float)p[j]);
			}

			else if(i == j)
			{
				//p[j] += x;
				//printf("Node %d can get to %d\n",i, j);
				//printf("Node %d has  %.3f\n",j, (float)p[j]);
				p[j] += ((1 - p[j]) * x);
				//printf("Now is %f\n",(float)p[j]);
			}


		}

		//printf("\n\n");
	}

	/* Calculate the final return value */
	x = 0;
	//for (i = 0; i < C.size(); i++) x += p[i];
	for (i = 0; i < N; i++) x += p[i];
	//printf("returning %f\n\n",(float)x);

	return x;

}



