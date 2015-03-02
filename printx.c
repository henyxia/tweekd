#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "printx.h"

#define	FILENAME_LENGTH	24
#define	S_RESET			"\33[0m"
#define	MAX_BUFFER		128

FILE* logfile = NULL;
char s_color[4][12] = {"\x1b[01;31m", "\x1b[01;33m", "\x1b[01;32m", "\x1b[01;36m"};

bool initLog()
{
	char	filename[FILENAME_LENGTH];
	time_t	now = time(NULL);

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
	va_list	arglist;
	va_start(arglist, str);
	vsprintf(buffer, str, arglist);
	fprintf(logfile, buffer);
	printf(s_color[s]);
	printf(buffer);
	printf(S_RESET);
	va_end(arglist);
}
