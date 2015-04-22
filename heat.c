#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "hvc.h"
#include "heat.h"

#define TASK_NUMBER 20
#define T_INIT_THRESH 400
#define T_STEP_THRESH 860
#define T_HOLD_THRESH 880
#define INIT_HEAT_TIME 15
#define INIT_WAIT_TIME 15
#define STEP_HEAT_TIME 2
#define STEP_WAIT_TIME 5
#define HOLD_HEAT_TIME 1
#define HOLD_WAIT_TIME 2
#define HOLD_PUMP_HEAT_TIME 3
#define HOLD_PUMP_WAIT_TIME 1


//Globals
time_t tTask;
time_t tNow;
time_t tAct; //Lecture de la température du capteur
bool task[TASK_NUMBER]; //A initialiser (task[0]=true, sinon false)
	/*for (int i=0;i<TASK_NUMBER;i++)
	{
		if (i == 0)
			task[i]=true;
		else
			task[i]=false;
	}*/
bool stop=false;
bool heat_ok=false;
bool hold_heat=true;
bool pumpitup=false; //Lecture de l'état de la pompe
bool eco_mode=false;
bool stopHeat=false;

void* processHeat(void* arg)
{
	while(!stopHeat)
	{
		tNow = clock();
		
		//Chaufffe initiale
		if (task[0])
		{
			tTask = clock();
			if (tAct < T_INIT_THRESH)
			{
				setHeatOn();
				task[0] = false;
				task[1] = true;
			}
			
			else
			{
				task[0] = false;
				task[3] = true;
			}
		}
		
		else if (task[1])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > INIT_HEAT_TIME )
			{
				setHeatOff();
				task[1] = false;
				task[2] = true;
			}
		}
		
		else if (task[2])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > INIT_WAIT_TIME )
			{
				task[2] = false;
				task[3] = true;
			}
		}
		
		//Chauffe par paliers
		else if (task[3])
		{
			if (tAct < T_STEP_THRESH)
			{
				setHeatOn();
				task[3] = false;
				task[4] = true;
			}
			
			else
			{
				task[3] = false;
				task[6] = true; //Numéro de tache début "Maintien au chaud"
			}
		}
		
		else if (task[4])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > STEP_HEAT_TIME )
			{
				setHeatOff();
				task[4] = false;
				task[5] = true;
			}
		}
		
		else if (task[5])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > STEP_WAIT_TIME )
			{
				if ( tAct < T_STEP_THRESH )
				{
					task[5] = false;
					task[3] = true;
				}
				
				else
				{
					task[5] = false;
					task[6] = true;
				}
			}
		}
		
		// Maintien au chaud
		else if (task[6])
		{
			heat_ok = true;
			task[6]=false;
			task[7]=true;
		}
		
		else if (task[7])
		{			
			if(hold_heat)
			{
				if( (tAct < T_HOLD_THRESH) && !pumpitup)
				{
					setHeatOn();
					task[7] = false;
					task[8] = true;
				}
			
				else if ( (tAct < T_HOLD_THRESH) && pumpitup)
				{
					setHeatOn();
					task[7]=false;
					task[10]=true; //Numéro de tache "T<Thold && pumpitup"
				}
			
			}
			
			else
			{
				task[6]=false;
				task[12]=true; //Sinon go étape d'attente fin
			}
		}
		
			//Maintien au chaud sans pompage
		else if (task[8])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_HEAT_TIME )
			{
				setHeatOff();
				task[8] = false;
				task[9] = true;
			}
		}
		
		else if (task[9])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_WAIT_TIME )
			{
				if ( tAct < T_HOLD_THRESH)
				{
					task[9] = false;	
					task[7] = true; //Retour au début de "Maintien au chaud" 
				}
			}
		}
		
			//Maintien au chaud avec pompage
		else if (task[10])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_PUMP_HEAT_TIME )
			{
				setHeatOff();
				task[10] = false;
				task[11] = true;
			}
		}
		
		else if (task[11])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_PUMP_WAIT_TIME )
			{
				if ( tAct < T_HOLD_THRESH)
				{
					task[11] = false;
					task[7] = true; //Retour au début de "Maintien au chaud" 
				}			
			}
		}
		
			//Fin de maintien au chaud (demandé par l'extérieur) et reboucle quand hold_heat est remis
		else if(task[12])
		{
			setHeatOff();
			heat_ok=false;
			
			if(hold_heat)
			{
				task[12]=false;
				task[1]=true; //Retour au début
			}
		}
		
		
		//Mode économie d'énergie
		else if(task[13])
		{
				//task[]=false;
				//task[]=true;
		}
		
		
	}
	
	
	return NULL;
}
