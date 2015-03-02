#ifndef __PRINTX_H__
#define __PRINTX_H__

#include <stdbool.h>

typedef enum
{
	ERROR, WARNING, INFO, DEBUG
}severity;

bool initLog();
void printx(severity, char*, ...);
void closeLog();

#endif
