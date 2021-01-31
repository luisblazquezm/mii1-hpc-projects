#ifndef __LOG_H
#define __LOG_H

#define TIME_STRING_SIZE 100
#define LOG_MESSAGE_SIZE 1000

typedef enum {
	DEBUG=1, 
	INFO=2, 
	WARNING=3, 
	ERROR=4
} LogLevel;

const char * getDateAndTime();
void doLog(LogLevel level, char * message);

#endif