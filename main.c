#include <stdio.h>
#include "printx.h"
#include "ui.h"

int main(void)
{
	initUILog();
	if(!initLog())
	{
		printf("Log creation failed\n");
		return 1;
	}
	printx(INFO, "Tweekd starting\n");
	displayUI();
	closeLog();
	return 0;
}
