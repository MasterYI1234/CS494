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
void CalcFlow_FW(int N, uint8_t * Flow, uint8_t *Adj)
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
{
	int i, j, v, f;

	uint8_t *buf1, *buf2, *buf4;
	//uint8_t *buf1, *buf2, *buf3, *buf4,*vnd;
	//__m128i *iv, *vv, *fv, *rv, *alli;
	__m128i *vv, *fv, *alli;
	__m128i * FL = (__m128i *)Flow;
	__m128i * AD = (__m128i *)Adj;;

	buf1   = (uint8_t *) calloc(N,sizeof(char));
	//buf3   = (uint8_t *) calloc(N,sizeof(char));
	buf2   = (uint8_t *) calloc(N,sizeof(char));
	 buf4   = (uint8_t *) calloc(N,sizeof(char));
	 //vnd   = (uint8_t *) calloc(N,sizeof(char));

	 //allows me to acces the buffers as 128 bit registers
	 alli = (__m128i *) buf1;
	 //rv = (__m128i *) vnd;
	 fv = (__m128i *) buf2;
	 //iv = (__m128i *) buf3;
	 vv = (__m128i *) buf4;
	 

	for (i = 0; i < N*N; i++) Flow[i] = Adj[i];
	
//	memcpy(FL,AD,N*N);
/*
	for (i = 0; i < N*N; i += 16) 
	{
		*FL = *AD;
		FL++;
		AD++;

	}
*/

	for (v = 0; v < N; v++) 
	{   
		//printf("\nV is %d \n",v);


		for (i = 0; i < N; i++) 
		{   
			if(i != v)
			{
				/* Create a vector alli, which is 16 instances of Flow[i*N+v] */
				*alli =  _mm_set1_epi8(Flow[i*N+v]);

				for (j = 0; j < N; j += 16) 
				{

					/* Load Flow[v*N+j] through Flow[v*N+j+15] to vector vv */
					memcpy(vv , Flow + (v*N)+j,16);

					/* Create fv, which is the flow from i to each of j through j+15
					 *            through v. This is simply the min of alli and vv. */
					*fv = _mm_min_epu8(*alli, *vv); 

					/* Load Flow[i*N+j] through Flow[i*N+j+15] to vector iv */
					//memcpy(iv , Flow + (i*N)+j,16);
					memcpy(vv , Flow + (i*N)+j,16);
					
					/* Create rv, which is the max of each value of fv and iv. */
					//*iv = _mm_max_epu8(*fv, *iv); 
					//*rv = _mm_max_epu8(*fv, *iv); 
					*vv = _mm_max_epu8(*fv, *vv); 

					/* Store rv into Flow[i*N+j] through Flow[i*N+j+15] */
					memcpy(Flow +( (i*N)+j), vv, 16); 

				}   
			}

		}
	}
}
