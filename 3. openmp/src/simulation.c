#include "utils.h"
#include "plate.h"
#include "logging.h"
#include "simulation.h"
#include "paralellUtils.h"

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>

long measureTime(){
	struct timeval time;
    gettimeofday(&time, NULL);
    long micros = (time.tv_sec  * 1000000) + time.tv_usec;
    return micros;
}

void serialSimulation(int numIterations, int numRows, int numCols, TimeResultRef timeResult){
	Value newValue;
	int iteration, i, j;
	PlateRef oldPlate = NULL, currentPlate = NULL, switchPlate = NULL;

	doLog(DEBUG, "starting serial simulation");

	// define holes
	Hole holes [1];
	initializeHole(&(holes[0]),3,6,3,6);

	// create and initialize plates
	doLog(DEBUG, "creating plates structures");
	oldPlate = createPlate(numRows, numCols);
	currentPlate = createPlate(numRows, numCols);
	doLog(DEBUG, "initlializing plate structure");
	initializePlate(oldPlate, 1, holes);

	if(LOG_LEVEL == DEBUG){
		printPlate(oldPlate);
	}

	// measure total time
	timeResult->totaltime = measureTime();

	// update num iterations times
	for(iteration=0; iteration<numIterations; iteration++){
		doLog(INFO, "new iteration");

		// measure iteration time
		timeResult->times[iteration] = measureTime();

		// update plate values
		for(i=0; i < currentPlate->rows; i++){
			for(j=0; j < currentPlate->cols; j++){
				if(isHole(oldPlate, i, j)){
					setPlateValue(currentPlate, i, j, HOLE_VALUE);
					continue;
				}
				newValue = calculateNewTemperature(oldPlate, i, j);
				setPlateValue(currentPlate, i, j, newValue);
			}
		}
		
		// switch plates
		switchPlate = oldPlate;
		oldPlate = currentPlate;
		currentPlate = switchPlate;

		if(LOG_LEVEL == DEBUG){
			printPlate(oldPlate);
		}

		// measure iteration time
		timeResult->times[iteration] = measureTime() - timeResult->times[iteration];
	}

	// measure total time
	timeResult->numThreads = 1;
	timeResult->totaltime = measureTime() - timeResult->totaltime;

	doLog(DEBUG, "ending serial simulation");
}

void loopBehaviour(PlateRef oldPlate, PlateRef currentPlate, MatrixSubTask subtask){
	int i,j;
	Value newValue;
	for(i=subtask.fromRow; i <= subtask.toRow; i++){
		for(j=subtask.fromCol; j <= subtask.toCol; j++){
			if(isHole(oldPlate, i, j)){
				setPlateValue(currentPlate, i, j, HOLE_VALUE);
				continue;
			}
			newValue = calculateNewTemperature(oldPlate, i, j);
			setPlateValue(currentPlate, i, j, newValue);
		}
	}
}

void paralellSimulation(int numIterations, int numRows, int numCols, int numThreads, DivisionStructure division, TimeResultRef timeResult){
	int iteration, i, j, threadId;
	MatrixSubTaskRef subtasks = NULL;
	PlateRef oldPlate = NULL, currentPlate = NULL, switchPlate = NULL;

	doLog(DEBUG, "starting paralell simulation");

	// define holes
	Hole holes [1];
	initializeHole(&(holes[0]),3,6,3,6);

	// create and initialize plates
	doLog(DEBUG, "creating plates structures");
	oldPlate = createPlate(numRows, numCols);
	currentPlate = createPlate(numRows, numCols);
	doLog(DEBUG, "initlializing plate structure");
	initializePlate(oldPlate, 1, holes);

	if(LOG_LEVEL == DEBUG){
		printPlate(oldPlate);
	}

	// define matrix divisions
	subtasks = buildMatrixDivisions(numRows, numCols, numThreads, division);

	// measure total time
	timeResult->totaltime = measureTime();


	#pragma omp parallel num_threads(numThreads)
	{
		// update num iterations times
		for(iteration=0; iteration<numIterations; iteration++){

			doLog(INFO, "new iteration");

			// measure iteration time
			timeResult->times[iteration] = measureTime();

			// update plate values
			#pragma omp for
			for(i=0; i < numThreads; i++){
				loopBehaviour(oldPlate, currentPlate, subtasks[i]);
			}
		
			// switch plates
			doLog(DEBUG, "switching plates");
			switchPlate = oldPlate;
			oldPlate = currentPlate;
			currentPlate = switchPlate;

			doLog(INFO, "finished iteration");
			if(LOG_LEVEL == DEBUG){
				printPlate(oldPlate);
			}

			// measure iteration time
			timeResult->times[iteration] = measureTime() - timeResult->times[iteration];
		
		}
	}
	
	// measure total time
	timeResult->numThreads = numThreads;
	timeResult->totaltime = measureTime() - timeResult->totaltime;

	doLog(DEBUG, "ending paralell simulation");
}

