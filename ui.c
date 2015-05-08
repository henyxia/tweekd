#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <termios.h>
#include "printx.h"
#include "ui.h"
#include "bus.h"
#include "usb.h"
#include "pump.h"

#define	TIME_LENGTH			24
#define	HEADER_TEXT_LENGTH	24
#define	LOG_LINES			35
#define	LOG_LENGTH			82
#define	IPS					2
#define	SCREEN_TIME			1000000/IPS
#define	MAX_FILENAME_LENGTH	30
#define	SC_HOME				0
#define	SC_MAIN				1


char			started[TIME_LENGTH];
char			uid[HEADER_TEXT_LENGTH];
char			cmd[LOG_LENGTH];
pid_t			mainPid;
bool			heat = false;
bool			pump = false;
unsigned int	temp = 1;
unsigned int	debit = 1;
struct			termios old={0};
bool			uiStop = false;
int				actScreen = SC_HOME;

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

int calculateInteraction(int act, int x, int y)
{
	char			filename[MAX_FILENAME_LENGTH];
	FILE*			f = NULL;
	unsigned char	ret;
	switch(act)
	{
		case SC_HOME:
			return act;
		case SC_MAIN:
			strcpy(filename, "map/main.map");
			break;
		default:
			printx(ERROR, UI, "Unrecognized state %d\n", act);
			return act;
	}

	f = fopen(filename, "r");
	if(f == NULL)
	{
		printx(ERROR, UI, "The selected map is unopenable\n");
		return act;
	}

	fseek(f, x + y*320, SEEK_SET);

	ret = fgetc(f);
	printx(DEBUG, UI, "Ret is %02X\n", ret);

	if(ret == 1)
	{
		setVolWanted(1);
		setCoffeeWanted();
	}
	else if(ret == 2)
	{
		setVolWanted(2);
		setCoffeeWanted();
	}

	fclose(f);
	
	return act;
}

void processScreen()
{
	int x = -1;
	int y = -1;
	if(actScreen == SC_HOME)
	{
		if(uid[0] == '\0')
			return;
		else
		{
			actScreen = SC_MAIN;
			displayPicture("img/main.rgb");
		}
	}
	else if(actScreen == SC_MAIN)
	{
		if(screenTouched(&x, &y))
			actScreen = calculateInteraction(actScreen, x, y);
	}
}

void* drawUI(void* we)
{
	while(!uiStop)
	{
		processScreen();
		usleep(SCREEN_TIME);
	}

	return NULL;
}

void setTagName(char* tag)
{
	//time_t now = time(NULL);
	strcpy(uid, tag);
	//strftime(uidDate, HEADER_TEXT_LENGTH, "%F-%T:%d", localtime(&now));
}

void initUILog()
{
	cmd[0]='\0';
	started[0]='\0';
	mainPid = getpid();
	uid[0]='\0';
	initUSB();
}

void setStartTime(char* sT)
{
	strcpy(started, sT);
}
