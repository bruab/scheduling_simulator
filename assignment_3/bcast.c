#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <math.h>

#define RAND_SEED 842270

#define NUM_BYTES   10000000

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
    fprintf(stderr,"              -platform <XML platform file> -hostfile <host file> %s [-c <chunk size>] [-s <message string>]\n",exec_name);
    fprintf(stderr,"MPIRUN arguments:\n");
    fprintf(stderr,"\t<num processes>: number of MPI processes\n");
    fprintf(stderr,"\t<XML platform file>: a simgrid platform description file\n");
    fprintf(stderr,"\t<host file>: MPI host file with host names from the XML platform file\n");
    fprintf(stderr,"PROGRAM arguments:\n");
    fprintf(stderr,"\t[-c <chunk size>]: chunk size in bytes for message splitting\n");
    fprintf(stderr,"\t[-c <message string]>: arbitrary text to be printed out (no spaces)\n");
    fprintf(stderr,"\n");
  }
}

///////////////////////////
////// Main function //////
///////////////////////////

int main(int argc, char *argv[])
{
  int i,j;
  // Parse command-line arguments (not using getopt because not thread-safe
  // and annoying anyway). The code below ignores extraneous command-line
  // arguments, which is lame, but we're not in the business of developing
  // a cool thread-safe command-line argument parser.

  MPI_Init(&argc, &argv);

  // Chunk size optional argument
  int chunk_size = NUM_BYTES;
  for (i=1; i < argc; i++) {
    if (!strcmp(argv[i],"-c")) {
      if ((i+1 >= argc) || (sscanf(argv[i+1],"%d",&chunk_size) != 1)) {
        program_abort(argv[0],"Invalid <chunk size> argument\n");
      }
    }
  }
  
  // Message string optional argument
  char *message_string = "";
  for (i=1; i < argc; i++) {
    if (!strcmp(argv[i],"-s")) {
      if ((i+1 >= argc)) {
        program_abort(argv[0],"Invalid <chunk size> argument\n");
      } else {
	message_string = strdup(argv[i+1]);
      }
    }
  }

  // Determine rank and number of processes
  int num_procs;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  // Allocate buffer
  int checksum;
  char *buffer;
  
  if ((buffer = malloc(sizeof(char) * NUM_BYTES)) == NULL) {
    program_abort(argv[0],"Out of memory!");
  }

  // On rank 0 fill the buffer with random data 
  if (0 == rank) { 
    checksum = 0;
    srandom(RAND_SEED);
    for (j = 0; j < NUM_BYTES; j++) {
      buffer[j] = (char) (random() % 256); 
      checksum += buffer[j];
    }
  }

  // Start the timer
  double start_time;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {  
    start_time = MPI_Wtime();
  }

  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////// TO IMPLEMENT: BEGIN ////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  
#if VERSION == 0 //bcast_default
  //MPI_Bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
  MPI_Bcast(buffer, NUM_BYTES, MPI_CHAR, 0, MPI_COMM_WORLD);

#elif VERSION == 1 //bcast_naive
  if (rank == 0) {
	  int n;
	for (n=1; n<num_procs; n++) {
		  MPI_Send(buffer, NUM_BYTES, MPI_CHAR, n, MPI_ANY_TAG, MPI_COMM_WORLD);
	   }

  } else {
	  MPI_Recv(buffer, NUM_BYTES, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

#elif VERSION == 2 //bcast_ring
  if (rank != 0) {
	  // Receive first
	  MPI_Recv(buffer, NUM_BYTES, MPI_CHAR, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  if (rank < num_procs-1) {
	  // Everyone but the last process sends
	  MPI_Send(buffer, NUM_BYTES, MPI_CHAR, rank+1, MPI_ANY_TAG, MPI_COMM_WORLD);
  }

#elif VERSION == 3 //bcast_ring_pipelined
  int num_chunks, chunk_index, current_address, remainder;
  remainder = NUM_BYTES % chunk_size;
  if (remainder == 0) {
	  num_chunks = NUM_BYTES / chunk_size;
  } else {
	  num_chunks = NUM_BYTES / chunk_size + 1;
  }
  for (chunk_index=0; chunk_index<num_chunks; chunk_index++) {
	  current_address = chunk_index*chunk_size;
	  if (chunk_index == num_chunks-1 && remainder != 0) {
		  chunk_size = remainder;
	  }
	  
	  if (rank != 0) {
		  // Receive first
		  MPI_Recv(&buffer[current_address], chunk_size, MPI_CHAR, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  }
	  if (rank < num_procs-1) {
		  // Everyone but the last process sends
			  // send next chunk
			  MPI_Send(&buffer[current_address], chunk_size, MPI_CHAR, rank+1, MPI_ANY_TAG, MPI_COMM_WORLD);
		  }
  }

#elif VERSION == 4 //bcast_ring_pipelined_isend
  MPI_Request request;
  MPI_Status status;
  int num_chunks, chunk_index, current_address, remainder;
  remainder = NUM_BYTES % chunk_size;
  if (remainder == 0) {
	  num_chunks = NUM_BYTES / chunk_size;
  } else {
	  num_chunks = NUM_BYTES / chunk_size + 1;
  }
  for (chunk_index=0; chunk_index<num_chunks; chunk_index++) {
	  current_address = chunk_index*chunk_size;
	  if (chunk_index == num_chunks-1 && remainder != 0) {
		  chunk_size = remainder;
	  }
	  
	  if (rank < num_procs-1) {
		  // Everyone but the last process sends
			  // send next chunk
			  MPI_Isend(&buffer[current_address], chunk_size, MPI_CHAR, rank+1, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
		  }
	  if (rank != 0) {
		  // Receive first
		  MPI_Recv(&buffer[current_address], chunk_size, MPI_CHAR, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		  MPI_Wait(&request, &status);
	  }
  }


#endif

  
  /////////////////////////////////////////////////////////////////////////////
  ///////////////////////////// TO IMPLEMENT: END /////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
 
  // All processes send checksums back to the root, which checks for consistency
  char all_ok = 1;
  if (0 == rank) {
    for (j = 1; j < num_procs; j++) {
      int received_checksum;
      MPI_Recv(&received_checksum, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // Print a single message in case of a mismatch, but continue
      // receiving other checksums to ensure that all processes
      // reach the MPI_Finalize()
      if ((all_ok == 1) && (checksum != received_checksum)) {
	fprintf(stderr,"\t** Non-matching checksum! **\n");
	all_ok = 0;
	break;
      }
    }
  } else {
    int checksum=0;
    for (j = 0; j < NUM_BYTES; j++) {
      checksum += buffer[j];
    }
    MPI_Send(&checksum, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
  }

  // Print out string message and wall-clock time if the broadcast was
  // successful
  MPI_Barrier(MPI_COMM_WORLD);
  if ((0 == rank) && (all_ok == 1)) {
    fprintf(stdout,"%s%.3lf\n",message_string,MPI_Wtime() - start_time);
  }

  // Clean-up
  free(buffer);
  MPI_Finalize();

  return 0;
}
