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

FILE* logfiles[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
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
	logfiles[0] = fopen("log/main.log", "a");
	if(logfiles[0] == NULL)
	{
		printf("Unable to open the main log file\n");
		return false;
	}
	logfiles[1] = fopen("log/ui.log", "a");
	if(logfiles[1] == NULL)
	{
		printf("Unable to open the UI log file\n");
		return false;
	}

	logfiles[2] = fopen("log/nfc.log", "a");
	if(logfiles[2] == NULL)
	{
		printf("Unable to open the NFC log file\n");
		return false;
	}

	logfiles[3] = fopen("log/hvc.log", "a");
	if(logfiles[3] == NULL)
	{
		printf("Unable to open the HVC log file\n");
		return false;
	}

	logfiles[4] = fopen("log/bus.log", "a");
	if(logfiles[4] == NULL)
	{
		printf("Unable to open the BUS log file\n");
		return false;
	}

	logfiles[5] = fopen("log/temp.log", "a");
	if(logfiles[5] == NULL)
	{
		printf("Unable to open the temperature log file\n");
		return false;
	}

	return true;
}

void closeLog()
{
	fclose(logfiles[0]);
	fclose(logfiles[1]);
	fclose(logfiles[2]);
	fclose(logfiles[3]);
	fclose(logfiles[4]);
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
	fprintf(logfiles[from], "[%10ld.%06ld] : %s", tv.tv_sec, tv.tv_usec, buffer1);
	fflush(logfiles[from]);
	sprintf(buffer2, "[%s] %s%s%s", f_name[from], s_color[s], buffer1, S_RESET);
	if(s>DEBUG)
		printf("%s", buffer2);
	va_end(arglist);
}
