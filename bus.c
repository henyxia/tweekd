#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "printx.h"
#include "ui.h"
#include "nfc.h"
#include "hvc.h"
#include "heat.h"
#include "pump.h"

#define	CMD_MAX		70
#define KEY			1100
#define	SEM_NUMBER	2
#define	SEM_INPUT	0
#define	SEM_OUTPUT	1

int		bus;
bool	busFree = true;
bool	busStop = false;
int		mySem;

int get_sem(int i)
{
	ushort semarr[30];
	union semun
	{
		int val;
		struct semid_ds *buf;
		ushort *array;
	}arg;

	arg.array = semarr;
	semctl(mySem, i, GETALL, arg);
	return semarr[i];
}

void show_sem(int i)
{
	int val;
	val = get_sem(i);
	printf("semaphore[%d]=%d\n", i, val);
}

void create_sem(int N)
{
	printx(DEBUG, BUS, "create %d semaphores\n", N);
	mySem = semget(KEY, N, 0666 | IPC_CREAT);
	if(mySem < 0)
		printx(ERROR, BUS, "Unable to create the semaphore\n");
}

void init_sem(int N)
{
	int j;
	int retval;
	union semun
	{
		int val;
		struct semid_ds *buf;
		ushort *array;
	}arg;
	arg.val = 1;
	for (j=0; j<N; j++)
	{
		retval = semctl(mySem, j, SETVAL, arg);
		if(retval < 0)
			printx(ERROR, BUS, "Unable to initialize the semaphore\n");
	}
}

void PV(int i, int act)
{
	struct sembuf op;
	int retval;

	op.sem_num = i;
	op.sem_op = act; //1=V, -1=P
	op.sem_flg = 0; //will wait
	retval = semop(mySem, &op, 1);
	if(retval != 0)
		printx(ERROR, BUS, "Error semop will do %d\n", act);
}

void P(int i)
{
	PV(i, -1);
}

void V(int i)
{
	PV(i, 1);
}


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
	else if(strcmp(buffer, "setheaton2s") == 0)
	{
		printx(INFO, BUS, "Setting HEAT ON for 2 secs");
		setHeatTimer(2);
		setHeatWantedState(true);
	}
	else if(strcmp(buffer, "setheaton5s") == 0)
	{
		printx(INFO, BUS, "Setting HEAT ON for 5 secs");
		setHeatTimer(5);
		setHeatWantedState(true);
	}
	else if(strcmp(buffer, "setheaton10s") == 0)
	{
		printx(INFO, BUS, "Setting HEAT ON for 10 secs");
		setHeatTimer(10);
		setHeatWantedState(true);
	}
	else if(strcmp(buffer, "setheaton15s") == 0)
	{
		printx(INFO, BUS, "Setting HEAT ON for 15 secs");
		setHeatTimer(15);
		setHeatWantedState(true);
	}
	else if(strcmp(buffer, "setpumpon5s") == 0)
	{
		printx(INFO, BUS, "Setting PUMP ON for 5 secs");
		setPumpTimer(5);
		setPumpWantedState(true);
	}
	else if(strcmp(buffer, "setpumpon10s") == 0)
	{
		printx(INFO, BUS, "Setting PUMP ON for 10 secs");
		setPumpTimer(10);
		setPumpWantedState(true);
	}
	else if(strcmp(buffer, "autoheat") == 0)
	{
		printx(INFO, BUS, "Processing Auto Heat to 90°C");
		processHeat(NULL);
	}
	else if(strcmp(buffer, "stopautoheat") == 0)
	{
		printx(INFO, BUS, "Auto Heat has been stopped");
		stopAutoHeat();
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
		P(SEM_OUTPUT);
		printx(DEBUG, BUS, "Event receved !\n");
		busFree = false;
		lseek(bus, 0, SEEK_SET);
		printx(DEBUG, BUS, "Data read %d\n", read(bus, buffer, CMD_MAX));
		ftruncate(bus, 0);
		sync();
		processCmd(buffer);
		busFree = true;
		printx(DEBUG, BUS, buffer);
		V(SEM_INPUT);
		usleep(2);
	}
	return NULL;
}

bool initBus()
{
	create_sem(SEM_NUMBER);
	printx(ERROR, BUS, "Oh dear, something went wrong with %s !\n", strerror(errno));
	init_sem(SEM_INPUT);
	init_sem(SEM_OUTPUT);
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
	printx(DEBUG, BUS, "Control semaphore %d in %d out %d\n", mySem, get_sem(SEM_INPUT), get_sem(SEM_OUTPUT));
	P(SEM_INPUT);
	ftruncate(bus, 0);
	ftruncate(bus, CMD_MAX);
	lseek(bus, 0, SEEK_SET);
	dprintf(bus, "%s\n", cmd);
	V(SEM_OUTPUT);
}
