#include <stdio.h>
#include <pthread.h>
#include "printx.h"
#include "ui.h"
#include "nfc.h"

#include <unistd.h>

int main(void)
{
	int ret;
	pthread_t tUI, tNFC;//tHVC
	initUILog();
	if(!initLog())
	{
		printf("Log creation failed\n");
		return 1;
	}
	printx(INFO, MAIN, "Tweekd starting\n");
	printx(DEBUG, MAIN, "Creating processes\n");
	displayUI();
	ret = pthread_create(&tUI, NULL, drawUI, NULL);
	printx(DEBUG, MAIN, "UI Started ID %08x ret %d\n", tUI, ret);
	printx(INFO, MAIN, "Initializing NFC");
	if(!initNFC())
	{
		printx(ERROR, MAIN, "Unable to start the NFC interface\n");
		return 2;
	}
	pthread_create(&tNFC, NULL, processNFC, NULL);
	pthread_join(tUI, NULL);
	closeLog();
	return 0;
}
