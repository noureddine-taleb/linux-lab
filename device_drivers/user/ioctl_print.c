#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../include/so2_cdev.h"

int main()
{
	int fd;
	fd = open("/dev/so2_cdev", O_RDONLY);
	if (fd < 0)
		return EXIT_FAILURE;

	if (ioctl(fd, SO2_IOCTL_PRINT) < 0)
		return EXIT_FAILURE;

	return 0;
}
