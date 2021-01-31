#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "logging.h"

const char * getDateAndTime(){
	static char timeString[TIME_STRING_SIZE];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(timeString,"%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return timeString;
}

const char * getLogLevel(LogLevel level){
	static char * levels[] = {
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR"
	};
	return levels[level-1];
}

int getId(){
	static int internalId = -1;
	if(-1 == internalId) MPI_Comm_rank(MPI_COMM_WORLD, &internalId);
	return internalId;
}

void doLog(LogLevel level, char * message){
	const int id = getId();
	if(level >= LOG_LEVEL && ( ISMASTER(id) || LOG_LEVEL == DEBUG )){
		char logEntry[LOG_MESSAGE_SIZE];
		const char * time = getDateAndTime();
		const char * levelStr = getLogLevel(level);
		sprintf(logEntry,"[%19s][%-7s][%1d] %s", time, levelStr, id, message);
		fprintf(stderr, "%s\n", logEntry);
	}
}