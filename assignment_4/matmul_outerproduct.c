#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>


///////////////////////////////////////////////////////
//// program_abort() and print_usage() functions //////
///////////////////////////////////////////////////////

static void program_abort(char *exec_name, char *message);
static void print_usage();

// Abort, printing the usage information only if the
// first argument is non-NULL (and set to argv[0]), and
// printing the second argument regardless.
static void program_abort(char *exec_name, char *message) {
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  if (my_rank == 0) {
    if (message) {
      fprintf(stderr,"%s",message);
    }
    if (exec_name) {
      print_usage(exec_name);
    }
  }
  MPI_Abort(MPI_COMM_WORLD, 1);
  exit(1);
}

// Print the Usage information
static void print_usage(char *exec_name) {
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

  if (my_rank == 0) {
    fprintf(stderr,"Usage: mpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1Gf -np <num processes>\n");
    fprintf(stderr,"              -platform <XML platform file> -hostfile <host file> %s \n",exec_name);
    fprintf(stderr,"MPIRUN arguments:\n");
    fprintf(stderr,"\t<XML platform file>: a simgrid platform description file\n");
    fprintf(stderr,"\t<host file>: MPI host file with host names from the XML platform file\n");
    fprintf(stderr,"\n");
  }
}

///////////////////////////
////// Main function //////
///////////////////////////

int main(int argc, char *argv[])
{
  int blocks_per_row,N,block_size;
  
  // Get N from command line arg
  // TODO segfaults if i forget to pass arg, so ghetto :(
  N = atoi(argv[3]); 

  MPI_Init(&argc, &argv);

  // Determine rank and number of processes
  int num_procs;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  // Abort if num_procs is not a perfect squre
  blocks_per_row = sqrt(num_procs);
  if (blocks_per_row*blocks_per_row != num_procs) {
        program_abort(argv[0],"Number of processors is not a perfect square, I give up...\n");
  }

  // Abort if sqrt(num_procs) doesn't divide N
  block_size = N / blocks_per_row;
  if (block_size * blocks_per_row != N) {
	  program_abort(argv[0], "Square root of number of processors does not divide N, I give up...\n");
  }

  // Figure out which block we are in
  int num_blocks;
  int this_block_row,this_block_col;

  num_blocks = num_procs;  // Redundant but here to remind me WTH i am doing
  this_block_row = rank / blocks_per_row;
  this_block_col = (this_block_row + rank) % (blocks_per_row + 1);

  // Declare matrices
  double A[block_size][block_size];
  double B[block_size][block_size];
  double C[block_size][block_size];

  int i, j, k;
  // Fill matrices
  for (i=0; i<block_size; i++) {
  	for (j=0; j<block_size; j++) {
		A[i][j] = (rank / blocks_per_row) * block_size + i;
		if (rank == 0) {
			printf("process zero here. A[%d][%d] = %f\n", i, j, A[i][j]);
		}
		B[i][j] = (this_block_row + this_block_col) * block_size + i + j;
		if (rank == 0) {
			printf("process zero here. B[%d][%d] = %f\n", i, j, B[i][j]);
		}
		C[i][j] = 0.0;
	}
  }

  // Multiply matrices
  for (k=0; k<N; k++) {
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			C[i][j] = C[i][j] + (A[i][k] * B[k][i]);
		}
	}
  }

  // Sum elements of matrix C
  double sum;
  sum = 0.0;
  for (i=0; i<block_size; i++) {
  	for (j=0; j<block_size; j++) {
		sum = sum + C[i][j];
		if (rank == 0) {
			printf("process zero here. C[%d][%d] = %f\n", i, j, C[i][j]);
		}
	}
  }

  printf("process %d here, i got a sum of %f\n", rank, sum);

  // Calculate what total should be
  double c_sum;
  c_sum = N*N*N*(N-1)*(N-1)/2;

  printf("total sum should be %f\n", c_sum);
  

  // Start the timer
  double start_time;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {  
    start_time = MPI_Wtime();
  }

  // Not sure what this is here for, but afraid to delete just yet TODO
  MPI_Barrier(MPI_COMM_WORLD);

  // Clean-up
  MPI_Finalize();

  return 0;
}
