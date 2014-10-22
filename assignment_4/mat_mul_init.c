#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>

#ifndef VERSION
#define VERSION 1
#endif


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
  int i,j,N;
  
  // Get N from command line arg
  // TODO not sure why it's argv[3]; also getting compile warning about cast to int
  N = atoi(argv[3]);

  MPI_Init(&argc, &argv);

  // Determine rank and number of processes
  int num_procs;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  // Abort if num_procs is not a perfect squre
  // TODO

  // Abort if num_procs doesn't divide N
  // TODO

  // Declare matrices
  // TODO

  // Start the timer
  double start_time;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {  
    start_time = MPI_Wtime();
  }

  /////////////////////////////////////////////////////////////////////////////
  ///////////s i///////////////// TO IMPLEMENT: BEGIN ////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  
#if VERSION == 0 //bcast_default
  printf("this is processor %d...  ", rank);
  printf("you chose version 0 ...  ");
  printf("N is %d\n", N);

#elif VERSION == 1 //bcast_naive
  printf("you chose version 1");
#endif

  
  /////////////////////////////////////////////////////////////////////////////
  ///////////////////////////// TO IMPLEMENT: END /////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
 
  // Print out string message and wall-clock time if the broadcast was
  // successful
  MPI_Barrier(MPI_COMM_WORLD);

  // Clean-up
  MPI_Finalize();

  return 0;
}
