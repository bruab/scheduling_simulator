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
#define N 1100
#define BLOCK_SIZE 100

// Declare arrays
int A[N][N];
int B[N][N];
int C[N][N];

int main(int argc, char *argv[])
{
	int i, j, k, ii, jj, kk, sum;
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
	char ordering[] = {'i', 'j', 'k', '\0'};
	gettimeofday(&t0, NULL);

	for (i=0; i<N/BLOCK_SIZE; i++) {
		for (j=0; j<N/BLOCK_SIZE; j++) {
			for (k=0; k<N/BLOCK_SIZE; k++) {
				for (ii=i*BLOCK_SIZE; ii<(i*BLOCK_SIZE+BLOCK_SIZE); ii++) {
					for (jj=j*BLOCK_SIZE; jj<(j*BLOCK_SIZE+BLOCK_SIZE); jj++) {
						for (kk=k*BLOCK_SIZE; kk<(k*BLOCK_SIZE+BLOCK_SIZE); kk++) {
							C[ii][jj] = C[ii][jj] + A[ii][kk]*B[kk][jj];
						}
					}
				}
			}
		}
	}

	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

// 2 - i, k, j
#elif ORDER == 2
	char ordering[] = {'i', 'k', 'j', '\0'};
	gettimeofday(&t0, NULL);

	for (i=0; i<N/BLOCK_SIZE; i++) {
		for (j=0; j<N/BLOCK_SIZE; j++) {
			for (k=0; k<N/BLOCK_SIZE; k++) {
				for (ii=i*BLOCK_SIZE; ii<(i*BLOCK_SIZE+BLOCK_SIZE); ii++) {
					for (kk=k*BLOCK_SIZE; kk<(k*BLOCK_SIZE+BLOCK_SIZE); kk++) {
						for (jj=j*BLOCK_SIZE; jj<(j*BLOCK_SIZE+BLOCK_SIZE); jj++) {
							C[ii][jj] = C[ii][jj] + A[ii][kk]*B[kk][jj];
						}
					}
				}
			}
		}
	}

	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

// 3 - j, i, k
#elif ORDER == 3
	char ordering[] = {'j', 'i', 'k', '\0'};
	gettimeofday(&t0, NULL);

	for (i=0; i<N/BLOCK_SIZE; i++) {
		for (j=0; j<N/BLOCK_SIZE; j++) {
			for (k=0; k<N/BLOCK_SIZE; k++) {
				for (jj=j*BLOCK_SIZE; jj<(j*BLOCK_SIZE+BLOCK_SIZE); jj++) {
					for (ii=i*BLOCK_SIZE; ii<(i*BLOCK_SIZE+BLOCK_SIZE); ii++) {
						for (kk=k*BLOCK_SIZE; kk<(k*BLOCK_SIZE+BLOCK_SIZE); kk++) {
							C[ii][jj] = C[ii][jj] + A[ii][kk]*B[kk][jj];
						}
					}
				}
			}
		}
	}

	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

// 4 - j, k, i
#elif ORDER == 4
	char ordering[] = {'j', 'k', 'i', '\0'};
	gettimeofday(&t0, NULL);

	for (i=0; i<N/BLOCK_SIZE; i++) {
		for (j=0; j<N/BLOCK_SIZE; j++) {
			for (k=0; k<N/BLOCK_SIZE; k++) {
				for (jj=j*BLOCK_SIZE; jj<(j*BLOCK_SIZE+BLOCK_SIZE); jj++) {
					for (kk=k*BLOCK_SIZE; kk<(k*BLOCK_SIZE+BLOCK_SIZE); kk++) {
						for (ii=i*BLOCK_SIZE; ii<(i*BLOCK_SIZE+BLOCK_SIZE); ii++) {
							C[ii][jj] = C[ii][jj] + A[ii][kk]*B[kk][jj];
						}
					}
				}
			}
		}
	}

	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;

// 5 - k, i, j
#elif ORDER == 5
	char ordering[] = {'k', 'i', 'j', '\0'};
	gettimeofday(&t0, NULL);

	for (i=0; i<N/BLOCK_SIZE; i++) {
		for (j=0; j<N/BLOCK_SIZE; j++) {
			for (k=0; k<N/BLOCK_SIZE; k++) {
				for (kk=k*BLOCK_SIZE; kk<(k*BLOCK_SIZE+BLOCK_SIZE); kk++) {
					for (ii=i*BLOCK_SIZE; ii<(i*BLOCK_SIZE+BLOCK_SIZE); ii++) {
						for (jj=j*BLOCK_SIZE; jj<(j*BLOCK_SIZE+BLOCK_SIZE); jj++) {
							C[ii][jj] = C[ii][jj] + A[ii][kk]*B[kk][jj];
						}
					}
				}
			}
		}
	}

	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;



// 6 - k, j, i
#elif ORDER == 6
	char ordering[] = {'k', 'j', 'i', '\0'};
	gettimeofday(&t0, NULL);

	for (i=0; i<N/BLOCK_SIZE; i++) {
		for (j=0; j<N/BLOCK_SIZE; j++) {
			for (k=0; k<N/BLOCK_SIZE; k++) {
				for (kk=k*BLOCK_SIZE; kk<(k*BLOCK_SIZE+BLOCK_SIZE); kk++) {
					for (jj=j*BLOCK_SIZE; jj<(j*BLOCK_SIZE+BLOCK_SIZE); jj++) {
						for (ii=i*BLOCK_SIZE; ii<(i*BLOCK_SIZE+BLOCK_SIZE); ii++) {
							C[ii][jj] = C[ii][jj] + A[ii][kk]*B[kk][jj];
						}
					}
				}
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

	printf("%d\t%f\t%s\n", sum, elapsedTime, ordering);

	return 0;
}

