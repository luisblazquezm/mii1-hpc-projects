#include <mpi.h>
#include <stdio.h>

#include "utils.h"
#include "plate.h"
#include "logging.h"
#include "simulation.h"
#include "paralellUtils.h"

void calculateNeighbours(int numRows, int numCols, int numVirtualCols, ProcessSubTask subtask, NeighboursRef neighbours);

void simulation(int processId, int numProcesses, int numIterations, int numRows, int numCols, TimeResultRef timeResult){
	Value newValue;
	Neighbours neighbours;
	ProcessSubTask subtask;
	int numVirutalRows, numVirutalCols, iteration, i, j;
	PlateRef currentPlate = NULL, oldPlate = NULL, switchPlate = NULL;
	
	// define holes
	Hole holes [1];
	initializeHole(&(holes[0]),3,6,3,6);

	// create and initialize plates
	doLog(DEBUG, "creating plates structures");
	oldPlate = createPlate(numRows, numCols);
	currentPlate = createPlate(numRows, numCols);
	doLog(DEBUG, "initlializing plate structure");
	initializePlate(oldPlate, 1, holes);

	if(LOG_LEVEL == DEBUG && ISMASTER(processId)){
		printPlate(oldPlate);
	}

	// calculate virutal matrix and process subtask 
	doLog(DEBUG, "calculating virtual matrix");
	calculateTaskMatrixDivisions(numProcesses, &numVirutalRows, &numVirutalCols);
	doLog(DEBUG, "calculating process subtask");
	calculateProcessSubtask(processId, numRows, numCols, numVirutalRows, numVirutalCols, &subtask);

	// calculate neighbours
	doLog(DEBUG, "calculating neighbours");
	calculateNeighbours(numRows, numCols, numVirutalCols, subtask, &neighbours);

	// wait for all to finish
	MPI_Barrier(MPI_COMM_WORLD);

	// perform simulation
	doLog(DEBUG, "start simulation");

	// measure total time
	if(ISMASTER(processId)){
		timeResult->totaltime = MPI_Wtime();
	}

	// update num iterations times
	for(iteration=0; iteration<numIterations; iteration++){
		doLog(INFO, "new iteration");

		// measure iteration time
		if(ISMASTER(processId)){
			timeResult->times[iteration] = MPI_Wtime();
		}

		doLog(DEBUG, "update matrix");

		// update plate values
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

		// send to neighbours
		doLog(DEBUG, "send update");
		sendUpdateToNeighbours(currentPlate, neighbours, iteration);

		// wait for update
		doLog(DEBUG, "recive update");
		recvUpdateFromNeighbours(currentPlate, neighbours, iteration);


		if(ISMASTER(processId)){
			// measure iteration time
			timeResult->times[iteration] =  MPI_Wtime() - timeResult->times[iteration];
		}

		// switch plates
		switchPlate = oldPlate;
		oldPlate = currentPlate;
		currentPlate = switchPlate;

		if(LOG_LEVEL == DEBUG && ISMASTER(processId)){
			printPlate(oldPlate);
		}
		
		doLog(DEBUG, "finished iteration");
	}

	// send to master process (or recive if is master)
	doLog(DEBUG, "joining matrix pieces");
	handleFinalResult(oldPlate, subtask, processId, numProcesses);

	// wait for all to finish
	MPI_Barrier(MPI_COMM_WORLD);

	// measure total time
	if(ISMASTER(processId)){
		timeResult->totaltime = MPI_Wtime();
	}

	if(LOG_LEVEL == DEBUG && ISMASTER(processId)){
		printPlate(oldPlate);
	}

	doLog(DEBUG, "finsihed simulation");
}


void calculateNeighbours(int numRows, int numCols, int numVirtualCols, ProcessSubTask subtask, NeighboursRef neighbours){

	// initialize
	int n, relativeX, relativeY;
	neighbours->num = 0;
	for(n=0; n < 4; n++){
		neighbours->id[n] = -1;

		neighbours->recv[n].toCol = 0;
		neighbours->recv[n].toRow = 0;
		neighbours->recv[n].fromCol = 0;
		neighbours->recv[n].fromRow = 0;

		neighbours->send[n].toCol = 0;	
		neighbours->send[n].toRow = 0;
		neighbours->send[n].fromCol = 0;
		neighbours->send[n].fromRow = 0;
	}

	// If there is left neighbour
	if (subtask.fromCol > 0) {

		// calculate neighbouh id
		relativeX = subtask.quadrantCoordX - 1;
		relativeY = subtask.quadrantCoordY;
		neighbours->id[neighbours->num] = (relativeY * numVirtualCols) + relativeX;

		// recv the previous column
		neighbours->recv[neighbours->num].fromCol = subtask.fromCol - 1;
		neighbours->recv[neighbours->num].toCol = subtask.fromCol - 1;

		neighbours->recv[neighbours->num].fromRow = subtask.fromRow;
		neighbours->recv[neighbours->num].toRow = subtask.toRow;		

		// send my first column
		neighbours->send[neighbours->num].fromCol = subtask.fromCol;
		neighbours->send[neighbours->num].toCol = subtask.fromCol;
	
		neighbours->send[neighbours->num].fromRow = subtask.fromRow;	
		neighbours->send[neighbours->num].toRow = subtask.toRow;
		
		// increase
		neighbours->num ++;
	}

	// If there is right neighbour
	if (subtask.toCol < (numCols-1)) {
		
		// calculate neighbouh id
		relativeX = subtask.quadrantCoordX + 1;
		relativeY = subtask.quadrantCoordY;
		neighbours->id[neighbours->num] = (relativeY * numVirtualCols) + relativeX;

		// recv the next column
		neighbours->recv[neighbours->num].fromCol = subtask.toCol + 1;
		neighbours->recv[neighbours->num].toCol = subtask.toCol + 1;

		neighbours->recv[neighbours->num].fromRow = subtask.fromRow;
		neighbours->recv[neighbours->num].toRow = subtask.toRow;		

		// send my last column
		neighbours->send[neighbours->num].fromCol = subtask.toCol;
		neighbours->send[neighbours->num].toCol = subtask.toCol;
	
		neighbours->send[neighbours->num].fromRow = subtask.fromRow;
		neighbours->send[neighbours->num].toRow = subtask.toRow;	
		
		// increase
		neighbours->num ++;
	}

	// If there is upper neighbour
	if (subtask.fromRow > 0) {
		
		// calculate neighbouh id
		relativeX = subtask.quadrantCoordX;
		relativeY = subtask.quadrantCoordY - 1;
		neighbours->id[neighbours->num] = (relativeY * numVirtualCols) + relativeX;

		// recv the upper row
		neighbours->recv[neighbours->num].fromRow = subtask.fromRow - 1;
		neighbours->recv[neighbours->num].toRow = subtask.fromRow - 1;

		neighbours->recv[neighbours->num].fromCol = subtask.fromCol;
		neighbours->recv[neighbours->num].toCol = subtask.toCol;

		// send my first row
		neighbours->send[neighbours->num].fromRow = subtask.fromRow;
		neighbours->send[neighbours->num].toRow = subtask.fromRow;
	
		neighbours->send[neighbours->num].fromCol = subtask.fromCol;
		neighbours->send[neighbours->num].toCol = subtask.toCol;
		
		// increase
		neighbours->num ++;
	}

	// If there is lower neighbour
	if (subtask.toRow < (numRows-1)) {
		
		// calculate neighbouh id
		relativeX = subtask.quadrantCoordX;
		relativeY = subtask.quadrantCoordY + 1;
		neighbours->id[neighbours->num] = (relativeY * numVirtualCols) + relativeX;

		// recv the lower row
		neighbours->recv[neighbours->num].fromRow = subtask.toRow + 1;
		neighbours->recv[neighbours->num].toRow = subtask.toRow + 1;

		neighbours->recv[neighbours->num].fromCol = subtask.fromCol;
		neighbours->recv[neighbours->num].toCol = subtask.toCol;

		// send my last row
		neighbours->send[neighbours->num].fromRow = subtask.toRow;
		neighbours->send[neighbours->num].toRow = subtask.toRow;
	
		neighbours->send[neighbours->num].fromCol = subtask.fromCol;
		neighbours->send[neighbours->num].toCol = subtask.toCol;
		
		// increase
		neighbours->num ++;
	}
}
