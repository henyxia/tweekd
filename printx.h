#ifndef __PRINTX_H__
#define __PRINTX_H__

#include <stdbool.h>

typedef enum
{
	ERROR, WARNING, INFO, DEBUG
}severity;

typedef enum
{
	MAIN, UI, NFC, HVC, BUS
}msgfrom;

bool initLog();
void printx(severity, msgfrom, char*, ...);
void closeLog();

#endif
