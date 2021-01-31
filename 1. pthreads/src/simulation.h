#include "plate.h"
#include "paralellUtils.h"

#ifndef __SIMULATION_H
#define __SIMULATION_H

void serialSimulation(int numIterations, int numRows, int numCols, TimeResultRef timeResult);
void paralellSimulation(int numIterations, int numRows, int numCols, int numThreads, DivisionStructure division, TimeResultRef timeResult);

#endif