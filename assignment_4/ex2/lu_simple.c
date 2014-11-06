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

void print_matrix(double *A,int n) {
  int i,j;

  for (i=0; i < n; i++) {
    printf("  ");
    for (j=0; j < n; j++) {
      printf("%2.0lf ",A[i*n+j]);
    }
    printf("\n");
  }
  printf("-------------------------------\n");
  return;
}

int owner_global_column(int col_num, int procs, int N) {
	return col_num * procs / N;
}

struct cell local_to_global(struct cell local_cell, int rank, int np, int N) {
	int global_row, global_col;
	global_row = local_cell.row;
	global_col = local_cell.col + rank * N / np;
	struct cell global_cell = {global_row, global_col};
	return global_cell;
}

struct cell global_to_local(struct cell global_cell, int np, int N) {
	int local_row, local_col;
	local_row = global_cell.row;
	local_col = global_cell.col % (N / np);
	struct cell local_cell = {local_row, local_col};
	return local_cell;
}


///////////////////////////
////// Main function //////
///////////////////////////

int main(int argc, char *argv[])
{
  // Get N from command line arg
  // TODO segfaults if i forget to pass arg, so ghetto :(
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

  // Testing ...
#ifdef DEBUG 
  if (rank == 0) {
	  int jj, owner;
	  for (jj=0; jj<N; jj++) {
		  owner = owner_global_column(jj, num_procs, N);
		  printf("column %d belongs to process %d\n", jj, owner);
	  }
  }
#endif

#ifdef DEBUG 
  if (rank == 0) {
	  // Test calculations with dummy values
	  int test_num_procs, test_rank, test_N;
	  test_num_procs = 4;
	  test_rank = 2;
	  test_N = 12;
	  struct cell test_local_cell1 = {7, 0};
	  struct cell test_global_cell1 = local_to_global(test_local_cell1, test_rank, test_num_procs, test_N);
	  printf("local cell is (7, 0); global cell is (%d, %d)\n", test_global_cell1.row, test_global_cell1.col);

	  struct cell test_global_cell2 = {10, 4};
	  struct cell test_local_cell2 = global_to_local(test_global_cell2, test_num_procs, test_N);
	  printf("global cell is (10, 4); local cell is (%d, %d)\n", test_local_cell2.row, test_local_cell2.col);

  }
#endif
  /*
  // Determine row and column mates
  int row_mates[blocks_per_row];
  int col_mates[blocks_per_row];

  // Calculate row mates
  int factor, index, mate_rank;
  factor = rank / blocks_per_row;
  for (i=0; i<blocks_per_row; i++) {
	  mate_rank = factor * blocks_per_row + i;
		  row_mates[i] = mate_rank;
  }

  // Calculate column mates
  int lowest_col_mate;
  lowest_col_mate = rank % blocks_per_row;
  for (i=0; i<blocks_per_row; i++) {
	  mate_rank = lowest_col_mate + i * blocks_per_row;
		  col_mates[i] = mate_rank;
  }

  // Create MPI groups and communicators
  MPI_Group world_group_id;
  MPI_Comm row_comm_id;
  MPI_Group row_group_id;
  MPI_Comm col_comm_id;
  MPI_Group col_group_id;

  MPI_Comm_group ( MPI_COMM_WORLD, &world_group_id );

  // Rows
  MPI_Group_incl ( world_group_id, blocks_per_row, row_mates, &row_group_id );
  MPI_Comm_create ( MPI_COMM_WORLD, row_group_id, &row_comm_id );

  // Columns
  MPI_Group_incl ( world_group_id, blocks_per_row, col_mates, &col_group_id );
  MPI_Comm_create ( MPI_COMM_WORLD, col_group_id, &col_comm_id );

  // Fill matrices
  for (i=0; i<block_size; i++) {
  	for (j=0; j<block_size; j++) {
		A[i*block_size + j] = (rank / blocks_per_row) * block_size + i;
		B[i*block_size + j] = (this_block_row + this_block_col) * block_size + i + j;
		C[i*block_size + j] = 0.0;
		bufferA[i*block_size + j] = 0.0;
		bufferB[i*block_size + j] = 0.0;
	}
  }

  // Broadcast and multiply
  int row_sender_rank, col_sender_rank;
  for (k=0; k < blocks_per_row; k++) {
	  // Every block in column k sends A to its row mates
	  if (this_block_col == k) {
		// first copy contents of A to bufferA
		for (i=0; i<block_size; i++) {
			for (j=0; j<block_size; j++) {
				bufferA[i*block_size + j] = A[i*block_size + j];
			}
		}
	  }
		
	MPI_Bcast(bufferA, block_size*block_size, MPI_DOUBLE, k, row_comm_id);

	  // Every block in row k sends B to its column mates
	  if (this_block_row == k) {
		// first copy contents of B to bufferB
		for (i=0; i<block_size; i++) {
			for (j=0; j<block_size; j++) {
				bufferB[i*block_size + j] = B[i*block_size + j];
			}
		}
	  }

	  MPI_Bcast(bufferB, block_size*block_size, MPI_DOUBLE, k, col_comm_id);
	  
	  // Multiply Matrix blocks
	  if (this_block_row == k && this_block_col == k) {
	  	MatrixMultiply(A, B, C, block_size);
	  } else if (this_block_row == k) {
	  	MatrixMultiply(bufferA, B, C, block_size);
	  } else if (this_block_col == k) {
	  	MatrixMultiply(A, bufferB, C, block_size);
	  } else {
	  	MatrixMultiply(bufferA, bufferB, C, block_size);
	  }
  }

  // Sum elements of matrix C
  double sum;
  sum = 0.0;
  for (i=0; i<block_size; i++) {
  	for (j=0; j<block_size; j++) {
		sum = sum + C[i*block_size + j];
	}
  }

  // Collect all sums
  double total_sum;
  MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rank == 0) {
	  printf("actual sum is %f\n", total_sum);
  }

#ifdef DEBUG
  printf("process %d here, i got a sum of %f\n", rank, sum);
#endif


  // Calculate what total should be, only one process needs to do this
  if (rank == num_procs - 1) {
	  double c_sum;
	  c_sum = N*N*N*(N-1)*(N-1)/2;
	  printf("total sum should be %f\n", c_sum);
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
