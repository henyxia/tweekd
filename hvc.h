#ifndef __HVC_H__
#define __HVC_H__

#include <stdbool.h>

bool	initHVC();
void*	processHVC();
void	stopHVC();
void	setPumpWantedState(bool);
void	setHeatWantedState(bool);
void	setPumpTimer(double);
void	setHeatTimer(double);
void	setHeatOn();
void	setHeatOff();
void	setPumpOn();
void	setPumpOff();

#endif
