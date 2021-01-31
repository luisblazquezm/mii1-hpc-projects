#include "utils.h"

#ifndef __CSV_H
#define __CSV_H

#define FILE_NAME "analysis.csv"
#define DIRECTORY_NAME "./csv/"

typedef struct {
	int numThreads;
	double totaltime;
	double times[NUM_ITERATIONS];
} TimeResult, *TimeResultRef;

void addCsvEntry(int simulationType, int divisionType, int numIterations, int numThreads, int numRows, int numCols, TimeResult result);

#endif