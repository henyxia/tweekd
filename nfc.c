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
#include "serial.h"

#define	DEVICE1				"/dev/ttyACM0"
#define	SPEED				B19200
#define NFC_POLLING_TIME	500000
#define	NFC_TAG_LENGTH		9
#define	GET_DEVICE_MODEL	'A'
#define	DEVICE_MODEL_NFC	'B'
#define	GET_NFC_STATUS		'C'
#define	NO_TAG_DETECTED		'D'
#define	PROFESSOR_TAG		'E'
#define	STUDENT_TAG			'F'
#define	GET_TAG_DETAILS		'H'

// Globals
int		nfc_fd = -1;
bool	nfcStop = false;
struct	termios saveterm;

bool initNFC()
{
	unsigned char data;
	printx(DEBUG, NFC, "Connecting to interface 1\n");
	if(init_serial(DEVICE1, SPEED, &nfc_fd, &saveterm))
	{
		printx(DEBUG, NFC, "Connected to an interface\n");
		sendData(&nfc_fd, GET_DEVICE_MODEL);
		printx(DEBUG, NFC, "Sended identification request %02x\n", GET_DEVICE_MODEL);
		data = getData(&nfc_fd);
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

void stopNFC()
{
	close_serial(&nfc_fd, &saveterm);
	nfcStop = true;
}

bool isTagPresent(char* tag)
{
	unsigned char data;
	sendData(&nfc_fd, GET_NFC_STATUS);
	data = getData(&nfc_fd);
	if(data == PROFESSOR_TAG)
	{
		sendData(&nfc_fd, GET_TAG_DETAILS);
		//printx(INFO, NFC, "Professor Tag Detected %02x\n", data);
		//return (read(serial_fd, tag, 6) == 6);
	}
	else if(data == STUDENT_TAG)
	{
		sendData(&nfc_fd, GET_TAG_DETAILS);
		printx(INFO, NFC, "Student Tag Detected\n");
		tag[0] = getData(&nfc_fd);
		printx(DEBUG, NFC, "Last read %02x\n", tag[0]);
		tag[1] = getData(&nfc_fd);
		printx(DEBUG, NFC, "Last read %02x\n", tag[1]);
		tag[2] = getData(&nfc_fd);
		printx(DEBUG, NFC, "Last read %02x\n", tag[2]);
		tag[3] = getData(&nfc_fd);
		printx(DEBUG, NFC, "Last read %02x\n", tag[3]);
		tag[4] = getData(&nfc_fd);
		printx(DEBUG, NFC, "Last read %02x\n", tag[4]);
		tag[5] = getData(&nfc_fd);
		printx(DEBUG, NFC, "Last read %02x\n", tag[5]);
		tag[6] = getData(&nfc_fd);
		printx(DEBUG, NFC, "Last read %02x\n", tag[6]);
		tag[7] = getData(&nfc_fd);
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
	return false;
}

void* processNFC(void* we)
{
	char myTag[NFC_TAG_LENGTH];
	char buffer[NFC_TAG_LENGTH];

	while(!nfcStop)
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
