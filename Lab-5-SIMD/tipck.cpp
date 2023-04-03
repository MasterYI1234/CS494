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

void setbit( void * mem, int off, int oz)
//void setbit( void * mem, int i, int j)
{
	uint8_t * get = (uint8_t * )mem;

	int from = off/8;

	int to = off%8;
//	printf("byte : %d to %d\n", from, to);

	if(oz) get[from]  |= (1<<to);
	else  get[from]  ^= (1<<to);
}

void vprintit( vector<vector<char> > C )
{
	int i, j;
	F(i, C.size())
	{
		printf("Node: %d\n");
		F(j, C.size())
		{
			printf("%d ",C[i][j]);
		}
		printf("\n");
	}
}

void Sprintit( uint8_t *C , int N)
{
	int i, j;
	F(i, N)
	{
		printf("Node: %d\n");
		F(j, N)
		{
		}
		printf("\n");
	}
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


//	printf("Need %d bytes \n",bytes);

	bufsize = N;


	if (bufsize % 16 != 0) bufsize += (16 - bufsize%16);

//	printf("buff size is %d\n", bufsize);
//	printf("New buff size is %d\n", bfsz);

	buf1 = (uint8_t *) calloc(0,bufsize);
	buf2 = (uint8_t *) calloc(0,bufsize);
	buf3 = (uint8_t *) calloc(0,bufsize);
	cp1 = (uint8_t *) calloc(0,bufsize);
	cp2 = (uint8_t *) calloc(0,bufsize);
	Cb   = (uint8_t *) calloc(0,bufsize);
	con   = (uint8_t *) calloc(0,N*16);
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


	*Ra = _mm_set_epi32(0,0,0,0);
	*(Ra + 1) = _mm_set_epi32(0,0,0,0);

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
	for( i = 0; i < N ; i++, k++)
	{
		C[i].resize(N);
		
		for(j = 0; j < N; j++)
		{

			C[i][j] = (clues[i][j] == 'Y') ? 1 : 0;
		}
	}

	k = 0;

if(PNT)	printf("\n\n");


	if(PNT)vprintit(C);
	if(PNT)printf("\n\n");

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
			
			if (C[i][v] )
			{

	if(PNT)			printf("NODE %d can get to %d, byte is %d\n", i, v);

				for (j = 0; j < C.size(); j++) 
				{
					C[i][j] |= (C[v][j]);
				}

			}
		}
	}




	if(1)
	{
		for(i = 0; i < N; i++)
		{
			printf("NODE %d\n", i);
			for(j = 0; j < N; j++)
			{

				printf("%d ",C[i][j]);
			}
			printf("\n");
		}
	}





	if(PNT)vprintit(C);
	if(PNT) printf("\n\n");

	double x = 0.0;

	p.clear();

	p.resize(C.size(), 0);

	for (i = 0, k = 0; i < N; i++, k++) 
	{
		x = probability[i];

		x = x/100.0;

		for (j = 0; j < N; j++) 
		{  
			if (C[i][j] == 1) 
			{
				if(PNT)		printf("node %d can get to %d\n",i, j);
				p[j] += ((1 - p[j]) * x);
			}
			else if(i == j)
			{
				if(PNT)	printf("Node %d can get to %d\n",i, j);
				p[i] += ((1 - p[i]) * x);
			}
		}

	}

	/* Calculate the final return value */
	x = 0;
	for (i = 0; i < N; i++) x += p[i];

	return x;

}



