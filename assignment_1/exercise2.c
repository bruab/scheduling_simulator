#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Set row/column size
// 1100 is the magic number on my crappy laptop
#define N 1100

// Declare arrays
int A[N][N];
int B[N][N];
int C[N][N];

int main(int argc, char *argv[])
{
	int i, j, k, sum;
	struct timeval t0, t1;
	double elapsedTime;
	int *ptrC, *ptrA, *ptrB;
	
	// Populate A and B with random integers
	int seed = 1234;
	srand(seed);
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			A[i][j] = rand();
			B[i][j] = rand();
			C[i][j] = 0;
		}
	}

	ptrC = &(C[0][0]);
	ptrA = &(A[0][0]);
	ptrB = &(B[0][0]);

// Multiply A x B
	char ordering[] = {'i', 'k', 'j', '\0'};
	gettimeofday(&t0, NULL);
	for (i=0; i<N; i++) {
		for (k=0; k<N; k++) {
			ptrC = &(C[i][0]);
			ptrA = &(A[i][k]);
			for (j=0; j<N; j++) {
				// used to be C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
				*ptrC = *ptrC + (*ptrA * *ptrB);
				ptrC ++;
				ptrB ++;
			}
			ptrA ++;
		}
		ptrB = &(B[0][0]);
		ptrC ++;
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

	// Output diagonal sum of C
	sum = 0;
	for (i=0; i<N; i++) {
		sum = sum + C[i][i];
	}

	printf("%d\t%f\t%s\n", sum, elapsedTime, ordering);

	return 0;
}
