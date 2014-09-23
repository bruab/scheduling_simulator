#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#define N 12000

int A[N+2][N+2];

int main(int argc, char **argv) {

  int num_threads;
  int num_iterations;

  int iter;
  int i,j,x;
  struct timeval start, end;

  if (argc != 3) {
	  fprintf(stderr,"Usage: %s <num threads> <num iterations>\n", argv[0]);
	  exit(1);
  }

  if (sscanf(argv[1],"%d",&num_threads) != 1) {
	  fprintf(stderr,"Invalid number of threads\n");
	  exit(1);
  }

  if (sscanf(argv[2],"%d",&num_iterations) != 1) {
	  fprintf(stderr,"Invalid number of iterations\n");
	  exit(1);
  }

  // Fill in the array
  for (i=0; i < N+2; i++) {
    for (j=0; j < N+2; j++) {
      A[i][j] = i+j;
    }
  }

  // Start timer
  gettimeofday(&start,NULL);

  for (iter = 0; iter < num_iterations; iter++) {
	  fprintf(stderr, "."); fflush(stderr);

	  for (x = 1; x < 2*N+1; x++) {
		  //#pragma omp parallel for private(j)
		  for (j = x; j > 0; j--) {
			  int k = x + 1 - j;
			if (j <= N && k <= N) {
          			A[k][j] = (3*A[k-1][j] + A[k+1][j] + 3*A[k][j-1] + A[k][j+1])/4;
			}
			k++;
		  }
	  }
  }

  gettimeofday(&end,NULL);
  printf("\n%f\n", 
		 (end.tv_sec*1000000.0 + end.tv_usec - 
		  start.tv_sec*1000000.0 - start.tv_usec) / 1000000.0);

  // Compute and print the sum of elements for correctness checking
  int sum =0 ;
  for (i=1; i < N; i++) {
    for (j=1; j < N; j++) {
      sum += A[i][j];
    }
  }
  fprintf(stderr,"sum = %d\n",sum);

  exit(0);
}
