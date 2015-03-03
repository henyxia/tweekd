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
#define	SPACES				"                                                                                                                                                                                                                                                                  "

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
	started[0]='\0';
	mainPid = getpid();
	uid[0]='\0';
	uidDate[0]='\0';
	for(int i=0; i<LOG_LINES; i++)
		uilog[i][0] = '\0';
}

char* fillHeaderWithSpaces(char* buf, char* text)
{
	char tmpretheader[HEADER_TEXT_LENGTH+1];
	strcpy(tmpretheader, text);
	while(strlen(tmpretheader) < HEADER_TEXT_LENGTH)
		strcat(tmpretheader, " ");

	for(int i=0; i<strlen(tmpretheader); i++)
		buf[i] = tmpretheader[i];
	buf[strlen(tmpretheader)] = '\0';

	return buf;
}

void addToLog(char* newStr)
{
	for(int i=LOG_LINES-2; i>=0; i--)
		strcpy(uilog[i+1], uilog[i]);
	strcpy(uilog[0], newStr);
}

char* fillLogWithSpaces(char* buf, char* text)
{
/*
	char tmpretlog[LOG_LENGTH];
	strncpy(tmpretlog, text, LOG_LENGTH);
	if(strlen(text) > 0)
		while((strlen(tmpretlog)-12) < LOG_LENGTH)
			strcat(tmpretlog, " ");
	else
		while(strlen(tmpretlog) < LOG_LENGTH)
			strcat(tmpretlog, " ");

//	int j;
//	printf("J val\n");
//	printf("%d\n", j);
	for(int j=0; j<strlen(tmpretlog); j++)
		buf[j] = tmpretlog[j];
	buf[strlen(tmpretlog)] = '\0';
*/
	strncpy(buf, text, LOG_LENGTH);
	/*
	if(strlen(buf) > 0)
		while((strlen(buf)-12) < LOG_LENGTH)
			strcat(buf, " ");
	else
		while(strlen(buf) < LOG_LENGTH)
			strcat(buf, " ");
	*/
	//strncat(buf, SPACES, LOG_LENGTH-strlen(buf));

	return buf;
}


void setStartTime(char* sT)
{
	strcpy(started, sT);
}

void displayUI()
{
	char	buffer[LOG_LENGTH];
	//header
	printf("\u250F\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2513\n");
	printf("\u2503 %s \u2503      T H E      \u2503", fillHeaderWithSpaces(buffer, "Started since"));
   	printf(" %s \u2503\n", fillHeaderWithSpaces(buffer, "No Tag Detected"));
	printf("\u2503 %s \u2503    T W E E K    \u2503 ", fillHeaderWithSpaces(buffer, started));
	sprintf(buffer, "%s", strlen(uidDate) > 0 ? uidDate : "Waiting for a tag");
	printf("%s \u2503\n", fillHeaderWithSpaces(buffer, buffer));
	sprintf(buffer, "PID %d", mainPid);
	printf("\u2503 %s \u2503  P R O J E C T  \u2503 ", fillHeaderWithSpaces(buffer, buffer));
	sprintf(buffer, "UID : %s", strlen(uid) > 0 ? uid : "Nope");
	printf("%s \u2503\n", fillHeaderWithSpaces(buffer, buffer));
	printf("\u2522\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u252B\n");
	//body
	int i=0;
	while(i<LOG_LINES)
	{
		printf("\u2503 %s \u2503\n", fillLogWithSpaces(buffer, uilog[LOG_LINES-i-1]));
		i++;
	}
	printf("\u2517\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u251B\n");
}
