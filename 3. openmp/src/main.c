#include <stdio.h>
#include <stdlib.h>
#include "csv.h"
#include "utils.h"
#include "simulation.h"

int main(int argc, char *argv[]){
    putchar('\n');

    // assign values by default
    TimeResult timeResult;
    int numRows = NUM_ROWS;
    int numCols = NUM_COLS;
    int numThreads = NUM_THREADS;
    int numIterations = NUM_ITERATIONS;
    int selectedSimulation = SELECTED_SIMULATION;
	DivisionStructure division = DIVISION_BY_ROWS;

    if(argc!=7){
        printf("\nNo Extra Command Line Argument Passed Other Than Program Name. Using default parameters\n"); 
    }else { 
        // initial configuration parameters 
		if (atoi(argv[1]) != -1)
	    	numRows = atoi(argv[1]);
	    if (atoi(argv[2]) != -1)
	    	numCols = atoi(argv[2]);
	    if (atoi(argv[3]) != -1)
	    	numThreads = atoi(argv[3]);
	    if (atoi(argv[4]) != -1)
	    	numIterations = atoi(argv[4]);
	    if (atoi(argv[5]) != -1)
	    	selectedSimulation = atoi(argv[5]);
        if (atoi(argv[6]) != -1)
            division = atoi(argv[6]);
    } 

    // choose between serial and parallel simulation
	if(SIMULATION_SERIAL == selectedSimulation){
		serialSimulation(numIterations, numRows, numCols, &timeResult);
	}else{
		paralellSimulation(numIterations, numRows, numCols, numThreads, division, &timeResult);
	}

	// Add entry CSV
	addCsvEntry(selectedSimulation, division, numIterations, numThreads, numRows, numCols, timeResult);
}