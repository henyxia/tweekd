#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libusb-1.0/libusb.h>
#include "printx.h"
#include "usb.h"

#define VENDOR_ID			0xfccf
#define PRODUCT_ID			0xa001
#define ENDPOINT_OUT		0x01
#define	ENDPOINT_IN			0x82
#define	MAX_SIZE_OUT		64
#define HEIGHT				240
#define WIDTH				320
#define	STATUS_QUERY_LENGTH	11

libusb_device_handle*	screenHandle;
unsigned char			data[MAX_SIZE_OUT];
unsigned short			dataPointer = 0;

void blackScreen(void)
{
	unsigned char data[] = {0x81, 0x00, 0x00};
	int transfered;
	libusb_bulk_transfer(screenHandle, ENDPOINT_OUT, data, 3, &transfered, 0);
}

void fillDataWithHeader(void)
{
	data[dataPointer] = 0x82; // HEADER
	dataPointer++;
	data[dataPointer] = 0x00; // XPOS
	dataPointer++;
	data[dataPointer] = 0x00; // XPOS
	dataPointer++;
	data[dataPointer] = 0x00; // YPOS
	dataPointer++;
	data[dataPointer] = 0x00; // YPOS
	dataPointer++;
	data[dataPointer] = 0x40; // Width 0x40
	dataPointer++;
	data[dataPointer] = 0x01; // Width 0x01
	dataPointer++;
	data[dataPointer] = 0xF0; // Height 
	dataPointer++;
	data[dataPointer] = 0x00; // Height
	dataPointer++;
	data[dataPointer] = 0x00; // Copy
	dataPointer++;
}

void fillDataWithSubHeader(void)
{
	data[dataPointer] = 0x02; // HEADER
	dataPointer++;
}

void sendUSBData(int i)
{
	//printf("Sending data %d\n", i);
	int transfered;
	libusb_bulk_transfer(screenHandle, ENDPOINT_OUT, data, dataPointer, &transfered, 0);
	dataPointer = 0;
	fillDataWithSubHeader();
}

void displayPicture(char* filename)
{
	FILE* file = NULL;
	file = fopen(filename, "r");
	dataPointer = 0;

	if(file == NULL)
	{
		printf("Unable to open the picture\n");
		return;
	}
	else
		printf("Picture successfully opened\n");

	fillDataWithHeader();
	for(int i=0; i<HEIGHT*WIDTH*2; i++)
	{
		if(feof(file))
		{
			printf("File corrupted\n");
			return;
		}
		data[dataPointer] = getc(file);
		dataPointer++;
		if(dataPointer == MAX_SIZE_OUT)
			sendUSBData(i);
	}

	if(dataPointer != 0)
		sendUSBData(-1);

	printf("Screen updated\n");
}

bool initUSB(void)
{
	int				ret;

	ret = libusb_init(NULL);
	if(ret != 0)
	{
		printf("Error while initializing libusb, return : %d\n", ret);
		return false;
	}

	struct			libusb_config_descriptor* dConfig = NULL;
	libusb_device**	list = NULL;
	ssize_t			cnt = libusb_get_device_list(NULL, &list);

	struct libusb_device_descriptor dDevice;


	printf("Starting lsusb things\n");

	if(cnt < 0)
	{
		printf("Unable to get USB device list\n");
		return false;
	}

	printf("%d devices detected\n", (int)cnt);
	printf("List of compatible devices detected\n");

	for (int i = 0; i < cnt; i++)
	{
		libusb_device *device = list[i];
		ret = libusb_get_device_descriptor(device, &dDevice);
		if(VENDOR_ID == dDevice.idVendor && PRODUCT_ID == dDevice.idProduct)
		{
			printf("Bus %03d Device %03d: ID %04x:%04x\n",
					libusb_get_bus_number(device),
					libusb_get_device_address(device), dDevice.idVendor,
					dDevice.idProduct);
			ret = libusb_open(device, &screenHandle);
			if(ret != 0)
			{
				printf("Unable to open this device, error %d\n", ret);
				return false;
			}
		}
	}

	libusb_free_device_list(list, 1);

	ret = libusb_get_config_descriptor(libusb_get_device(screenHandle), 0, &dConfig);
	if(ret!=0)
	{
		printf("Descriptor for this device unavailable\n");
		return false;
	}
	else
	{
		for(int j=0; j<dConfig->bNumInterfaces; j++)
		{
			if(libusb_kernel_driver_active(screenHandle, j) && (libusb_detach_kernel_driver(screenHandle, j) != 0))
			{
				printf("Unable to detach this device\n");
				return false;
			}
		}
		ret = libusb_set_configuration(screenHandle, dConfig->bConfigurationValue);
		if(ret != 0)
		{
			printf("Configuration unavailable, error %d\n", ret);
			return false;
		}
		for(int j=0; j<dConfig->bNumInterfaces; j++)
			if(libusb_claim_interface(screenHandle, j) != 0)
			{
				printf("Device not claimed\n");
				return false;
			}
			else
				printf("Interface %d ready\n", j);
	}
	
	libusb_free_config_descriptor(dConfig);

	displayPicture("img/home.rgb");

	return true;
}

bool screenTouched(int* x, int* y)
{
	unsigned char	data[STATUS_QUERY_LENGTH];
	int				transfered;
	libusb_interrupt_transfer(screenHandle, ENDPOINT_IN, data, STATUS_QUERY_LENGTH, &transfered, 0);
	if(transfered != STATUS_QUERY_LENGTH)
	{
		printx(ERROR, UI, "Status recieved is %d bytes long, and expected %d\n", transfered, STATUS_QUERY_LENGTH);
		return false;
	}

	*x = data[3] + (256 * (int)data[4]);
	*y = data[7];

	printx(DEBUG, UI, "Screen %s at X %d Y %d\n", data[2] == 1 ? "touched" : "untouched", *x, *y);

	return data[2] == 1;
}
