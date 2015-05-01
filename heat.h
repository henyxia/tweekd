#ifndef __HEAT_H__
#define __HEAT_H__

void initProcessHeat(void);
void* processHeat(void*);
void actTemp(float);
void stopAutoHeat(void);
void setHeatOk(void);
void resetHeatOk(void);
bool isHeatOk(void);

#endif
