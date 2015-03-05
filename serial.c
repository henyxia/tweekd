#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <strings.h>
#include "serial.h"

#define	WAIT_BEFORE_RETRY	50000

bool init_serial(char* device, int speed, int *serial_fd, struct termios *saveterm)
{
	struct termios new;

	int fd=open(device,O_RDWR|O_NOCTTY|O_NONBLOCK);
	
	if(fd<0)
		return false;

	tcgetattr(fd,saveterm); // save current port settings
	bzero(&new,sizeof(new));
	new.c_cflag=CLOCAL|CREAD|speed|CS8;
	new.c_iflag=0;
	new.c_oflag=0;
	new.c_lflag=0;  // set input mode (non-canonical, no echo,...)
	new.c_cc[VTIME]=0; // inter-character timer unused
	new.c_cc[VMIN]=1; // blocking read until 1 char received
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&new);

	*serial_fd = fd;
	sleep(2);
	return true;
}

void close_serial(int *serial_fd, struct termios *saveterm)
{
	tcsetattr(*serial_fd,TCSANOW,saveterm);
	close(*serial_fd);
}

unsigned char getData(int *serial_fd)
{
	unsigned char buf;
	while(read(*serial_fd, &buf, 1) != 1)
		usleep(WAIT_BEFORE_RETRY);
		return buf;
}

void sendData(int *serial_fd, unsigned char data)
{
	while(write(*serial_fd, &data, 1) != 1)
		usleep(WAIT_BEFORE_RETRY);
}

