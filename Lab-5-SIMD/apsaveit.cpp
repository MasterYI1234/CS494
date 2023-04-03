#include "AP-Flow.h"
#include "MOA.h"
#include <emmintrin.h>
#include <cstring>

#define NL printf("\n");

void printVec( void * mem, int N)
{
	uint8_t * val = (uint8_t *)mem;

	for(int i = 0; i < N ; i++)
	{
		printf("%u ",val[i]);
	}

	NL;

}


//void APFlow::CalcFlow()
void CalcFlow(int N, uint8_t * Flow, uint8_t *Adj)
{
	int i, j, v, f;

	for (i = 0; i < N*N; i++) Flow[i] = Adj[i];

	for (v = 0; v < N; v++) {
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				f = (Flow[i*N+v] < Flow[v*N+j]) ? Flow[i*N+v] : Flow[v*N+j];
				if (f > Flow[i*N+j]) Flow[i*N+j] = f;
			}

			printVec( Flow+i*N, N);
		}
		NL;
	}
}



void APFlow::CalcFlow()
//void CalcFlow(int N, uint8_t * Flow, uint8_t *Adj)
{
	int i, j, v, f;

	uint8_t *buf1, *buf2, *buf3, *buf4, *Cb, *Ca, **C2, *cp1, *cp2, *vnd;
	__m128i *iv, *vv, *fv, *rv, *alli, *CP2, one =  _mm_set_epi32(0,0,0,1), ze = _mm_setzero_si128 ();

	buf1   = (uint8_t *) calloc(N,sizeof(char));
	buf3   = (uint8_t *) calloc(N,sizeof(char));
	buf2   = (uint8_t *) calloc(N,sizeof(char));
	 buf4   = (uint8_t *) calloc(N,sizeof(char));
	 vnd   = (uint8_t *) calloc(N,sizeof(char));

	 //allows me to acces the buffers as 128 bit registers
	 alli = (__m128i *) buf1;
	 rv = (__m128i *) vnd;
	 fv = (__m128i *) buf2;
	 iv = (__m128i *) buf3;
	 vv = (__m128i *) buf4;
	 


	 int ndd = 128;

	 while(ndd < N)
	 {
		 ndd += 128;
	 }

	 int mul = ndd/128;
	 printf("Need %d sectors 128 bits long\n",mul);
			




	for (i = 0; i < N*N; i++) Flow[i] = Adj[i];

	for (v = 0; v < N; v++) 
	{   
		//printf("\nV is %d \n",v);
		
		
		for (i = 0; i < N; i++) 
		{
			//	for(int k = 0; k < N; k++)
			//	{
			//		printVec(Flow+k*N, N);
			//		NL;NL;
			//	}

			//printf("\nV is %d   I is %d \n",v,i); 

			/* Create a vector alli, which is 16 instances of Flow[i*N+v] */
				*alli =  _mm_set1_epi8(Flow[i*N+v]);

			//printf("Alli (%d-->%d): \n",i,v);
			//printVec( alli, N);

			for (j = 0; j < N; j += 16) 
			{

				/* Load Flow[v*N+j] through Flow[v*N+j+15] to vector vv */
				//vv* =  (__m128i *)Flow + v*N+j;
				//*vv =  (__m128i )(*(Flow + v*N+j));
				memcpy(vv , Flow + (v*N)+j,16);
			//	printf("VV: \n");
			//	printVec(vv, N);

				/* Create fv, which is the flow from i to each of j through j+15
				 *            through v. This is simply the min of alli and vv. */
				*fv = _mm_min_epu8(*alli, *vv); 
			//	printf("fv: \n");
			//	printVec( fv, N);

				/* Load Flow[i*N+j] through Flow[i*N+j+15] to vector iv */
				//*iv =  (__m128i )*(Flow + i*N+j);
				memcpy(iv , Flow + (i*N)+j,16);
				//printf("iv: \n");
				//printVec( iv, N);


				/* Create rv, which is the max of each value of fv and iv. */
				//*iv = _mm_max_epu8(*fv, *iv); 
				*rv = _mm_max_epu8(*fv, *iv); 
				//printf("rv: \n");
				//printVec( rv, N);

				//printf("\n--------------------------\n");
			//	for(int k = 0; k < N; k++)
			//	{
			//		printVec(Flow+k*N, N);
			//		NL;NL;
			//	}
			//	printf("\n--------------------------\n");



				/* Store rv into Flow[i*N+j] through Flow[i*N+j+15] */
				memcpy(Flow +( (i*N)+j), rv, 16); 
				//memcpy(Flow +( (i*N)+j), rv, N); 
				
				
			//	printf("\nB--------------------------\n");
			//	for(int k = 0; k < N; k++)
			//	{
			//		printVec(Flow+k*N, N);
			//		NL;NL;
			//	}
			//	printf("\nB--------------------------\n");

				
				
			//	printf("flow + i*N \n");
			//	printVec( Flow+i*N, N);
			//	//*iv = *rv;
			//	NL;
			//	NL;
			}   
		}

	//	NL;
	//	NL;
	}
}
