#include <math.h>

#ifndef __PLATE_H
#define __PLATE_H

#define NOT_ALLOWED_NUMBER NAN
#define IS_NOT_ALLOWED_NUMBER(v) (isnan(v))

#define R_VALUE 0.25
#define INITAL_COLD_TEMPERATURE 0
#define INITAL_HOT_TEMPERATURE 100
#define INITAL_DEFAULT_TEMPERATURE 20
#define HOLE_VALUE NOT_ALLOWED_NUMBER
#define ADIABATIC_VALUE NOT_ALLOWED_NUMBER

typedef float Value;

typedef struct {
	int fromRow, toRow, fromCol, toCol;
} Hole, *HoleRef;

typedef struct {
	int rows;
	int cols;
	Value ** values;
} Plate, * PlateRef;


int isAdiabatic(Value v);
void printPlate(PlateRef plate);
int isHole(PlateRef plate, int row, int col);
PlateRef createPlate(int numRows, int numCols);
Value getPlateValue(PlateRef plate, int row, int col);
void initializePlate(PlateRef plate, int numHoles, Hole holes[]);
Value calculateNewTemperature(PlateRef plate, int row, int col);
void setPlateValue(PlateRef plate, int row, int col, Value value);
void initializeHole(HoleRef hole, int fromRow, int toRow, int fromCol, int toCol);

#endif