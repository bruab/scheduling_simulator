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
  int k, local_j; 

  // Start the timer
  double start_time, idle_start_time, idle_time;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {  
    start_time = MPI_Wtime();
  }

  for (k=0; k < N-1; k++) {
	  int k_owner, local_k;
	  k_owner = owner_global_column(k, num_procs, N);
	  local_k = global_to_local_column(k, num_procs, N);
	  // update k-th column if I have it
	  if (k_owner == rank) {
		  for (i=k+1; i<N; i++) {
			  A[i*columns_per_processor + local_k] = A[i*columns_per_processor + local_k] / A[k*columns_per_processor + local_k];
		  }

		  // Copy kth column to buffer for broadcast
		  for (i=0; i<N; i++) {
	 		  buffer[i] = A[i*columns_per_processor + local_k];
		  }
	  } // end if (k_owner == rank)

	// broadcast to all
	MPI_Bcast(buffer, N, MPI_DOUBLE, k_owner, MPI_COMM_WORLD);


	  // update any columns I have that come after k
	  MPI_Barrier(MPI_COMM_WORLD);
	  idle_start_time = MPI_Wtime();
	  double Aik;
	  for (j=k+1; j<N; j++) {  // j is the global column
		  for (i=k+1; i<N; i++) {  // i is the row
			  if (owner_global_column(j, num_procs, N) == rank) {
			          local_j = global_to_local_column(j, num_procs, N);
				  if (k_owner == rank) {
					  Aik = A[i*columns_per_processor + local_k];
				  } else {

					  Aik = buffer[i];
				  }

				  A[i*columns_per_processor + local_j] -= Aik * A[k*columns_per_processor + local_j];
			  }
		  }
	  }
	  MPI_Barrier(MPI_COMM_WORLD);
	  idle_time += MPI_Wtime() - idle_start_time;
  } // end k loop

  // Print out string message and wall-clock time if the broadcast was
  // successful
  MPI_Barrier(MPI_COMM_WORLD);
  double total_idle_time;
  MPI_Reduce(&idle_time, &total_idle_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (0 == rank) {
    fprintf(stdout,"Wall clock time: %.3lf\n",MPI_Wtime() - start_time);
    printf("Idle time is %f\n", total_idle_time);
  }

  // Validate the results
  double Aij;
  for (i=0; i < N; i++) {
    for (j=0; j < N; j++) {
	    if (owner_global_column(j, num_procs, N) == rank) {
		    local_j = global_to_local_column(j, num_procs, N);
		    Aij = A[i*columns_per_processor + local_j];
		    if ((i > j) && (Aij != 1.0)) {
        		fprintf(stderr,"** MISMATCH  ** A(%d,%d) = %.2lf but should be %.2lf\n", i,j, Aij, 1.0);
		    }
		    if ((i <= j) && (Aij != (double)(i+1)*(i+1))) {
        		fprintf(stderr,"** MISMATCH  ** A(%d,%d) = %.2lf but should be %.2lf\n",i,j, Aij, (double)(i+1)*(i+1));
		    }
	    }
    }
  }

  /*
  */

  // Clean-up
  MPI_Finalize();

  return 0;
}
