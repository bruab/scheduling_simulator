#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Set ordering scheme for critical loop
// Orders are: 	1 - i, j, k
// 		2 - i, k, j
// 		3 - j, i, k
// 		4 - j, k, i
// 		5 - k, i, j
// 		6 - k, j, i
#ifndef ORDER
# define ORDER 1
#endif

// Set row/column size
// 1100 is the magic number on my crappy laptop
#define N 100

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

// Multiply A x B
// 1 - i, j, k
#if ORDER == 1
	gettimeofday(&t0, NULL);
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			for (k=0; k<N; k++) {
				C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;
	
// 2 - i, k, j
#elif ORDER == 2
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

// 3 - j, i, k
#elif ORDER == 3
	gettimeofday(&t0, NULL);
	for (j=0; j<N; j++) {
		for (i=0; i<N; i++) {
			for (k=0; k<N; k++) {
				C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

// 4 - j, k, i
#elif ORDER == 4
	gettimeofday(&t0, NULL);
	for (j=0; j<N; j++) {
		for (k=0; k<N; k++) {
			for (i=0; i<N; i++) {
				C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

// 5 - k, i, j
#elif ORDER == 5
	gettimeofday(&t0, NULL);
	for (k=0; k<N; k++) {
		for (i=0; i<N; i++) {
			for (j=0; j<N; j++) {
				C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

// 6 - k, j, i
#elif ORDER == 6
	gettimeofday(&t0, NULL);
	for (k=0; k<N; k++) {
		for (j=0; j<N; j++) {
			for (i=0; i<N; i++) {
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

	printf("Sum is %d\n", sum);
	printf("Elapsed time was %f seconds\n", elapsedTime);
	return 0;
}

