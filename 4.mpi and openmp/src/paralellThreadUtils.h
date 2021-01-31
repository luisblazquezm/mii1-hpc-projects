#include "utils.h"
#include "plate.h"
#include <time.h>

#ifndef __PARALEL_THREADS_UTILS
#define __PARALEL_THREADS_UTILS

typedef struct {
	int fromRow, toRow, fromCol, toCol;
} MatrixSubTask, * MatrixSubTaskRef;

typedef enum { DIVISION_BY_COLS=2, DIVISION_BY_ROWS=3  } DivisionStructure;


MatrixSubTaskRef buildMatrixDivisions(int numRows, int numCols, int numThreads, DivisionStructure ds);

#endif