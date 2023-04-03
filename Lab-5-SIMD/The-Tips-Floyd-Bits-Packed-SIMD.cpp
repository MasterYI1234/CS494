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

//sets the bit pointed by by mem at bit offset off to a 1 (oz = 1), or a 0 (oz = 0)
//treats memory as a char
void setbit( void * mem, unsigned int off, int oz)
{
	uint8_t * get = (uint8_t * )mem;

	int from = off/8;

	int to = off%8;

	if(oz == 1) get[from]  |= (oz<<to);
	else get[from]  &= (oz<<to);
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

	con = (uint8_t *)calloc(N*mul,sizeof(__m128i));

	C = (__m128i *) con;


	//use clues vector to set up initial connectivity matrix
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			if(j == i) setbit(C,i*(128*mul)+i,1);
			else if(clues[i][j] == 'Y') setbit(C,i*(128*mul)+j,1);
		}
	}


	//the below would print the matrix testing
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


	//use a version of FW to set up the connectivity matrix
	//with SIMD instructions for massive speed
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

				for( j = 0; j < N; j += 128)
				{
					*b1 = _mm_or_si128(*b1, *b2);
					b1++;
					b2++;
				}
			}
		}
	}


	//the below would print the matrix testing
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


	//use the connectivity matrix to
	//calculate probabilities of each
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

	//sum and return
	for(i = 0; i < N; i++) x += p[i];


	return x;
}


