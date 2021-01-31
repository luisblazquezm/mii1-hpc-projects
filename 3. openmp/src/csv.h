#include "paralellUtils.h"

#ifndef __CSV_H
#define __CSV_H

#define FILE_NAME "analysis.csv"
#define DIRECTORY_NAME "./csv/"

void addCsvEntry(int simulationType, int divisionType, int numIterations, int numThreads, int numRows, int numCols, TimeResult result);

#endif