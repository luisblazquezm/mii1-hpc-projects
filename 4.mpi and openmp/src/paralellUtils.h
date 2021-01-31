
#ifndef __PARALELL_UTILS_H
#define __PARALELL_UTILS_H

#define MAX_NEIGHBOUR 4

typedef struct {
	int quadrantCoordX, quadrantCoordY;
	int fromRow, toRow, fromCol, toCol;
} ProcessSubTask, *ProcessSubTaskRef;

typedef struct {
	int num, id[MAX_NEIGHBOUR];
	ProcessSubTask send[MAX_NEIGHBOUR];
	ProcessSubTask recv[MAX_NEIGHBOUR];
} Neighbours, *NeighboursRef;

typedef enum {
	SEND_ITERATION_PIECE=10000,
	SEND_FINAL_PIECE=20000
} MpiMessageTags;

void sendUpdateToNeighbours(PlateRef plate, Neighbours neighbours, int iteration);
void recvUpdateFromNeighbours(PlateRef plate, Neighbours neighbours, int iteration);
void handleFinalResult(PlateRef plate, ProcessSubTask subtask, int processId, int numProcesses);

void calculateTaskMatrixDivisions(int numProcesses, int * virtualRows, int * virtualColumns);
void calculateProcessSubtask(int processId, int numRows, int numCols, int numVirutalRows, int numVirutalCols, ProcessSubTaskRef subtask);

#endif
