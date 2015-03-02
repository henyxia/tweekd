#include <stdio.h>
#include "printx.h"

int main(void)
{
	initLog();
	printx(INFO, "Tweekd starting\n");
	closeLog();
	return 0;
}
