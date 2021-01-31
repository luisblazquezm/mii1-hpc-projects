#include "utils.h"
#include "plate.h"
#include "logging.h"
#include "simulation.h"
#include "paralellUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>

void * calculateSubMatrix(void* arg);

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

void paralellSimulation(int numIterations, int numRows, int numCols, int numThreads, DivisionStructure division, TimeResultRef timeResult){
	Value newValue;
	int iteration, i, j;
	pthread_barrier_t barrier; 
	pthread_barrierattr_t attr;
	pthread_t * threadIds = NULL;
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

	// create thread id containers
	doLog(DEBUG, "creating thread id array");
	threadIds = (pthread_t *) calloc(numThreads,sizeof(pthread_t));
	if(threadIds == NULL){
		doLog(ERROR, "Unable to reserve memory to the thread id array, aborting the execution");
		exit(IO_ERROR_CODE);
	}

	// create and initialize barrier
	doLog(DEBUG, "creating barrier");
	if(0 != pthread_barrier_init(&barrier, &attr, numThreads-1)){
		doLog(ERROR, "Unable to create barrier for threads");
		exit(IO_ERROR_CODE);
	}

	// define matrix divisions
	subtasks = buildMatrixDivisions(numRows, numCols, numThreads, division);

	// update plate values
	doLog(DEBUG, "starting threads");
	for(i=0; i < numThreads; i++){
		// assign plates
		subtasks[i].barrier = barrier;
		subtasks[i].oldPlate = oldPlate;
		subtasks[i].timeResult = timeResult;
		subtasks[i].currentPlate = currentPlate;
		subtasks[i].numIterations = numIterations;

		// invoke thread except for main thread (last)
		if(i < (numThreads-1)){
			pthread_create(&(threadIds[i]),  NULL,  calculateSubMatrix, (void *) &(subtasks[i]));
		}
	}

	// run application for main thread
	calculateSubMatrix((void *) &(subtasks[numThreads-1]));

	// wait for threads
	doLog(DEBUG, "waiting for threads to finish");
	for(i=0; i < (numThreads-1); i++){
		pthread_join(threadIds[i], NULL);
	}

	// measure total time
	timeResult->totaltime = measureTime();

	// measure total time
	timeResult->numThreads = numThreads;
	timeResult->totaltime = measureTime() - timeResult->totaltime;


	if(LOG_LEVEL == DEBUG){
		printPlate(oldPlate);
	}

	doLog(DEBUG, "ending paralell simulation");
}

void waitForFinishCalculation(MatrixSubTaskRef threadArgs){
	doLog(DEBUG, "waiting for threads");

	int ret = pthread_barrier_wait(&(threadArgs->barrier));
	if( (0 != ret) && (PTHREAD_BARRIER_SERIAL_THREAD != ret) ){
		doLog(ERROR, "Unable to wait for barrier for threads");
		exit(IO_ERROR_CODE);
	}

	doLog(DEBUG, "finished wait for threads");
}

void * calculateSubMatrix(void* arg){
	Value newValue;
	int i, j, iteration;
	PlateRef switchPlate;
	long iterationTime = 0;
	MatrixSubTaskRef threadArgs = (MatrixSubTaskRef) arg;

	doLog(INFO, "calculating submatrix");

	// update num iterations times
	for(iteration=0; iteration < threadArgs->numIterations; iteration++){
		doLog(INFO, "new iteration");

		// measure iteration time
		iterationTime = measureTime();

		for(i=threadArgs->fromRow; i <= threadArgs->toRow; i++){
			for(j=threadArgs->fromCol; j <= threadArgs->toCol; j++){
				if(isHole(threadArgs->oldPlate, i, j)){
					setPlateValue(threadArgs->currentPlate, i, j, HOLE_VALUE);
					continue;
				}
				newValue = calculateNewTemperature(threadArgs->oldPlate, i, j);
				setPlateValue(threadArgs->currentPlate, i, j, newValue);
			}
		}

		// switch plates
		doLog(DEBUG, "switching plates");
		switchPlate = threadArgs->oldPlate;
		threadArgs->oldPlate = threadArgs->currentPlate;
		threadArgs->currentPlate = switchPlate;

		// measure iteration time
		iterationTime = measureTime() - iterationTime;
		threadArgs->timeResult->times[iteration] = iterationTime;

		// wait for all threads to start the calculation (between iterations)
		if (iteration < (threadArgs->numIterations-1)){
			waitForFinishCalculation(threadArgs);
		}
		
		doLog(INFO, "finished iteration");
	}

}
