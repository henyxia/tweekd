#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include "printx.h"
#include "ui.h"

#define	FILENAME_LENGTH	24
#define	S_RESET			"\33[0m"
#define	MAX_BUFFER		128

FILE* logfiles[5] = {NULL, NULL, NULL, NULL, NULL};
char s_color[4][12] = {"\x1b[01;31m", "\x1b[01;33m", "\x1b[01;32m", "\x1b[01;36m"};
char f_name[6][5] = {"MAIN", "UI  ", "NFC ", "HVC ", "BUS ", "TEMP"};
struct timeval tv;

void removeCharFromString(char c, char *str)
{
	int len = strlen(str)+1;

	for(int i=0; i<len; i++)
		if(str[i] == c)
			strncpy(&str[i],&str[i+1],len-i);
}

bool initLog()
{
	//char	filename[FILENAME_LENGTH];
	//time_t	now = time(NULL);

	//strftime(filename, FILENAME_LENGTH, "%F-%T:%d", localtime(&now));
	//setStartTime(filename);
	//strftime(filename, FILENAME_LENGTH, "log/%F-%T:%d", localtime(&now));
	//strcat(filename, ".log");

	logfiles[5] = fopen("temp.log", "a");
	if(logfiles[5] == NULL)
	{
		printf("Unable to open the temperature log file\n");
		return false;
	}


	return true;
}

void closeLog()
{
	fclose(logfiles[5]);
}

void printx(severity s, msgfrom from, char* str, ...)
{
	char	buffer1[MAX_BUFFER];
	char	buffer2[MAX_BUFFER];
	va_list	arglist;
	va_start(arglist, str);
	vsprintf(buffer1, str, arglist);
	gettimeofday(&tv,NULL);
	fprintf(logfiles[5], "[%10ld] : %s", tv.tv_usec, buffer1);
	fflush(logfiles[5]);
	sprintf(buffer2, "[%s] %s%s%s", f_name[from], s_color[s], buffer1, S_RESET);
	if(s>DEBUG)
		printf("%s", buffer2);
	va_end(arglist);
}
