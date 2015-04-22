#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <termios.h>
#include "ui.h"
#include "bus.h"

#define	TIME_LENGTH			24
#define	HEADER_TEXT_LENGTH	24
#define	LOG_LINES			35
#define	LOG_LENGTH			82
#define	IPS					20
#define	SCREEN_TIME			1000000/IPS
#define	SPACES				"                                                                                                                                                                                                                                                                  "
char	started[TIME_LENGTH];
char	uid[HEADER_TEXT_LENGTH];
char	uidDate[HEADER_TEXT_LENGTH];
char	cmd[LOG_LENGTH];
pid_t	mainPid;
bool	heat = false;
bool	pump = false;
unsigned int	temp = 1;
unsigned int	debit = 1;
struct	termios old={0};
bool uiStop = false;

void stopUI()
{
	uiStop = true;
}

void setHeat(bool s)
{
	heat = s;
}

void setPump(bool s)
{
	pump = s;
}

void setTemp(unsigned int t)
{
	temp = t;
}

void setDebit(unsigned int d)
{
	debit = d;
}

void processScreen()
{
	
}

void* drawUI(void* we)
{
	while(!uiStop)
	{
		//printf("\x1b[2J\x1b[1;1H");
		//displayUI();
		processScreen();
		usleep(SCREEN_TIME);
	}

	return NULL;
}

void setTagName(char* tag)
{
	time_t now = time(NULL);
	strcpy(uid, tag);
	strftime(uidDate, HEADER_TEXT_LENGTH, "%F-%T:%d", localtime(&now));
}

void initUILog()
{
	cmd[0]='\0';
	started[0]='\0';
	mainPid = getpid();
	uid[0]='\0';
	uidDate[0]='\0';
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

void setStartTime(char* sT)
{
	strcpy(started, sT);
}
