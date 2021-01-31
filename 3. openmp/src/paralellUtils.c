#include "paralellUtils.h"

#include <stddef.h>
#include <stdlib.h>

MatrixSubTaskRef createMarixDivisions(int numThreads){
	MatrixSubTaskRef subtasks = NULL;
	subtasks = (MatrixSubTaskRef) calloc(numThreads,sizeof(MatrixSubTask));
	if(subtasks == NULL){
		doLog(ERROR, "Unable to reserve memory for the subtask division, aborting the execution");
		exit(IO_ERROR_CODE);
	}
	return subtasks;
}

MatrixSubTaskRef divideMatrixByRows(int numRows, int numCols, int numThreads){
	MatrixSubTaskRef subtasks;
	int numRowsPerTask, remainingRows, assignedRemainingRows, i;

	// do calculus
	numRowsPerTask = (int) numRows / numThreads;
	remainingRows = numRows % numThreads;

	// create subtasks array
	subtasks = createMarixDivisions(numThreads);

	// create subtasks
	assignedRemainingRows = 0;
	for(i=0; i<numThreads; i++){
		subtasks[i].fromCol = 0;
		subtasks[i].toCol = numCols - 1;
		subtasks[i].fromRow = (i * numRowsPerTask) + assignedRemainingRows;
		subtasks[i].toRow = (i * numRowsPerTask) + numRowsPerTask - 1 + assignedRemainingRows;
		if(assignedRemainingRows < remainingRows){
			subtasks[i].toRow++;
			assignedRemainingRows++;
		}
	}

	return subtasks;
}

MatrixSubTaskRef divideMatrixByColumns(int numRows, int numCols, int numThreads){
	MatrixSubTaskRef subtasks;
	int numColsPertask, remainingCols, assignedRemainingCols, i;

	// do calculus
	numColsPertask = (int) numCols / numThreads;
	remainingCols = numCols % numThreads;

	// create subtasks array
	subtasks = createMarixDivisions(numThreads);

	// create subtasks
	assignedRemainingCols = 0;
	for(i=0; i<numThreads; i++){
		subtasks[i].fromRow = 0;
		subtasks[i].toRow = numRows - 1;
		subtasks[i].fromCol = (i * numColsPertask) + assignedRemainingCols;
		subtasks[i].toCol = (i * numColsPertask) + numColsPertask - 1 + assignedRemainingCols;
		if(assignedRemainingCols < remainingCols){
			subtasks[i].toCol++;
			assignedRemainingCols++;
		}
	}

	return subtasks;
}

MatrixSubTaskRef buildMatrixDivisions(int numRows, int numCols, int numThreads, DivisionStructure ds){
	switch(ds){
		case DIVISION_BY_ROWS:
			return divideMatrixByRows(numRows, numCols, numThreads);
		case DIVISION_BY_COLS:
			return divideMatrixByColumns(numRows, numCols, numThreads);
		default:
			return NULL;
	}
}