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

char	uilog[LOG_LINES][LOG_LENGTH];
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

void* drawUI(void* we)
{
	while(!uiStop)
	{
		printf("\x1b[2J\x1b[1;1H");
		displayUI();
		usleep(SCREEN_TIME);
	}

	return NULL;
}

void setDFG(void)
{
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
}

void setTagName(char* tag)
{
	time_t now = time(NULL);
	strcpy(uid, tag);
	strftime(uidDate, HEADER_TEXT_LENGTH, "%F-%T:%d", localtime(&now));
}

void initUILog()
{
	setDFG();
	cmd[0]='\0';
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
	for(int i=LOG_LINES-3; i>=0; i--)
		strcpy(uilog[i+1], uilog[i]);
	strcpy(uilog[0], newStr);
}

char* fillLogWithSpaces(char* buf, char* text)
{
	strncpy(buf, text, LOG_LENGTH);
	if(strlen(buf) > 0)
		strncat(buf, SPACES, LOG_LENGTH-strlen(buf)-1);
	else
		strncat(buf, SPACES, LOG_LENGTH-strlen(buf)-1-12);

	return buf;
}

char* fillStatusWithSpaces(char* buf, char* text)
{/*
	strncpy(buf, text, LOG_LENGTH);
		strncat(buf, SPACES, LOG_LENGTH-strlen(buf)-1-7);
*/
	return buf;
}

void setStartTime(char* sT)
{
	strcpy(started, sT);
}

char getch()
{
    char buf=0;
    if(read(0,&buf,1)<0)
        perror("read()");
    return buf;
}

void displayUI()
{
	char	buffer[LOG_LENGTH];
	struct timeval tv = {0, 5000};
	char	bufferO[4] = {0, 0, 0, 0};
	fd_set rdfs;
	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);
	select(1, &rdfs, NULL, NULL, &tv);
	//header
	printf("\u250F\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2533\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2513\n");
	printf("\u2503 %s \u2503      T H E      \u2503", fillHeaderWithSpaces(buffer, "Started since"));
	sprintf(buffer, "%s", strlen(uidDate) > 0 ? uidDate : "Waiting for a tag");
   	printf(" %s \u2503\n", fillHeaderWithSpaces(buffer, buffer));
	printf("\u2503 %s \u2503    T W E E K    \u2503 ", fillHeaderWithSpaces(buffer, started));
	printf("%s \u2503\n", fillHeaderWithSpaces(buffer, " "));
	sprintf(buffer, "PID %d", mainPid);
	printf("\u2503 %s \u2503  P R O J E C T  \u2503 ", fillHeaderWithSpaces(buffer, buffer));
	sprintf(buffer, "UID : %s", strlen(uid) > 0 ? uid : "Nope");
	printf("%s \u2503\n", fillHeaderWithSpaces(buffer, buffer));
	printf("\u2523\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u253B\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u252B\n");
	sprintf(buffer, "\u2503 HEAT[%s] %02.1f\u2103  PUMP[%s] %04d", heat ? "ON " : "OFF", 25 + (((float)(80*temp)))/255, pump ? "ON " : "OFF", debit);
	printf("%s  \u2503\n", fillStatusWithSpaces(buffer, buffer));
	printf("\u2523\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u252B\n");
	//body
	int i=0;
	while(i<LOG_LINES)
	{
		printf("\u2503 %s \u2503\n", fillLogWithSpaces(buffer, uilog[LOG_LINES-i-1]));
		i++;
	}
	printf("\u2523\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u252B\n");
	//cmd
	if(FD_ISSET(STDIN_FILENO, &rdfs))
	{
		bufferO[0] = getch();
		if(bufferO[0] >= 65 && bufferO[0] <= 90 && bufferO[1] == '\0')
		{
			sprintf(buffer, "%c", bufferO[0] + 32);
			strcat(cmd, buffer);
		}
		else if(bufferO[0] >= 97 && bufferO[0] <= 122 && bufferO[1] == '\0')
		{
			sprintf(buffer, "%c", bufferO[0]);
			strcat(cmd, buffer);
		}
		else if(bufferO[0] >= 48 && bufferO[0] <= 57)
		{
			sprintf(buffer, "%c", bufferO[0]);
			strcat(cmd, buffer);
		}
		else if(bufferO[0] == 32)
		{
			sprintf(buffer, "%c", bufferO[0]);
			strcat(cmd, buffer);
		}
		else if(bufferO[0] == 10)
		{
			sendToBus(cmd);
			cmd[0]='\0';
		}
		else if(bufferO[0] == 127)
		{
			if(strlen(cmd)>0)
			cmd[strlen(cmd)-1]='\0';
		}
		else
		{
			//sprintf(buffer, "Unrecognized %02x", bufferO[0]);
			//strcat(cmd, buffer);
		}
	}
	sprintf(buffer, "\u2503> %s", cmd);
	printf("%s\u2503\n", fillStatusWithSpaces(buffer, buffer));
	printf("\u2517\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u2501\u251B\n");
	printf("\x1b[2A\x1b[%dC", strlen(cmd) + 3);
	fflush(stdout);
}
