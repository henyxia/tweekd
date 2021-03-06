#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "hvc.h"
#include "heat.h"
#include "pump.h"
#include "printx.h"

#define TASK_NUMBER 32
#define T_INIT_THRESH 40
#define T_STEP_THRESH 80
#define T_HOLD_THRESH 88
#define T_ECO_MODE_THRESH 50
#define INIT_HEAT_TIME 15
#define INIT_WAIT_TIME 12
#define STEP_HEAT_TIME 2
#define STEP_WAIT_TIME 6
#define HOLD_HEAT_TIME 1
#define HOLD_WAIT_TIME 8
#define HOLD_PUMP_HEAT_TIME 3
#define HOLD_PUMP_WAIT_TIME 5


//Globals
time_t tTask;
time_t tNow;
float tAct;
bool task[TASK_NUMBER];
bool stop=false;
bool hold_heat=true;
bool eco_mode=false;
bool stopHeat=false;
bool heat_ok=false;

void initProcessHeat(void)
{
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

void setHeatOk(void)
{
	heat_ok=true;
}

void resetHeatOk(void)
{
	heat_ok=false;
}

bool isHeatOk(void)
{
	heat_ok = (tAct > T_HOLD_THRESH);
	return heat_ok;
}

void* processHeat(void* arg)
{
	while(!stopHeat)
	{
		tNow = clock();

		//Chaufffe initiale
		if (task[0])
		{
			printx(INFO, HEAT, "Entering in task 0\n");
			tTask = clock();

			if (tAct < T_INIT_THRESH)
			{
				setHeatOn();
				printx(INFO, HEAT, "D�but chauffe initiale 0\n");
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
			printx(INFO, HEAT, "Entering in task 1\n");
			tTask = clock();
			task[1] = false;
			task[2] = true;
		}

		else if (task[2])
		{
			printx(INFO, HEAT, "Entering in task 2\n");
			printx(INFO, HEAT, "Time heating : %d (%d - %d) / %d\n", (tNow - tTask) / CLOCKS_PER_SEC, tNow, tTask, CLOCKS_PER_SEC);
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > INIT_HEAT_TIME )
			{
				setHeatOff();
				printx(INFO, HEAT, "Fin chauffe initiale 1\n");
				task[2] = false;
				task[3] = true;
			}
		}

		else if (task[3])
		{
			printx(INFO, HEAT, "Entering in task 3\n");
			tTask = clock();
			task[3] = false;
			task[4] = true;
		}

		else if (task[4])
		{
			printx(INFO, HEAT, "Entering in task 4\n");
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > INIT_WAIT_TIME )
			{
				printx(INFO, HEAT, "Fin attente %d sec 2\n", INIT_WAIT_TIME);
				task[4] = false;
				task[5] = true;
			}
		}


		//Chauffe par palier
		else if (task[5])
		{
			printx(INFO, HEAT, "Entering in task 5\n");
			tTask = clock();
			task[5] = false;
			task[6] = true;
		}


		else if (task[6])
		{
			printx(INFO, HEAT, "Entering in task 6\n");
			if (tAct < T_STEP_THRESH)
			{
				printx(INFO, HEAT, "D�but Chauffe par palier 3\n");
				setHeatOn();
				task[6] = false;
				task[7] = true;
			}

			else
			{
				task[6] = false;
				task[11] = true; //Num�ro de tache d�but "Maintien au chaud"
			}
		}

		else if (task[7])
		{
			printx(INFO, HEAT, "Entering in task 7\n");
			tTask = clock();
			task[7] = false;
			task[8] = true;
		}

		else if (task[8])
		{
			printx(INFO, HEAT, "Entering in task 8\n");
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > STEP_HEAT_TIME )
			{
				printx(INFO, HEAT, "Fin de chauffe palier interm�diaire 4\n");
				setHeatOff();
				task[8] = false;
				task[9] = true;
			}
		}

		else if (task[9])
		{
			printx(INFO, HEAT, "Entering in task 9\n");
			tTask = clock();
			task[9] = false;
			task[10] = true;
		}

		else if (task[10])
		{
			printx(INFO, HEAT, "Entering in task 10\n");
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > STEP_WAIT_TIME )
			{
				printx(INFO, HEAT, "Fin d'attente palier interm�diaire 5\n");

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
			printx(INFO, HEAT, "Entering in task 11\n");
			tTask = clock();

			if (eco_mode)
            {
                task[11] = false;
                task[24] = true; //Num�ro tache Mode �co
            }

            else
            {
                task[11] = false;
                task[12] = true;
            }
		}

		else if (task[12])
		{
			printx(INFO, HEAT, "Entering in task 12\n");
			printx(INFO, HEAT, "D�but Maintien au chaud 6\n");
			setHeatOk();
			task[12]=false;
			task[13]=true;
		}

		else if (task[13])
		{
			printx(INFO, HEAT, "Entering in task 13\n");
			tTask = clock();
			task[13] = false;
			task[14] = true;
		}

		else if (task[14])
		{
			printx(INFO, HEAT, "Entering in task 14\n");
			if(hold_heat)
			{
				if( (tAct < T_HOLD_THRESH))
				{
					if (!isPumpOn())
					{
						printx(INFO, HEAT, "D�but Chauffe maintien sans pompe 7\n");
						setHeatOn();
						task[14] = false;
						task[15] = true;
					}

					else
					{
						printx(INFO, HEAT, "D�but Chauffe maintien avec pompe 7'\n");
						setHeatOn();
						task[14]=false;
						task[19]=true; //Num�ro de tache "T<Thold && pumpitup"
					}
				}

			}

			else
			{
				printx(INFO, HEAT, "Maintien au chaud annul� 7 ' ' Go etape de fin\n");
				task[14]=false;
				task[23]=true; //Sinon go �tape d'attente fin
			}
		}

		else if (task[15])
		{
			printx(INFO, HEAT, "Entering in task 15\n");
			tTask = clock();
			task[15] = false;
			task[16] = true;
		}

			//Maintien au chaud sans pompage
		else if (task[16])
		{
			printx(INFO, HEAT, "Entering in task 16\n");
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_HEAT_TIME )
			{
				printx(INFO, HEAT, "Fin chauffe maintien sans pompe 8\n");
				setHeatOff();
				task[16] = false;
				task[17] = true;
			}
		}

		else if (task[17])
		{
			printx(INFO, HEAT, "Entering in task 17\n");
			tTask = clock();
			task[17] = false;
			task[18] = true;
		}

		else if (task[18])
		{
			printx(INFO, HEAT, "Entering in task 18\n");
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_WAIT_TIME )
			{
				if ( tAct < T_HOLD_THRESH)
				{
					printx(INFO, HEAT, "Fin d'attente chauffe interm�diaire maintien 9\n");
					task[18] = false;
					task[11] = true; //Retour au d�but de "Maintien au chaud"
				}
			}
		}

			//Maintien au chaud avec pompage
		else if (task[19])
		{
			printx(INFO, HEAT, "Entering in task 19\n");
			tTask = clock();
			task[19] = false;
			task[20] = true;
		}

		else if (task[20])
		{
			printx(INFO, HEAT, "Entering in task 20\n");
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_PUMP_HEAT_TIME )
			{
				printx(INFO, HEAT, "Fin chauffe maintien avec pompe 10\n");
				setHeatOff();
				task[20] = false;
				task[21] = true;
			}
		}

		else if (task[21])
		{
			printx(INFO, HEAT, "Entering in task 21\n");
			tTask = clock();
			task[21] = false;
			task[22] = true;
		}

		else if (task[22])
		{
			printx(INFO, HEAT, "Entering in task 22\n");
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_PUMP_WAIT_TIME )
			{
				if ( tAct < T_HOLD_THRESH)
				{
					printx(INFO, HEAT, "Fin d'attente chauffe interm�diaire maintien avec pompe 11\n");
					task[22] = false;
					task[5] = true; //Retour au d�but de "Chauffe par palier" (shuntera si temperature OK (t>T_HOLD_THRESH))
				}
			}
		}

			//Fin de maintien au chaud (demand� par l'ext�rieur) et reboucle quand hold_heat est remis
		else if(task[23])
		{
			printx(INFO, HEAT, "Entering in task 23\n");
			setHeatOff();
			resetHeatOk();

			if(hold_heat)
			{
				task[23]=false;
				task[0]=true; //Retour au d�but
			}
		}


		//Mode �conomie d'�nergie
		else if(task[24])
		{
		    printx(INFO, HEAT, "Entering in task 24\n");
		    printx(INFO, HEAT, "D�but Mode Eco maintien 24\n");

			if (eco_mode)
            {
                if (tAct < T_ECO_MODE_THRESH)
                {
                    printx(INFO, HEAT, "Debut chauffe maintien Eco Mode 25\n");
                    setHeatOn();
                    task[24] = false;
                    task[25] = true;
                }
            }

            else
            {
                task[24] = false;
                task[5] = true; //Retour "Chauffe par palier"
            }
		}

		else if (task[25])
		{
			tTask = clock();
			task[25] = false;
			task[26] = true;
		}

        else if (task[26])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_HEAT_TIME )
			{
				printx(INFO, HEAT, "Fin chauffe maintien Eco mode 27\n");
				setHeatOff();
				task[26] = false;
				task[27] = true;
			}
		}

		else if (task[27])
		{
			tTask = clock();
			task[27] = false;
			task[28] = true;
		}

		else if (task[28])
		{
			if( ((tNow - tTask) / CLOCKS_PER_SEC) > HOLD_WAIT_TIME )
			{
					printx(INFO, HEAT, "Fin d'attente maintien Eco mode 29\n");
					task[28] = false;
					task[24] = true;
            }
        }

		usleep(20000);
    }


	return NULL;
}
