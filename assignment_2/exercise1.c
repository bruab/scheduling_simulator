#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Determine which loop is parallelized
// 1 - i, 2 - k, 3 - j
// (Default is 'i')
#ifndef PARALLEL_LOOP
# define PARALLEL_LOOP 1
#endif

// Set row/column size
// 1100 is the magic number on my crappy laptop
#define N 8

// Declare arrays
int A[N][N];
int B[N][N];
int C[N][N];

int main(int argc, char *argv[])
{
	int i, j, k, sum;
	struct timeval t0, t1;
	double elapsedTime;

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

#if PARALLEL_LOOP == 1
// Parallelize the 'i' loop
	char parallel_loop = 'i';
	gettimeofday(&t0, NULL);
	for (i=0; i<N; i++) {
		for (k=0; k<N; k++) {
			for (j=0; j<N; j++) {
				C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

#elif PARALLEL_LOOP == 2
// Parallelize the 'k' loop
	char parallel_loop = 'k';
	gettimeofday(&t0, NULL);
	for (i=0; i<N; i++) {
		for (k=0; k<N; k++) {
			for (j=0; j<N; j++) {
				C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

#elif PARALLEL_LOOP == 3
// Parallelize the 'j' loop
	char parallel_loop = 'j';
	gettimeofday(&t0, NULL);
	for (i=0; i<N; i++) {
		for (k=0; k<N; k++) {
			for (j=0; j<N; j++) {
				C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;
#endif

	// Output diagonal sum of C
	sum = 0;
	for (i=0; i<N; i++) {
		sum = sum + C[i][i];
	}

	printf("%d\t%f\t%c\n", sum, elapsedTime, parallel_loop);

	return 0;
}

