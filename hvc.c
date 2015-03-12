#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <inttypes.h>
#include <time.h>
#include "ui.h"
#include "hvc.h"
#include "printx.h"
#include "serial.h"

#define	DEVICE1				"/dev/ttyACM1"
#define	SPEED				B19200
#define HVC_POLLING_TIME	50000
#define	WAIT_BEFORE_RETRY	5000
#define	GET_DEVICE_MODEL	'A'
#define	DEVICE_MODEL_HVC	'C'
#define	GET_TEMP			'H'
#define	SET_HEAT_ON			'E'
#define	SET_HEAT_OFF		'G'
#define	SET_PUMP_ON			'B'
#define	SET_PUMP_OFF		'D'

// Globals
int		hvc_fd = -1;
bool	hvcStop = false;
bool	sPump = false;
bool	sHeat = false;
struct	termios hvcSaveterm;
bool	wHeat = false;
bool	wPump = false;
time_t	tHeatStart;
time_t	tHeatStop;
double	tHeatTimer;
time_t	tPumpStart;
double	tPumpTimer;
time_t	tPumpStop;

void stopHVC()
{
	hvcStop = true;
}

void setPumpWantedState(bool s)
{
	tPumpStart = clock();
	tPumpStop = clock();
	wPump = s;
}

void setHeatWantedState(bool s)
{
	tHeatStart = clock();
	tHeatStop = clock();
	wHeat = s;
}

void setPumpTimer(double t)
{
	tPumpTimer = t;
}

void setHeatTimer(double t)
{
	tHeatTimer = t;
}

bool initHVC()
{
	unsigned char data;
	printx(DEBUG, HVC, "Connecting to interface 1\n");
	if(init_serial(DEVICE1, SPEED, &hvc_fd, &hvcSaveterm))
	{
		printx(DEBUG, HVC, "Connected to an interface\n");
		sendData(&hvc_fd, GET_DEVICE_MODEL);
		printx(DEBUG, HVC, "Sended identification request %02x\n", GET_DEVICE_MODEL);
		data = getData(&hvc_fd);
		if(data == DEVICE_MODEL_HVC)
			printx(INFO, HVC, "HVC Arduino connected\n");
		else
			printx(DEBUG, HVC, "HVC Arduino not found, response was %02x\n", data);
	}
	else
	{
		printx(WARNING, HVC, "Unable to connect to such interface\n");
		return false;
	}

	return true;
}

void* processHVC(void* we)
{
	uint8_t data;

	while(!hvcStop)
	{
		printx(DEBUG, HVC, "Querying data\n");
		sendData(&hvc_fd, GET_TEMP);
		data = getData(&hvc_fd);
		setTemp(data);
		printx(DEBUG, HVC, "TEMP %f\n", 25 + (((float)(80*data)))/255);
		if(tHeatTimer > 0)
		{
			tHeatStop = clock();
			if(((double)(tHeatStop - tHeatStart) / CLOCKS_PER_SEC) > tHeatTimer)
			{
				wHeat = false;
				tHeatTimer = 0;
			}

		}

		if(tPumpTimer > 0)
		{
			tPumpStop = clock();
			if(((double)(tPumpStop - tPumpStart) / CLOCKS_PER_SEC) > tPumpTimer)
			{
				wPump = false;
				tPumpTimer = 0;
			}
		}

		if(wHeat ^ sHeat)
		{
			sendData(&hvc_fd, wHeat ? SET_HEAT_ON : SET_HEAT_OFF);
			sHeat = wHeat;
			setHeat(sHeat);
			if(sHeat)
				tHeatStart = clock();
		}

		if(wPump ^ sPump)
		{
			sendData(&hvc_fd, wPump ? SET_PUMP_ON : SET_PUMP_OFF);
			sPump = wPump;
			setPump(sPump);
			if(sPump)
				tPumpStart = clock();
		}

		usleep(HVC_POLLING_TIME);
	}

	return NULL;
}
