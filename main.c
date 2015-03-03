#include <stdio.h>
#include <pthread.h>
#include "printx.h"
#include "ui.h"

#include <unistd.h>

int main(void)
{
	int ret;
	pthread_t tUI;//, tNFC;//tHVC
	initUILog();
	if(!initLog())
	{
		printf("Log creation failed\n");
		return 1;
	}
	printx(INFO, "Tweekd starting\n");
	printx(DEBUG, "Creating processes\n");
	displayUI();
	ret = pthread_create(&tUI, NULL, drawUI, NULL);
	printx(DEBUG, "UI Started ID %08x ret %d\n", tUI, ret);
	printx(INFO, "Initializing NFC");
	//pthread_create(&tNFC, NULL, 
	pthread_join(tUI, NULL);
	closeLog();
	return 0;
}
