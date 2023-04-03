#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <set>

//macros:
#define F(i,l) for( i = 0; i < l; i++)
#define Fs(i,s,l) for( i = s; i < j; i++)
#define EPNT(s) fprintf(stderr,"%s\n",s)

#define SZ 256

const int PNT = 1;

using namespace std;


int main(int argc, char ** argv)
{

	if(argc < 0)
	{
		EPNT("usage: a.out stuff\n");
		exit(0);
	}


	int i= 0, j = 0, k = 0, fd = 0, *ip= NULL;

	double dbl = 0, *dp = NULL;

	float fl = 0.0, *flp = NULL;

	FILE * fp;

	set<int>::iterator sit;
	set<int> IS;
	vector<int> IV;
	vector<vector<int> > IVV;

	string str;


	return 0;
}
