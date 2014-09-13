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
#define N 8
#define BLOCK_SIZE 4

// Declare arrays
int A[N][N];
int B[N][N];
int C[N][N];

int main(int argc, char *argv[])
{
	int i, j, k, jj, kk, sum, total;
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

	for(jj=0; jj<N; jj+=BLOCK_SIZE){
		for(kk=0; kk<N; kk+= BLOCK_SIZE){
			for(i=0; i<N; i++){
				for(j = jj; j<((jj+BLOCK_SIZE)>N?N:(jj+BLOCK_SIZE)); j++){  // Hope to understand this line by mid-semester
					total = 0;
					for(k = kk; k<((kk+BLOCK_SIZE)>N?N:(kk+BLOCK_SIZE)); k++){
						total += A[i][k]*B[k][j];
					}
					C[i][j] += total;
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

	for(kk=0; kk<N; kk+=BLOCK_SIZE){
		for(jj=0; jj<N; jj+=BLOCK_SIZE){
			for(i=0; i<N; i++){
				for(k = kk; k<((kk+BLOCK_SIZE)>N?N:(kk+BLOCK_SIZE)); k++){  // Hope to understand this line by mid-semester
					total = 0;
					for(j = jj; j<((jj+BLOCK_SIZE)>N?N:(jj+BLOCK_SIZE)); j++){
						total += A[i][k]*B[k][j];
					}
					C[i][j] += total;
				}
			}
		}
	}
	gettimeofday(&t1, NULL);
	elapsedTime = t1.tv_sec - t0.tv_sec;
	/*
	char ordering[] = {'i', 'k', 'j', '\0'};
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
	*/

#endif

	// Output diagonal sum of C
	sum = 0;
	for (i=0; i<N; i++) {
		sum = sum + C[i][i];
	}

	printf("%d\t%f\t%s\n", sum, elapsedTime, ordering);

	return 0;
}

