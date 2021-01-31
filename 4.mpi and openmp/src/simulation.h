
#include "csv.h"
#include "paralellThreadUtils.h"


#ifndef __SIMULATION_H
#define __SIMULATION_H


void simulation(int processId, int numProcesses, int numThreads, DivisionStructure division, int numIterations, int numRows, int numCols, TimeResultRef timeResult);

#endif