#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "printx.h"
#include "ui.h"
#include "nfc.h"
#include "bus.h"
#include "hvc.h"
#include "heat.h"
#include "pump.h"

int main(void)
{
	int ret;
	pthread_t tUI;
	pthread_t tNFC;
	pthread_t tBUS;
	pthread_t tHVC;
	pthread_t tHeat;
	pthread_t tPump;
	initUILog();
	if(!initLog())
	{
		printf("Log creation failed\n");
		return 1;
	}
	printx(INFO, MAIN, "Tweekd starting\n");
	printx(DEBUG, MAIN, "Creating processes\n");
	ret = pthread_create(&tUI, NULL, drawUI, NULL);
	printx(DEBUG, MAIN, "UI Started ID %08x ret %d\n", tUI, ret);
	printx(INFO, MAIN, "Initializing NFC\n");
	if(!initNFC())
	{
		printx(ERROR, MAIN, "Unable to start the NFC interface\n");
		return 2;
	}
	pthread_create(&tNFC, NULL, processNFC, NULL);
	if(!initBus())
	{
		printx(ERROR, MAIN, "Unable to start the BUS interface\n");
		return 3;
	}
	pthread_create(&tBUS, NULL, processBus, NULL);
	if(!initHVC())
	{
		printx(ERROR, MAIN, "Unable to start the HVC interface\n");
		return 4;
	}
	pthread_create(&tHVC, NULL, processHVC, NULL);
	initProcessHeat();
	initProcessPump();
	pthread_create(&tHeat, NULL, processHeat, NULL);
	pthread_create(&tPump, NULL, processPump, NULL);

	sleep(20);
	setVolWanted(1);
	setCoffeeWanted();

	pthread_join(tUI, NULL);
	closeLog();
	return 0;
}
