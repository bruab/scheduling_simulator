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

int blocks_per_row,N,block_size;

///////////////////////////
////// Main function //////
///////////////////////////

int main(int argc, char *argv[])
{
  int i,j,k;

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
  double* A = (double*)malloc(sizeof(double)*block_size*block_size);
  double* B = (double*)malloc(sizeof(double)*block_size*block_size);
  double* C = (double*)malloc(sizeof(double)*block_size*block_size);
  double* bufferA = (double*)malloc(sizeof(double)*block_size*block_size);
  double* bufferB = (double*)malloc(sizeof(double)*block_size*block_size);

  // Determine row and column mates; create communicators
  int row_mates[blocks_per_row];
  int col_mates[blocks_per_row];

  // Calculate row mates
  int factor, index, mate_rank;
  factor = rank / blocks_per_row;
  for (i=0; i<blocks_per_row; i++) {
	  mate_rank = factor * blocks_per_row + i;
		  row_mates[i] = mate_rank;
  }

  // Verify row mates
  #ifdef DEBUG
  //printf("process %d here. these are my row mates: ", rank);
  //for (i=0; i<blocks_per_row; i++) {
	  //printf("%d, ", row_mates[i]);
  //}
  //printf("\n");
  #endif

  // Calculate column mates
  int lowest_col_mate;
  lowest_col_mate = rank % blocks_per_row;
  for (i=0; i<blocks_per_row; i++) {
	  mate_rank = lowest_col_mate + i * blocks_per_row;
		  col_mates[i] = mate_rank;
  }

  // Verify column mates
  #ifdef DEBUG
  //printf("process %d here. these are my column mates: ", rank);
  //for (i=0; i<blocks_per_row; i++) {
	  //printf("%d, ", col_mates[i]);
  //}
  //printf("\n");
  #endif

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

  // Local function to multiply matrices
  void MatrixMultiply(double myA[block_size*block_size], double myB[block_size*block_size]) {
	for (i=0; i<block_size; i++) {
		for (k=0; k<block_size; k++) {
			for (j=0; j<block_size; j++) {
					C[i*block_size + j] = C[i*block_size + j] + (myA[i*block_size + k]) * (myB[k*block_size + j]);
			}
		}
	}
  }

  // Local function to print matrix for debugging
  void PrintMatrix(double mat[block_size*block_size]) {
	  for (i=0; i<block_size; i++) {
		  for (j=0; j<block_size; j++) {
			  printf("%f ", mat[i*block_size + j]);
		  }
		  printf("\n");
	  }
  }

  // Just testing ...
  MatrixMultiply(A, B);
#ifdef DEBUG
  /*
  printf("Process %d here. Matrix A:\n", rank);
  PrintMatrix(A);
  printf("******\nMatrix B:\n");
  PrintMatrix(B);
  printf("******\nMatrix C:\n");
  PrintMatrix(C);
  printf("\n\n");
  */
#endif

  // test broadcast ...
  /*
  if (rank == 1) {
		for (i=0; i<block_size; i++) {
			for (j=0; j<block_size; j++) {
				bufferB[i*block_size + j] = B[i*block_size + j];
			}
		}
  }

	#ifdef DEBUG
	  if (rank == 1) {
		  printf("process 1 here; my B is:\n");
		  PrintMatrix(B);
	  }
	#endif
  //MPI_Bcast(bufferB, block_size*block_size, MPI_FLOAT, 1, col_comm_id);
  MPI_Bcast(&B, block_size*block_size, MPI_FLOAT, 1, MPI_COMM_WORLD);
	#ifdef DEBUG
	  printf("process %d here, post broadcast; my B is:\n", rank);
	  PrintMatrix(B);
	#endif
	*/

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
		
	row_sender_rank = (rank / blocks_per_row) * blocks_per_row + k;
	MPI_Bcast(&bufferA, block_size*block_size, MPI_FLOAT, row_sender_rank, row_comm_id);

	  // Every block in row k sends B to its column mates
	  if (this_block_row == k) {
		// first copy contents of B to bufferB
		for (i=0; i<block_size; i++) {
			for (j=0; j<block_size; j++) {
				bufferB[i*block_size + j] = B[i*block_size + j];
			}
		}
	  }

	  col_sender_rank = rank % blocks_per_row + k * blocks_per_row;
	#ifdef DEBUG
	  if (rank == 0) {
		printf("process %d here, pre-broadcast. k=%d and my bufferB looks like this:\n", rank, k);
		for (i=0; i<block_size; i++) {
			for (j=0; j<block_size; j++) {
				printf("%f ", bufferB[i*block_size + j]);
			}
			printf("\n");
		}
	  }
	#endif
	  MPI_Bcast(&bufferB, block_size*block_size, MPI_FLOAT, col_sender_rank, col_comm_id);
	#ifdef DEBUG
	  if (rank == 0) {
		  printf("colsenderrank is %d\n", col_sender_rank);
		printf("process %d here, post-broadcast. k=%d and my bufferB looks like this:\n", rank, k);
		for (i=0; i<block_size; i++) {
			for (j=0; j<block_size; j++) {
				printf("%f ", bufferB[i*block_size + j]);
			}
			printf("\n");
		}
	  }
	#endif

	  // Multiply Matrix blocks
	  //
  }


  // Sum elements of matrix C
  double sum;
  sum = 0.0;
  for (i=0; i<block_size; i++) {
  	for (j=0; j<block_size; j++) {
		sum = sum + C[i*block_size + j];
	}
  }

  printf("process %d here, i got a sum of %f\n", rank, sum);

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

  // Not sure what this is here for, but afraid to delete just yet TODO
  MPI_Barrier(MPI_COMM_WORLD);

  // Clean-up
  MPI_Finalize();

  return 0;
}
