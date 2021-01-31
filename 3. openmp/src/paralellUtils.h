#include "utils.h"
#include "plate.h"
#include <time.h> 
#include <pthread.h>

#ifndef __PARALEL_UTILS
#define __PARALEL_UTILS

typedef struct {
	int fromRow, toRow, fromCol, toCol;
} MatrixSubTask, * MatrixSubTaskRef;

typedef enum { DIVISION_BY_COLS=2, DIVISION_BY_ROWS=3  } DivisionStructure;

typedef struct {
	int numThreads;
	long totaltime;
	long times[NUM_ITERATIONS];
} TimeResult, *TimeResultRef;

MatrixSubTaskRef buildMatrixDivisions(int numRows, int numCols, int numThreads, DivisionStructure ds);

#endif