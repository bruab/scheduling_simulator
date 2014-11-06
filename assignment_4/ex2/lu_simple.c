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
//////////////////////////////////////////////
////// Matrix and local-global routines //////
//////////////////////////////////////////////

struct cell {
	int row;
	int col;
};

void print_matrix(double *A, int num_cols, int num_rows) {
  int i,j;

  for (i=0; i < num_cols; i++) {
    printf("  ");
    for (j=0; j < num_rows; j++) {
      printf("%2.0lf ",A[i*num_rows+j]);
    }
    printf("\n");
  }
  printf("-------------------------------\n");
  return;
}

int owner_global_column(int col_num, int procs, int N) {
	return col_num * procs / N;
}

int local_to_global_column(int local_col, int rank, int np, int N) {
	int global_col;
	global_col = local_col + rank * N / np;
	return global_col;
}

int global_to_local_column(int global_col, int np, int N) {
	int local_col;
	local_col = global_col % (N / np);
	return local_col;
}

struct cell local_to_global_indices(struct cell local_cell, int rank, int np, int N) {
	int global_row, global_col;
	global_row = local_cell.row;
	global_col = local_cell.col + rank * N / np;
	struct cell global_cell = {global_row, global_col};
	return global_cell;
}

struct cell global_to_local_indices(struct cell global_cell, int np, int N) {
	int local_row, local_col;
	local_row = global_cell.row;
	local_col = global_cell.col % (N / np);
	struct cell local_cell = {local_row, local_col};
	return local_cell;
}

double calculate_cell_value(int i, int j) {
	double sum;
	int k;
        int min_i_j = (i < j ? i : j);
	for (k=0; k<=min_i_j; k++) {
		sum += (k + 1) * (k + 1);
	}
	return (double)sum;
}



///////////////////////////
////// Main function //////
///////////////////////////

int main(int argc, char *argv[])
{
  // Get N from command line arg
  if (argc != 4) {
    fprintf(stderr,"Missing an arg, or too many args.\nUsage: %s <N>\n",argv[0]);
    exit(1);
  }
  int N;
  N = atoi(argv[3]); 

  MPI_Init(&argc, &argv);

  // Determine rank and number of processes
  int num_procs;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  // Abort if num_procs doesn't divide N
  int columns_per_processor;
  columns_per_processor = N / num_procs;
  if (columns_per_processor * num_procs != N) {
	  program_abort(argv[0], "Number of processors does not divide N, I give up...\n");
  }
 
  // Declare matrices
  double* A = (double*)malloc(sizeof(double)*columns_per_processor*N);
  double* buffer = (double*)malloc(sizeof(double)*N);   // holds one column

  // Populate this processor's chunk of the matrix
  int i, j;
  struct cell local_cell = {0, 0};
  struct cell global_cell;
  double value;
  for (i=0; i<N; i++) {
	  for (j=0; j<columns_per_processor; j++) {
		  local_cell.row = i;
		  local_cell.col = j;
		  global_cell = local_to_global_indices(local_cell, rank, num_procs, N);
		  A[i*columns_per_processor + j] = calculate_cell_value(global_cell.row, global_cell.col);
	  }
  }

  // MAIN ALGORITHM
  int k, local_col; 

#ifdef DEBUG
  int rank_to_test;
  rank_to_test = 0;
#endif

  for (k=0; k < N-1; k++) {
	  int k_owner, local_k;
	  k_owner = owner_global_column(k, num_procs, N);
	  local_k = global_to_local_column(k, num_procs, N);
	  // update k-th column if I have it
	  if (k_owner == rank) {
		#ifdef DEBUG
		  printf("process %d here; k=%d and I own that column ...\n", rank, k);
		  printf("updating %dth column...\n", k);
		  if (rank == rank_to_test) {
			  printf("before:\n");
			  print_matrix(A, N, columns_per_processor);
		  }
		#endif
		  for (i=k+1; i<N; i++) {
			  local_col = global_to_local_column(i, num_procs, N); 
			  A[local_col*columns_per_processor + k] = A[local_col*columns_per_processor + k] / A[k*columns_per_processor + k];
		  }
		#ifdef DEBUG
		  if (rank == rank_to_test) {
			  printf("after:\n");
			  print_matrix(A, N, columns_per_processor);
		  }
		#endif

		  // Copy kth column to buffer for broadcast
		  for (i=0; i<N; i++) {
	 		  buffer[i] = A[i*columns_per_processor + local_k];
		  }
	  } // end if (k_owner == rank)

	// broadcast to all
	MPI_Bcast(buffer, N, MPI_DOUBLE, rank, MPI_COMM_WORLD);
	#ifdef DEBUG
		printf("rank %d here; my buffer looks like this:\n", rank);
		print_matrix(buffer, N, 1);
	#endif
	printf("*****\n\n");

	  // update any columns I have that come after k
	#ifdef DEBUG
	  printf("updating other columns now ...\n");
	#endif
	  for (j=k+1; j<N; j++) {
		  for (i=k+1; i<N; i++) {
			  if (owner_global_column(i, num_procs, N) == rank) {
			#ifdef DEBUG
				  if (rank == rank_to_test) {
					  printf("before:\n");
					  print_matrix(A, N, columns_per_processor);
				  }
			#endif
			          local_col = global_to_local_column(i, num_procs, N);
				  A[local_col*columns_per_processor + j] -= A[local_col*columns_per_processor + k] * buffer[local_k*columns_per_processor + j];
				  // TODO that A[k*cols + j] should check whether we own column k or not;
				  // IOW A[k*cols + j] is A[k][j] which this process may not own.
			  }
		  }
	  }
	#ifdef DEBUG
	  if (rank == rank_to_test) {
		  printf("after:\n");
		  print_matrix(A, N, columns_per_processor);
	  }
	#endif
  }

  /*
  // Do the Gaussian elimination
  for (k=0; k <N-1; k++) {
    // Update the k-th column
    for (i=k+1; i < N; i++) {
      A[i*N+k] = + A[i*N+k] / A[k*N+k];
    }
    for (j=k+1; j < N; j++) {
      for (i = k+1; i < N; i++) {
        A[i*N+j] -= A[i*N+k] * A[k*N+j];
      }
    }
  }

  // Start the timer
  double start_time;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {  
    start_time = MPI_Wtime();
  }
  */

  // Clean-up
  MPI_Finalize();

  return 0;
}
