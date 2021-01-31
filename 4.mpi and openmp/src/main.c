#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "csv.h"
#include "logging.h"
#include "simulation.h"

int main(int argc, char *argv[]){
    // assign values by default
    TimeResult timeResult;
    int numRows = NUM_ROWS;
    int numCols = NUM_COLS;
    int numThreads = NUM_THREADS;
    int numIterations = NUM_ITERATIONS;
    DivisionStructure division = DIVISION_BY_ROWS;
	int processId, numProcesses;

    if(argc!=6){
        printf("\nNo Extra Command Line Argument Passed Other Than Program Name. Using default parameters\n"); 
    }else { 
		if (atoi(argv[1]) != -1)
	    	numRows = atoi(argv[1]);
	    if (atoi(argv[2]) != -1)
	    	numCols = atoi(argv[2]);
	    if (atoi(argv[3]) != -1)
	    	numIterations = atoi(argv[3]);
        if (atoi(argv[4]) != -1)
            numThreads = atoi(argv[4]);
        if (atoi(argv[5]) != -1)
            division = atoi(argv[5]);
    } 

	// Init MPI
    MPI_Init(&argc, &argv);

    // get process id and num tasks
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    // run simulation
    simulation(processId, numProcesses, numThreads, division, numIterations, numRows, numCols, &timeResult);

    if(ISMASTER(processId)){
		addCsvEntry(1, division, numIterations, numProcesses, numThreads, numRows, numCols, timeResult);
	}

	MPI_Finalize();
}