#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "ui.h"

#define	TIME_LENGTH			24
#define	HEADER_TEXT_LENGTH	24
#define	LOG_LINES			25
#define	LOG_LENGTH			69
#define	IPS					20
#define	SCREEN_TIME			1000000/IPS

char	tmpretheader[HEADER_TEXT_LENGTH+1];
char	tmpretlog[HEADER_TEXT_LENGTH+1];
char	uilog[LOG_LINES][LOG_LENGTH];
char	started[TIME_LENGTH];
char	uid[HEADER_TEXT_LENGTH];
char	uidDate[HEADER_TEXT_LENGTH];
pid_t	mainPid;

void* drawUI(void* we)
{
	while(1)
	{
		printf("\x1b[2J\x1b[1;1H");
		displayUI();
		usleep(SCREEN_TIME);
	}

	return NULL;
}

void initUILog()
{
	tmpretheader[0]='\0';
	tmpretlog[0]='\0';
	started[0]='\0';
	mainPid = getpid();
	uid[0]='\0';
	uidDate[0]='\0';
	for(int i=0; i<LOG_LINES; i++)
		uilog[i][0] = '\0';
}

char* fillHeaderWithSpaces(char* text)
{
	strcpy(tmpretheader, text);
	while(strlen(tmpretheader) < HEADER_TEXT_LENGTH)
		strcat(tmpretheader, " ");

	return tmpretheader;
}

void addToLog(char* newStr)
{
	for(int i=LOG_LINES-2; i>=0; i--)
		strcpy(uilog[i+1], uilog[i]);
	strcpy(uilog[0], newStr);
}

char* fillLogWithSpaces(char* text)
{
	strcpy(tmpretlog, text);
	if(strlen(text) > 0)
		while((strlen(tmpretlog)-12) < LOG_LENGTH)
			strcat(tmpretlog, " ");
	else
		while(strlen(tmpretlog) < LOG_LENGTH)
			strcat(tmpretlog, " ");

	return tmpretlog;
}


void setStartTime(char* sT)
{
	strcpy(started, sT);
}

void displayUI()
{
	char	buffer[HEADER_TEXT_LENGTH];
	//header
	printf("\u250F\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2513\n");
	printf("\u2503 %s \u2503      T H E      \u2503", fillHeaderWithSpaces("Started since"));
   	printf(" %s \u2503\n", fillHeaderWithSpaces("No Tag Detected"));
	printf("\u2503 %s \u2503    T W E E K    \u2503 ", fillHeaderWithSpaces(started));
	sprintf(buffer, "%s", strlen(uidDate) > 0 ? uidDate : "Waiting for a tag");
	printf("%s \u2503\n", fillHeaderWithSpaces(buffer));
	sprintf(buffer, "PID %d", mainPid);
	printf("\u2503 %s \u2503  P R O J E C T  \u2503 ", fillHeaderWithSpaces(buffer));
	sprintf(buffer, "UID : %s", strlen(uid) > 0 ? uid : "Nope");
	printf("%s \u2503\n", fillHeaderWithSpaces(buffer));
	printf("\u2523\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u252B\n");
	//body
	for(int i=0; i<LOG_LINES; i++)
		printf("\u2503 %s \u2503\n", fillLogWithSpaces(uilog[LOG_LINES-i-1]));
	printf("\u2517\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u251B\n");
}
