#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "printx.h"
#include "hvc.h"
#include "heat.h"
#include "pump.h"


#define K_DEB 1925 //en imp/L
#define VOL_IMP_SHORT_COFFEE 97 //pour 5cl
#define VOL_IMP_LONG_COFFEE 231 //pour 12cl
#define TASK_NUMBER_PUMP 16


//Globals
bool stopPump=false;
bool coffee_wanted=false;
bool pump_on=false;
int volDeb=0; // Lecture volume du débitmetre en impulsion
int volWanted=0;
bool taskP[TASK_NUMBER_PUMP];
time_t tTaskP;
time_t tNowP;


void initProcessPump(void)
{
	taskP[0]=true;
	for (int i_initP=1;i_initP<TASK_NUMBER_PUMP;i_initP++)
		taskP[i_initP]=false;
}

void stopAutoPump(void)
{
	stopPump = true;
}

void setCoffeeWanted(void)
{
	coffee_wanted = true;
}

//Determine le volume en fonction de l'entier reçu (1 pour café court, 2 pour long, si autre : volume personalisé en cl)
void setVolWanted (int v)
{
	if (v == 1)
		volWanted = VOL_IMP_SHORT_COFFEE;
		
	else if (v == 2)
		volWanted = VOL_IMP_LONG_COFFEE;
		
	else if ((v > 5) && (v <= 20))
		volWanted = (v*K_DEB)/100;
		
	else
		volWanted = 0;
}

void actDeb(int v)
{
	volDeb = v;
}

bool isPumpOn(void)
{
	return pump_on;
}

void* processPump(void* arg)
{
	while (!stopPump)
	{
		tNowP = clock();
		
		if (taskP[0])
		{	
			if (coffee_wanted)
			{
				printx(INFO, BUS, "Demande de café prise en compte 1\n");
				taskP[0]=false;
				taskP[1]=true;
			}
		}
		
		else if (taskP[1])
		{
			if (isHeatOk())
			{
				printx(INFO, BUS, "Chauffe eau a bonne temperature OK 2\n");
				taskP[1]=false;
				taskP[3]=true;
			}
			
			else
			{
				printx(INFO, BUS, "Attente chauffe eau a bonne temperature 2'\n");
				taskP[1]=false;
				taskP[2]=true;
			}
		}
		
		else if (taskP[2])
		{
			if (isHeatOk())
			{
				taskP[2]=false;
				taskP[3]=true;
			}
		}
		
		else if (taskP[3])
		{
			tTaskP = clock();
			printx(INFO, BUS, "Distribution de l'eau Pompe ON 3\n");
			taskP[3]=false;
			taskP[4]=true;
		}
		
		else if (taskP[4])
		{
			setPumpOn();
			pump_on=true;
			
			if (volDeb > volWanted)
			{
				printx(INFO, BUS, "Fin de Distribution de l'eau Pompe OFF 4\n");
				taskP[4]=false;
				taskP[5]=true;
			}
				
			else if ( ((tNowP - tTaskP) / CLOCKS_PER_SEC) > 60 )
			{
				printx(INFO, BUS, "Pompe utilisée depuis plus d'1 min, mise en pause 4'\n");
				taskP[4]=false;
				taskP[6]=false;
			}
		}
		
		else if (taskP[5])
		{
			setPumpOff();
			pump_on=false;
			
			printx(INFO, BUS, "Cafe servi 5\n");
			volWanted=0;
			coffee_wanted=false;
			
			taskP[5]=false;
			taskP[0]=true;
		}
		
		else if (taskP[6])
		{
			setPumpOff();
			pump_on=false;
			
			printx(INFO, BUS, "En pause, Pompe OFF 6");
			
			tTaskP = clock();
			taskP[6]=false;
			taskP[7]=true;
		}
		
		else if (taskP[7])
		{
			if ( ((tNowP - tTaskP) / CLOCKS_PER_SEC) > 60 )
			{
				printx(INFO, BUS, "Pause terminée 7\n");
				taskP[7]=false;
				taskP[4]=true;
			}	
		}	
		
		sleep(50);
	}

	return NULL;
}
