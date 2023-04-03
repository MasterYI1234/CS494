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

const int PNT = 0;

using namespace std;

void setbit( void * mem, unsigned int off, int oz)
//void setbit( void * mem, int i, int j)
{
	uint8_t * get = (uint8_t * )mem;

	int from = off/8;

	int to = off%8;
//	printf("byte : %d to %d\n", from, to);

	get[from]  |= (1<<to);
}


double TheTips::solve(vector <string> clues, vector <int> probability, int print)
{
	int N = probability.size();

	//printf("N is %d\n",N);

	int i, j, k, v;


	int ndd = 128;

	while(ndd < N)
	{
		ndd += 128;
	}

	int mul = ndd/128;
	//printf("Need %d sectors 128 bits long\n",mul);


	double x = 0.0;

	vector<double> p;

	p.clear();
	p.resize(N,0.0);

	uint8_t * con;

	__m128i *b1, *b2, *C;
	


	//con = (uint8_t *)calloc(N*16,sizeof(char));
	con = (uint8_t *)calloc(N*mul,sizeof(__m128i));

	C = (__m128i *) con;
	

	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			if(j == i) setbit(C,i*(128*mul)+i,1);
			else if(clues[i][j] == 'Y') setbit(C,i*(128*mul)+j,1);
		}

		//setbit(C,i*128+i,1);
	}

	
	if(0)
	{
		for(i = 0; i < N; i++)
		{
			printf("NODE %d\n", i);
			for(j = 0; j < N; j++)
			{
				int from = (i*128*mul+j)/8;
				int to = (i*128*mul+j)%8;

				printf("%d ",( (con[from]>>to) & 1) ? 1:0);
			}
			printf("\n");
		}
	}


	for(v = 0; v < N; v++)
	{
		for(i = 0; i < N; i++)
		{
			int from = (mul*128*i+v)/8;
			int to = (mul*128*i+v)%8;

			if( (con[from]>>to) & 1)
			{
				b1 = C+i*mul;
				b2 = C+v*mul;

				//*b1 = _mm_or_si128(*b1, *b2);
				//from = i*128/8;
				//b1 = (__m128i *) con+from;


				//from = v*128/8;
				//b2 = (__m128i *) con+from;
				for( j = 0; j < N; j += 128)
				{
					//*(C+i) = _mm_or_si128(*b1, *b2);
					*b1 = _mm_or_si128(*b1, *b2);
					b1++;
					b2++;
				}
			}
		}
	}


	if(0)
	{
		for(i = 0; i < N; i++)
		{
			printf("NODE %d\n", i);
			for(j = 0; j < N; j++)
			{
				int from = (i*128+j)/8;
				int to = (i*128+j)%8;

				printf("%d ",( (con[from]>>to) & 1) ? 1:0);
			}
			printf("\n");
		}
	}


	for (i = 0; i < N; i++) 
	{
		x = (double)probability[i];

		x = x/100.0;

		for(v = 0; v < N; v++)
		{
			int from = (mul*128*i+v)/8;
			int to = (mul*128*i+v)%8;

			//if (con[from]&(1<<to))
			if((con[from]>>to) & 1)
			{
				p[v] += (1-p[v])*x;
			}
		}
	}


	x = 0;

	for(i = 0; i < N; i++) x += p[i];


	return x;
}



double Solve(vector <string> clues, vector <int> probability, int print)
{
	uint8_t *buf1, *buf2, *buf3, *Cb, *Ca,  *con, **C2, *cp1, *cp2, *vnd, *Iv, *Vv;  
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

	//	printf("need %d bits\n", ndbit);

	int req = 128;

	while(req < N*N)
	{
		req += 128;
	}

	int ndbyt = req/8;

	//	printf("need %d bytes\n", ndbyt);



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
	//	printf("New buff size is %d\n", bfsz);

	buf1 = (uint8_t *) calloc(0,bufsize);
	buf2 = (uint8_t *) calloc(0,bufsize);
	buf3 = (uint8_t *) calloc(0,bufsize);
	cp1 = (uint8_t *) calloc(0,bufsize);
	cp2 = (uint8_t *) calloc(0,bufsize);
	Cb   = (uint8_t *) calloc(0,bufsize);
	con   = (uint8_t *) calloc(0,N*16);
	Iv   = (uint8_t *) calloc(0,N*16);
	Vv   = (uint8_t *) calloc(0,N*16);
	Ca   = (uint8_t *) calloc(0,bufsize);
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
	//*VND = _mm_setzero_si128 ();;


	//	*Ra = _mm_set_epi32(0,0,0,0);
	//	*(Ra + 1) = _mm_set_epi32(0,0,0,0);

	int id = 0; 

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

	//	printf("hi\n");

	//graph setting up loop
	for( i = 0; i < N ; i++)
	{
		for(j = 0; j < N; j++)
		{

			if(clues[i][j] == 'Y')  setbit(Cn, i*128+j, 1);
		}

		//		setbit(Cn, i*128+i, 1);
	}

	k = 0;

	/*
	   for(i = 0; i < N; i++)
	   {
	   printf("NODE %d\n", i);
	   for(j = 0; j < N; j++)
	   {
	   int from = (i*128+j)/8;
	   int to = (i*128+j)%8;

	   printf("%d ",( (con[from]>>to) & 1) ? 1:0);
	   }
	   printf("\n");
	   }
	   */

	//	exit(0);


	int iv = 0, ij = 0, vj = 0;


	double prb = 0.0;

	//floyd warshall
	//intermediate node V
	//from node I 
	//for (i = 0, iv = 0; i < N; i++,iv++) 
	for (v = 0; v < N; v++) 
	{
		//for (v = 0, vj = 0; v < N; v++, vj++) 
		for (i = 0; i < N; i++) 
		{

			int from = (i*128+v)/8;
			int to = (i*128+v)%8;

			Iv = con+from;		


			if ((con[from]>>to &  (1))) 
			{

				if(PNT)				printf("NODE %d can get to %d, byte is %d\n", i, v, from);

				//b1 = (__m128i *) Cn+i;   //iv

				from = i*128/8;
				b1 = (__m128i *) con+from;


				from = v*128/8;
				b2 = (__m128i *) con+from;

				//b2 = Cn+v;
				//VND = Cn+v;

				*b1 = _mm_or_si128(*b1, *b2);
				//*b1 = _mm_or_si128(*b1, *VND);
				//*(Cn+i) = _mm_or_si128(*(Cn+i), *(Cn+v));

			}
		}
	}

	double x = 0.0;


	p.clear();

	p.resize(N,0);

	for (i = 0; i < N; i++) 
	{
		x = (double)probability[i];

		x = x/100.0;

		for (j = 0; j < N; j++) 
		{  
			int from = (i*128+j)/8;
			int to = (i*128+j)%8;

			if (con[from]&(1<<to)) 
			{
				if(PNT)			printf("node %d can get to %d\n",i, j);
				//printf("node %d has  %.3f\n",j, (float)p[j]);
				p[j] += ((1 - p[j]) * x);
				//printf("now is %.3f\n",(float)p[j]);
			}

			else if(i == j)
			{
				//p[j] += x;
				if(PNT) 	printf("Node %d can get to %d\n",i, j);
				//printf("Node %d has  %.3f\n",j, (float)p[j]);
				p[i] += ((1 - p[i]) * x);
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



