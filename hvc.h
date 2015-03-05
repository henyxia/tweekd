#ifndef __HVC_H__
#define __HVC_H__

#include <stdbool.h>

bool	initHVC();
void*	processHVC();
void	stopHVC();
void	setPumpWantedState(bool);
void	setHeatWantedState(bool);

#endif
