#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libusb-1.0/libusb.h>
#include "usb.h"

#define VENDOR_ID		0xfccf
#define PRODUCT_ID		0xa001
#define ENDPOINT_OUT	0x01
#define	MAX_SIZE_OUT	64

libusb_device_handle* screenHandle;

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

	printf("%d devices detected\n", cnt);
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

	return true;
}

