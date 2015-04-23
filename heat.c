#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "hvc.h"
#include "heat.h"
#include "printx.h"

#define TASK_NUMBER 32
#define T_INIT_THRESH 40
#define T_STEP_THRESH 80 //OP is 86
#define T_HOLD_THRESH 82 //OP is 88
#define INIT_HEAT_TIME 15
#define INIT_WAIT_TIME 15
#define STEP_HEAT_TIME 2
#define STEP_WAIT_TIME 8 //OP is 5
#define HOLD_HEAT_TIME 1
#define HOLD_WAIT_TIME 8 //OP is 4
#define HOLD_PUMP_HEAT_TIME 3
#define HOLD_PUMP_WAIT_TIME 5


//Globals
time_t tTask;
time_t tNow;
float tAct; //Lecture de la température du capteur
bool task[TASK_NUMBER];
bool stop=false;
bool heat_ok=false;
bool hold_heat=true;
bool pumpitup=false; //Lecture de l'état de la pompe
bool eco_mode=false;
bool stopHeat=false;


void initProcessHeat(void)
{
	//Initialisation du tableau task
	task[0]=true;
	for (int i_initH=1;i_initH<TASK_NUMBER;i_initH++)
		task[i_initH]=false;
}

void actTemp(float temp)
{
	tAct = temp;
}

void stopAutoHeat(void)
{
	stopHeat = true;
}

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
				printx(INFO, BUS, "Début chauffe initiale 0");
				task[0] = false;
				task[1] = true;
			}
			
			else
			{
				task[0] = false;
				task[6] = true;
			}
		}
		
		else if (task[1])
		{
			tTask = clock();
			task[1] = false;
			task[2] = true;
		}
		
		else if (task[2])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > INIT_HEAT_TIME )
			{
				setHeatOff();
				printx(INFO, BUS, "Fin chauffe initiale 1");
				task[2] = false;
				task[3] = true;
			}
		}
		
		else if (task[3])
		{
			tTask = clock();
			task[3] = false;
			task[4] = true;
		}
		
		else if (task[4])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > INIT_WAIT_TIME )
			{
				printx(INFO, BUS, "Fin attente %d sec 2", INIT_WAIT_TIME);
				task[4] = false;
				task[5] = true;
			}
		}
		
		
		//Chauffe par palier
		else if (task[5])
		{
			tTask = clock();
			task[5] = false;
			task[6] = true;
		}
		
		
		else if (task[6])
		{
			if (tAct < T_STEP_THRESH)
			{
				printx(INFO, BUS, "Début Chauffe par palier 3");
				setHeatOn();
				task[6] = false;
				task[7] = true;
			}
			
			else
			{
				task[6] = false;
				task[11] = true; //Numéro de tache début "Maintien au chaud"
			}
		}
		
		else if (task[7])
		{
			tTask = clock();
			task[7] = false;
			task[8] = true;
		}
		
		else if (task[8])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > STEP_HEAT_TIME )
			{
				printx(INFO, BUS, "Fin de chauffe palier intermédiaire 4");
				setHeatOff();
				task[8] = false;
				task[9] = true;
			}
		}
		
		else if (task[9])
		{
			tTask = clock();
			task[9] = false;
			task[10] = true;
		}
		
		else if (task[10])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > STEP_WAIT_TIME )
			{
				printx(INFO, BUS, "Fin d'attente palier intermédiaire 5");
				
				if ( tAct < T_STEP_THRESH )
				{
					task[10] = false;
					task[5] = true;
				}
				
				else
				{
					task[10] = false;
					task[11] = true;
				}
			}
		}
		
		
		// Maintien au chaud
		else if (task[11])
		{
			tTask = clock();
			task[11] = false;
			task[12] = true;
		}
	
		else if (task[12])
		{
			printx(INFO, BUS, "Début Maintien au chaud 6");
			heat_ok = true;
			task[12]=false;
			task[13]=true;
		}
		
		else if (task[13])
		{
			tTask = clock();
			task[13] = false;
			task[14] = true;
		}
		
		else if (task[14])
		{			
			if(hold_heat)
			{
				if( (tAct < T_HOLD_THRESH))
				{
					if (!pumpitup)
					{
						printx(INFO, BUS, "Début Chauffe maintien sans pompe 7");
						setHeatOn();
						task[14] = false;
						task[15] = true;
					}
			
					else 
					{
						printx(INFO, BUS, "Début Chauffe maintien avec pompe 7'");
						setHeatOn();
						task[14]=false;
						task[19]=true; //Numéro de tache "T<Thold && pumpitup"
					}
				}
			
			}
			
			else
			{
				printx(INFO, BUS, "Maintien au chaud annulé 7 ' ' Go etape de fin");
				task[14]=false;
				task[23]=true; //Sinon go étape d'attente fin
			}
		}
		
		else if (task[15])
		{
			tTask = clock();
			task[15] = false;
			task[16] = true;
		}
		
			//Maintien au chaud sans pompage
		else if (task[16])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_HEAT_TIME )
			{
				printx(INFO, BUS, "Fin chauffe maintien sans pompe 8");
				setHeatOff();
				task[16] = false;
				task[17] = true;
			}
		}
		
		else if (task[17])
		{
			tTask = clock();
			task[17] = false;
			task[18] = true;
		}
		
		else if (task[18])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_WAIT_TIME )
			{
				if ( tAct < T_HOLD_THRESH)
				{
					printx(INFO, BUS, "Fin d'attente chauffe intermédiaire maintien 9");
					task[18] = false;	
					task[11] = true; //Retour au début de "Maintien au chaud" 
				}
			}
		}
		
			//Maintien au chaud avec pompage
		else if (task[19])
		{
			tTask = clock();
			task[19] = false;
			task[20] = true;
		}	
			
		else if (task[20])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_PUMP_HEAT_TIME )
			{
				printx(INFO, BUS, "Fin chauffe maintien avec pompe 10");
				setHeatOff();
				task[20] = false;
				task[21] = true;
			}
		}
		
		else if (task[21])
		{
			tTask = clock();
			task[21] = false;
			task[22] = true;
		}
		
		else if (task[22])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_PUMP_WAIT_TIME )
			{
				if ( tAct < T_HOLD_THRESH)
				{
					printx(INFO, BUS, "Fin d'attente chauffe intermédiaire maintien avec pompe 11");
					task[22] = false;
					task[11] = true; //Retour au début de "Maintien au chaud" 
				}			
			}
		}

			//Fin de maintien au chaud (demandé par l'extérieur) et reboucle quand hold_heat est remis
		else if(task[23])
		{
			setHeatOff();
			heat_ok=false;
			
			if(hold_heat)
			{
				task[23]=false;
				task[0]=true; //Retour au début
			}
		}
		
		
		//Mode économie d'énergie
		else if(task[24])
		{
				//task[]=false;
				//task[]=true;
		}
		
		
	}
	
	
	return NULL;
}
