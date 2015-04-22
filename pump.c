#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "hvc.h"
#include "pump.h"

#define K_DEB 1925 //en imp/L  

void* processPump(void* arg)
{
	bool stopPump=false;
	bool heat_ok=false;
	bool coffee_wanted=false;
	int volDeb=0; // Volume du débitmetre en impulsion
	int volWanted=0;

	while (!stopPump)
	{
		if (heat_ok && coffee_wanted)
		{
			if (volDeb < volWanted)
			{
				setPumpOn();
			}
			
			else
			{
				setPumpOff();
				coffee_wanted=false;
				volDeb=0;
			}
		}
		sleep(1);
	}

	return NULL;
}
