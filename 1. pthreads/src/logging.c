#include "utils.h"
#include "logging.h"

#include<stdio.h>
#include <time.h>
#include<string.h>


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

void doLog(LogLevel level, char * message){
	char logEntry[LOG_MESSAGE_SIZE];
	const char * time = getDateAndTime();
	const char * levelStr = getLogLevel(level);
	sprintf(logEntry,"[%19s][%-7s] %s", time, levelStr, message);
	if(level >= LOG_LEVEL){
		fprintf(stderr, "%s \n", logEntry);
	}
}