#include "utils.h"
#include "plate.h"
#include "logging.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

PlateRef createPlate(int numRows, int numCols){
	int i, j;
	PlateRef plate = NULL;

	// reserve memory for the structure
	plate = (PlateRef) calloc(1,sizeof(Plate));
	if(plate == NULL){
		doLog(ERROR, "Unable to reserve memory to the structure, aborting the execution");
		exit(IO_ERROR_CODE);
	}
	plate->rows = numRows;
	plate->cols = numCols;
	plate->values = NULL;

	// reserve memory for the matrix
	plate->values = (Value *) calloc(plate->rows * plate-> cols, sizeof(Value));
	if(plate->values == NULL){
		doLog(ERROR, "Unable to reserve memory to the matrix, aborting the execution");
		exit(IO_ERROR_CODE);
	}

	return plate;
}

void initializePlate(PlateRef plate, int numHoles, Hole holes[]){
	Hole h;
	int i, j, k, row, col;

	// set common values for each position
	for(i=0; i < plate->rows; i++){
		for(j=0; j < plate->cols; j++){
			setPlateValue(plate, i, j, INITAL_DEFAULT_TEMPERATURE);
		}
	}

	// set values for holes
	for(k=0; k < numHoles; k++){
		Hole h = holes[k];
		for(i=h.fromRow; i<=h.toRow; i++){
			for(j=h.fromCol; j<=h.toCol; j++){
				setPlateValue(plate, i, j, HOLE_VALUE);
			}
		}
	}

	// intilialize extremes
	for(i=0; i < plate->rows; i++){
		setPlateValue(plate, i, 0, INITAL_COLD_TEMPERATURE);
		setPlateValue(plate, i, plate->cols-1, INITAL_HOT_TEMPERATURE);
	}
}

Value calculateNewTemperature(PlateRef plate, int row, int col){
	int posRow, posCol;
	Value current, above, bellow, left, right, result;

	// retrieve values
	current = getPlateValue(plate, row,     col    );
	above   = getPlateValue(plate, row - 1, col    );
	bellow  = getPlateValue(plate, row + 1, col    );
	left    = getPlateValue(plate, row,     col - 1);
	right   = getPlateValue(plate, row,     col + 1);

	// calculate terms
	result = current;
	if(!isAdiabatic(above)){
		result -= (R_VALUE * (current - above));
	}
	if(!isAdiabatic(bellow)){
		result -= (R_VALUE * (current - bellow));
	}
	if(!isAdiabatic(left)){
		result -= (R_VALUE * (current - left));
	}
	if(!isAdiabatic(right)){
		result -= (R_VALUE * (current - right));
	}

	return result;
}

void printPlate(PlateRef plate){
	int i, j, k;

	fprintf(stderr, "\n\t");

	// print separator
	for(k=0; k<plate->cols; k++){
		fprintf(stderr, "+------");
	}
	fprintf(stderr, "+\n\t");

	// print matrix
	for(i=0; i < plate->rows; i++){

		// print row
		for(j=0; j < plate->cols; j++){
			if(isHole(plate, i, j)){
				fprintf(stderr, "|  XX  ");
			}else{
				Value v = getPlateValue(plate, i, j);
				fprintf(stderr, "|%6.2f", v);
			}
		}
		fprintf(stderr, "|\n\t");

		// print separator
		for(k=0; k<plate->cols; k++){
			fprintf(stderr, "+------");
		}
		fprintf(stderr, "+\n\t");
	}

	fprintf(stderr, "\n");
}

Value getPlateValue(PlateRef plate, int row, int col){
	int offset;
	if( (row >= 0) && (row <= (plate->rows - 1)) && (col >= 0) && (col <= (plate->cols - 1))){
		offset = (row * plate->cols) + col;
		return plate->values[offset];
	}else{
		return ADIABATIC_VALUE;
	}
}

int isHole(PlateRef plate, int row, int col){
	Value v = getPlateValue(plate, row, col);
	return IS_NOT_ALLOWED_NUMBER(v);
}

int isAdiabatic(Value v){
	return IS_NOT_ALLOWED_NUMBER(v);
}

void setPlateValue(PlateRef plate, int row, int col, Value value){
	int offset;
	if(!isHole(plate, row, col)){
		offset = (row * plate->cols) + col;
		plate->values[offset] = value;
	}else if(!IS_NOT_ALLOWED_NUMBER(value)){
		doLog(WARNING, "Attempt to update a hole value.");
	}
}

void initializeHole(HoleRef hole, int fromRow, int toRow, int fromCol, int toCol){
	hole->toRow = toRow;
	hole->toCol = toCol;
	hole->fromRow = fromRow;
	hole->fromCol = fromCol;
}