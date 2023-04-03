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
	uint8_t *buf1, *buf2, *buf3, *Cb;  
	__m128i *b1, *b2, *b3;

	int N = clues.size();

	vector<int> prob = probability;
	vector<double> p(N,0.0);

	double rtnd;

	int i, j, k, v, num;

	printf("N is %d\n", N);

	vector<char> room(N, 0);

	   vector<vector<char> > C(N);


		printf("the size of and uint8 is %d\n",sizeof(uint8_t));

	   int bufsize = N;

	  int adjs = 1;

	   if( N > 8)
	   {
		   printf("must decide how may bytes each row gets\n");

		   while( adjs*8 < N)
		   {	
				adjs++;
		   }

		   printf("Need %d bytes per node\n",adjs);

	   }

		
	   int bytes = adjs*N;

	   printf("Need %d bytes \n",bytes);
	   

	   bufsize = bytes;

	   if (bufsize % 16 != 0) bufsize += (16 - bufsize%16);
	   buf1 = (uint8_t *) calloc(0,bufsize);
	   buf2 = (uint8_t *) calloc(0,bufsize);
	   buf3 = (uint8_t *) calloc(0,bufsize);
	   Cb   = (uint8_t *) calloc(0,bufsize);

	 //*buf1 = 0xff;

	   //allows me to acces the buffers as 128 bit registers
	   b1 = (__m128i *) buf1;
	   b2 = (__m128i *) buf2;
	   b3 = (__m128i *) buf3;


	   /*Make a buffer for oring and anding*/

	   k = -1;
	   

		   for(i = 0; i < N ; ++i)
		   {
			   if( i%8 == 0) k++;
			   buf1[k] |= (1<<i%8);
		   }


	   for( j = 0; j < N; j++)
	   {
		   printf("node %d\n", j);

		   k = 0;
		   for(i = N*j; i < bufsize ; i++)
		   {
			   if( i > 0 && i % 8 == 0) 
			   {
				   k++;
				   printf("\n");
			   }
			   if((buf1[k]<<j*N )& (1<<(i))) printf("%d",1);
			   else printf("%d",0);;
		   }
		   printf("\n");
	   }

	   printf("\n");




	   printf("buff size is %d\n", bufsize);


	   for( i = 0; i < N ; i++)
	   {
		   printf("%d ", 1<<i);
	   }

	   printf("\n");
	   exit(0);

	   for( i = 0; i < N ; i++)
	   {
		   C[i].resize(N);

		   printf("node %d has a prob of %d\n", i, prob[i]); 
		   for(j = 0; j < N; j++)
		   {

			   if( clues[i][j] == 'Y') printf("node %d can get to %d\n", i, j);
			   else printf("node %d can NOT get to %d\n", i, j);
			   C[i][j] = (clues[i][j] == 'Y') ? 1 : 0;
			   //Cb[i*N+j] = (clues[i][j] == 'Y') ? 1 : 0;
			   Cb[i] |= (clues[i][j] == 'Y') ? (1<<j) : 0;
		   }
	   }

	   printf("\n");

	   int lim = 0;

	   for(i = 0; i < N; i++)
	   {
		   if(N < 8) lim = 8;
		   else lim = N;
		   for(j = 0; j < lim; j++)
		   {
			   printf("%d",((Cb[i] >> j) & 1));
		   }

		   printf(" ");
	   }

	   printf("\n");
	   printf("\n");

	   for (v = 0; v < C.size(); v++) 
	   {
		   for (i = 0; i < C.size(); i++) 
		   {
			   //if (C[i][v] )
			   if (Cb[i]&1<<v) 
			   {
				   printf("node %d can get to %d\n", i, v);
				   b1 = (__m128i *) Cb+i;   //iv
				   b2 = (__m128i *) Cb+j;   //ij
				   b3 = (__m128i *) Cb+v;   //vj

				   for( j = 0; j < N; j += 16)
				   {

					   *b2 = _mm_or_si128(*b2, *b3);
					   b2++;
					   b3++;
				   }

				   /*
					  for (j = 0; j < C.size(); j++) 
					  {
					  C[i][j] |= (C[v][j]);
					  }
					  */
			   }
		   }
	   }


	   double x = 0.0;

	   for (i = 0; i < C.size(); i++) 
	   {
		   printf("---9090-----------------I is %d\n",i); 
		   x = (double)probability[i];
		   printf("node %d has a prob of %.1f\n", i, (float)x);

		   x = x/100.0;

		   printf("\n\n------node %d has a prob of %.4f\n", i, (float)x);

		   for (j = 0; j < C.size(); j++) 
		   {  
			   printf("--------------------j is %d\n",j); 
			   if (Cb[i]&(1<<j)) 
			   {
				   printf("node %d can get to %d\n",i, j);
				   printf("node %d has  %.3f\n",j, (float)p[j]);
				   p[j] += ((1 - p[j]) * x);
				   printf("now is %.3f\n",(float)p[j]);
			   }
			   else if(i == j)
			   {
				   //	p[j] += x;
				   printf("Node %d can get to %d\n",i, j);
				   printf("Node %d has  %.3f\n",j, (float)p[j]);
				   p[j] += ((1 - p[j]) * x);
				   printf("Now is %f\n",(float)p[j]);
			   }

		   }

		   printf("get it \n\n");
		   printf("\n\n");
	   }
	   printf("2get it \n\n");

	   /* Calculate the final return value */
	   x = 0;
	   for (i = 0; i < C.size(); i++) x += p[i];
	   printf("returning %f\n\n",(float)x);

	   return x;

}



