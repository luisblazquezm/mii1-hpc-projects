#include "csv.h"
#include "utils.h"
#include "logging.h"
#include "paralellUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void addCsvEntry(int simulationType, int divisionType, int numIterations, int numThreads, int numRows, int numCols, TimeResult result){
	int i;
	FILE * f = NULL;
	double meanIteration = 0;
	long totalIterationsTime = 0;

 	// calculate mean iteration and total iteration time
	for(i=0; i< NUM_ITERATIONS; i++){
		totalIterationsTime += result.times[i];
	}
	meanIteration = totalIterationsTime / NUM_ITERATIONS;

	// write header if not exists
	if(access( CSV_FILE, F_OK ) == -1 ){
		// open file
		f = fopen(CSV_FILE,"w+");
		if(NULL == f){
			doLog(ERROR, "Unable to write data to csv. Finishing execution.");
			exit(IO_ERROR_CODE);
		}
		// write header
		fprintf(f,"TIME;ENTRY_TYPE;SIMULATION_TYPE;DIVISION_TYPE;THREADS;ITERATIONS;ROWS;COLUMNS;TOTAL_TIME;TOTAL_CALCULATION_TIME;MEAN_ITERATION_TIME");
	}else{
		// open file
		f = fopen(CSV_FILE,"a+");
		if(NULL == f){
			doLog(ERROR, "Unable to write data to csv. Finishing execution.");
			exit(IO_ERROR_CODE);
		}
	}

	// print registry
	char simulationTypeChar = (simulationType == SIMULATION_SERIAL) ? 'S' : 'P'; 
	char divisionTypeChar = (divisionType == DIVISION_BY_ROWS) ? 'R' : ( (divisionType == DIVISION_BY_COLS)  ? 'C' : 'M' ); 
    fprintf(f, "\n%s;%s;%c;%c;%d;%d;%d;%d;%d;%f;%", getDateAndTime(), ENTRY_TYPE, simulationTypeChar, divisionTypeChar, numThreads, numIterations, numRows, numCols, result.totaltime, totalIterationsTime, meanIteration);

    // close file
    fclose(f);
}