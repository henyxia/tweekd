#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <inttypes.h>
#include "ui.h"
#include "nfc.h"
#include "printx.h"

#define	DEVICE1				"/dev/ttyACM0"
#define	SPEED				B19200
#define NFC_POLLING_TIME	500000
#define	WAIT_BEFORE_RETRY	50000
#define	NFC_TAG_LENGTH		9
#define	GET_DEVICE_MODEL	'A'
#define	DEVICE_MODEL_NFC	'B'
#define	GET_NFC_STATUS		'C'
#define	NO_TAG_DETECTED		'D'
#define	PROFESSOR_TAG		'E'
#define	STUDENT_TAG			'F'
#define	GET_TAG_DETAILS		'H'

// Globals
int		serial_fd = -1;
bool	stop = false;
struct	termios saveterm;

bool init_serial(char* device, int speed)
{
	struct termios new;

	int fd=open(device,O_RDWR|O_NOCTTY|O_NONBLOCK);
	
	if(fd<0)
		return false;

	tcgetattr(fd,&saveterm); // save current port settings
	bzero(&new,sizeof(new));
	new.c_cflag=CLOCAL|CREAD|speed|CS8;
	new.c_iflag=0;
	new.c_oflag=0;
	new.c_lflag=0;  // set input mode (non-canonical, no echo,...)
	new.c_cc[VTIME]=0; // inter-character timer unused
	new.c_cc[VMIN]=1; // blocking read until 1 char received
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&new);

	serial_fd = fd;
	sleep(2);
	return true;
}

void close_serial()
{
	tcsetattr(serial_fd,TCSANOW,&saveterm);
	close(serial_fd);
}

unsigned char getData()
{
	unsigned char buf;
	while(read(serial_fd, &buf, 1) != 1)
		usleep(WAIT_BEFORE_RETRY);
		return buf;
}

void sendData(unsigned char data)
{
	while(write(serial_fd, &data, 1) != 1)
		usleep(WAIT_BEFORE_RETRY);
}

bool initNFC()
{
	unsigned char data;
	printx(DEBUG, NFC, "Connecting to interface 1\n");
	if(init_serial(DEVICE1, SPEED))
	{
		printx(DEBUG, NFC, "Connected to an interface\n");
		sendData(GET_DEVICE_MODEL);
		printx(DEBUG, NFC, "Sended identification request %02x\n", GET_DEVICE_MODEL);
		data = getData();
		if(data == DEVICE_MODEL_NFC)
			printx(INFO, NFC, "NFC Arduino connected\n");
		else
			printx(DEBUG, NFC, "NFC Arduino not found, response was %02x\n", data);
	}
	else
	{
		printx(WARNING, NFC, "Unable to connect to such interface\n");
		return false;
	}

	return true;
}

bool isTagPresent(char* tag)
{
	unsigned char data;
	sendData(GET_NFC_STATUS);
	data = getData();
	if(data == PROFESSOR_TAG)
	{
		sendData(GET_TAG_DETAILS);
		printx(INFO, NFC, "Professor Tag Detected %02x\n", data);
		return (read(serial_fd, tag, 6) == 6);
	}
	else if(data == STUDENT_TAG)
	{
		sendData(GET_TAG_DETAILS);
		printx(INFO, NFC, "Student Tag Detected\n");
		tag[0] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[0]);
		tag[1] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[1]);
		tag[2] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[2]);
		tag[3] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[3]);
		tag[4] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[4]);
		tag[5] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[5]);
		tag[6] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[6]);
		tag[7] = getData();
		printx(DEBUG, NFC, "Last read %02x\n", tag[7]);
		return true;
	}
	else if(data == NO_TAG_DETECTED)
	{
		printx(DEBUG, NFC, "No Tag Detected\n");
		return false;
	}
	else
	{
		printx(DEBUG, NFC, "Read data did not matched with expected data %02x\n", data);
		return false;
	}
	return true;
}

void* processNFC(void* we)
{
	char myTag[NFC_TAG_LENGTH];
	char buffer[NFC_TAG_LENGTH];

	while(!stop)
	{
		printx(DEBUG, NFC, "Scanning for tags\n");
		if(isTagPresent(buffer))
		{
			sprintf(myTag, "%s", buffer);
			printx(DEBUG, NFC, "Want to send %s as new tag\n", myTag);
			setTagName(myTag);
		}

		usleep(NFC_POLLING_TIME);
	}

	return NULL;
}
