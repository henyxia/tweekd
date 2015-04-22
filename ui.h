#ifndef __UI_H__
#define __UI_H__

#include <stdbool.h>

void*	drawUI(void*);
void	initUILog();
void	setDebit(unsigned int);
void	setHeat(bool);
void	setPump(bool);
void	setStartTime(char*);
void	setTagName(char*);
void	setTemp(unsigned int);
void	stopUI();

#endif
