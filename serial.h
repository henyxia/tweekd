#ifndef __SERIAL_H__
#define	__SERIAL_H__

bool init_serial(char*, int, int*, struct termios*);
void close_serial(int*, struct termios*);
unsigned char getData(int*);
void sendData(int*, unsigned char);

#endif
