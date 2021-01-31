#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "plate.h"
#include "logging.h"
#include "paralellUtils.h"

void calculateTaskMatrixDivisions(int numProcesses, int * virtualRows, int * virtualColumns) {
    int i, j, numVirtualRows, numVirtualCols;

    // set as default, one row and as much columns as processes in matrix
    numVirtualRows = 1;
    numVirtualCols = numProcesses;

    // calculate divisions
    for(i=1; i<=numProcesses; i++){
        for(j=1; j<=numProcesses; j++){
            if(i*j == numProcesses 
            	&& (abs(numVirtualRows-numVirtualCols) > abs(i-j))
            ){
                numVirtualRows = i;
                numVirtualCols = j;
            }
        }
    }

    *virtualRows = numVirtualRows;
    *virtualColumns = numVirtualCols;
}

void calculateProcessSubtask(int processId, int numRows, int numCols, int numVirutalRows, int numVirutalCols, ProcessSubTaskRef subtask){
	// calculate columns
	subtask->quadrantCoordX = processId % numVirutalCols;
	subtask->fromCol = (numCols / numVirutalCols) * subtask->quadrantCoordX;
	if (subtask->quadrantCoordX == numVirutalCols -1) {
		subtask->toCol = numCols - 1;
	} else {
		subtask->toCol = ((numCols / numVirutalCols) * (subtask->quadrantCoordX + 1)) - 1;
	}

	// calculate rows
	subtask->quadrantCoordY = processId / numVirutalCols;
	subtask->fromRow = (numRows / numVirutalRows) * subtask->quadrantCoordY;
	if (subtask->quadrantCoordY == numVirutalRows) {
		subtask->toRow = numRows - 1;
	} else {
		subtask->toRow = ((numRows / numVirutalRows) * (subtask->quadrantCoordY + 1)) - 1;
	}
}


void createVectorDataType(ProcessSubTask subtask, int plateCols, MPI_Datatype * type){
	int numRows, numCols;
	numCols = 1 + (subtask.toCol - subtask.fromCol);
	numRows = 1 + (subtask.toRow - subtask.fromRow);
	MPI_Type_vector(numRows, numCols, plateCols, MPI_FLOAT, type);
	MPI_Type_commit(type);		
}

void sendUpdateToNeighbours(PlateRef plate, Neighbours neighbours, int iteration){
	MPI_Datatype type;
	int tag, n, position;

	for(n=0; n<neighbours.num; n++){
		// tag,  type and start position
		tag = iteration + SEND_ITERATION_PIECE;
		createVectorDataType(neighbours.send[n], plate->cols, &type);
		position = (neighbours.send[n].fromRow * plate->cols) + neighbours.send[n].fromCol;
		// send
		MPI_Send(&(plate->values[position]), 1, type, neighbours.id[n], tag, MPI_COMM_WORLD);
	}
}

void recvUpdateFromNeighbours(PlateRef plate, Neighbours neighbours, int iteration){
	MPI_Datatype type;
	MPI_Status status;
	int tag, n, position;

	for(n=0; n<neighbours.num; n++){
		// tag,  type and start position
		tag = iteration + SEND_ITERATION_PIECE;
		createVectorDataType(neighbours.recv[n], plate->cols, &type);
		position = (neighbours.recv[n].fromRow * plate->cols) + neighbours.recv[n].fromCol;
		// recv
		MPI_Recv(&(plate->values[position]), 1, type, neighbours.id[n], tag, MPI_COMM_WORLD, &status);
	}
}

void handleFinalResult(PlateRef plate, ProcessSubTask subtask, int processId, int numProcesses){
	MPI_Datatype type;
	MPI_Status status;
	ProcessSubTask currentProcess;
	int tag, n, position, numVirutalRows, numVirutalCols;

	if(!(ISMASTER(processId))){
		// tag,  type and start position
		tag = SEND_FINAL_PIECE;
		createVectorDataType(subtask, plate->cols, &type);
		position = (subtask.fromRow * plate->cols) + subtask.fromCol;
		// send
		MPI_Send(&(plate->values[position]), 1, type, MASTERID, tag, MPI_COMM_WORLD);
	}else{
		// calculate matrix subidivisions for calculating each process subtask after
		calculateTaskMatrixDivisions(numProcesses, &numVirutalRows, &numVirutalCols);

		for(n=1; n<numProcesses;n++){
			// calculate process subtask
			calculateProcessSubtask(n, plate->rows, plate->cols, numVirutalRows, numVirutalCols, &currentProcess);
			// tag,  type and start position
			tag = SEND_FINAL_PIECE;
			createVectorDataType(currentProcess, plate->cols, &type);
			position = (currentProcess.fromRow * plate->cols) + currentProcess.fromCol;
			// recv
			MPI_Recv(&(plate->values[position]), 1, type, n, tag, MPI_COMM_WORLD, &status);
		}
	}
}