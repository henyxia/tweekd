#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "printx.h"
#include "ui.h"

#define	FILENAME_LENGTH	24
#define	S_RESET			"\33[0m"
#define	MAX_BUFFER		128

FILE* logfile = NULL;
char s_color[4][12] = {"\x1b[01;31m", "\x1b[01;33m", "\x1b[01;32m", "\x1b[01;36m"};

void removeCharFromString(char c, char *str)
{
	int len = strlen(str)+1;

	for(int i=0; i<len; i++)
		if(str[i] == c)
			strncpy(&str[i],&str[i+1],len-i);
}

bool initLog()
{
	char	filename[FILENAME_LENGTH];
	time_t	now = time(NULL);

	strftime(filename, FILENAME_LENGTH, "%F-%T:%d", localtime(&now));
	setStartTime(filename);
	strftime(filename, FILENAME_LENGTH, "log/%F-%T:%d", localtime(&now));
	strcat(filename, ".log");

	logfile = fopen(filename, "w");
	if(logfile == NULL)
	{
		printf("Unable to open the log file\n");
		return false;
	}

	return true;
}

void closeLog()
{
	fclose(logfile);
}

void printx(severity s, char* str, ...)
{
	char	buffer[MAX_BUFFER];
	char	buffer2[MAX_BUFFER];
	va_list	arglist;
	va_start(arglist, str);
	vsprintf(buffer, str, arglist);
	fprintf(logfile, buffer);
	sprintf(buffer2, "%s%s%s", s_color[s], buffer, S_RESET);
	removeCharFromString('\n', buffer2);
	addToLog(buffer2);
	va_end(arglist);
}
