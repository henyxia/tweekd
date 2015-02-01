#include <stdio.h>
#include <version.h>
#include <transfer.h>

int main(void)
{
	printf("Tweekd starting\n");
	if(initializeProxmark() != 0)
	{
		printf("Initialization failed\n");
		return 1;
	}
	printf("[NFC]\t: HW version\n");
	proxVersion();
	stopProxmark();
	return 0;
}
