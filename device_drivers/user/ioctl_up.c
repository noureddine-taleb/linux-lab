#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "../include/so2_cdev.h"

int main()
{
	int fd;
	char buf[BUFFER_SIZE];

	fd = open("/dev/so2_cdev", O_RDONLY);
	
	if (fd < 0) {
		perror("ioctl");
		return EXIT_FAILURE;
	}

	if (ioctl(fd, SO2_IOCTL_UP) < 0) {
		perror("ioctl");
		return EXIT_FAILURE;
	}
	
	return 0;
}
