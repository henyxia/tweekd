#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "printx.h"
#include "ui.h"
#include "nfc.h"
#include "hvc.h"

#define	CMD_MAX	70

int		bus;
bool	busFree = true;
bool	busStop = false;

void stopBus()
{
	busStop = true;
}

void processCmd(char* buffer)
{
	buffer[strlen(buffer)-1]='\0';
	if(strcmp(buffer, "quit") == 0 || strcmp(buffer, "exit") == 0)
	{
		printx(INFO, BUS, "Exit request receved, processing ...\n");
		stopNFC();
		stopHVC();
		stopBus();
		stopUI();
	}
	else if(strcmp(buffer, "setpumpon") == 0)
	{
		printx(INFO, BUS, "Setting PUMP ON");
		setPumpWantedState(true);
	}
	else if(strcmp(buffer, "setpumpoff") == 0)
	{
		printx(INFO, BUS, "Setting PUMP OFF");
		setPumpWantedState(false);
	}
	else if(strcmp(buffer, "setheaton") == 0)
	{
		printx(INFO, BUS, "Setting HEAT ON");
		setHeatWantedState(true);
	}
	else if(strcmp(buffer, "setheatoff") == 0)
	{
		printx(INFO, BUS, "Setting HEAT OFF");
		setHeatWantedState(false);
	}

	//printx(DEBUG, BUS, "STRLEN : %d and strcmp ret %d", strlen(buffer), strcmp(buffer, "quit"));
}

void* processBus(void* we)
{
	bus = open("bus.log", O_RDWR);
	char	buffer[CMD_MAX];
	printx(DEBUG, BUS, "Waiting for events\n");
	while(!busStop)
	{
		while(busFree);
		while(!busFree);
		printx(DEBUG, BUS, "Event receved !\n");
		busFree = false;
		lseek(bus, 0, SEEK_SET);
		printx(DEBUG, BUS, "Data read %d\n", read(bus, buffer, CMD_MAX));
		ftruncate(bus, 0);
		sync();
		processCmd(buffer);
		busFree = true;
		printx(DEBUG, BUS, buffer);
	}
	return NULL;
}

bool initBus()
{
	bus = creat("bus.log", 0666);
	if(bus == -1)
	{
		printf("Unable to open the bus file\n");
		return false;
	}

	return true;
}

void sendToBus(char* cmd)
{
	while(!busFree);
	busFree = false;
	ftruncate(bus, 0);
	ftruncate(bus, CMD_MAX);
	lseek(bus, 0, SEEK_SET);
	dprintf(bus, "%s\n", cmd);
	busFree = true;
}
