#ifndef __PUMP_H__
#define __PUMP_H__

void initProcessPump(void);
void stopAutoPump(void);
void actDeb(int);
void setCoffeeWanted(void);
void setVolWanted(int);
bool isPumpOn(void);
void* processPump(void*);

#endif
