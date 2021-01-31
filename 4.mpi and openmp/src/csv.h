#include "utils.h"
#include "paralellThreadUtils.h"

#ifndef __CSV_H
#define __CSV_H

typedef struct {
	int numThreads;
	double totaltime;
	double times[NUM_ITERATIONS];
} TimeResult, *TimeResultRef;

void addCsvEntry(int simulationType, int divisionType, int numIterations, int numProcesses, int numThreads, int numRows, int numCols, TimeResult result);

#endif