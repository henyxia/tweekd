#include <stdio.h>
#include <pthread.h>
#include "printx.h"
#include "ui.h"

#include <unistd.h>

int main(void)
{
	pthread_t tUI, tNFC, tHVC;
	tNFC = 0;
	tHVC = 0;
	tHVC = tNFC;
	tNFC = tHVC;
	initUILog();
	if(!initLog())
	{
		printf("Log creation failed\n");
		return 1;
	}
	printx(INFO, "Tweekd starting\n");
	printx(DEBUG, "Creating processes\n");
	displayUI();
	close(1);// Black Magic Operating
	pthread_create(&tUI, NULL, drawUI, NULL);
	pthread_join(tUI, NULL);
	closeLog();
	return 0;
}
