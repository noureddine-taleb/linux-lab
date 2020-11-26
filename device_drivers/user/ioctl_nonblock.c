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

	fd = open("/dev/so2_cdev", O_RDONLY|O_NONBLOCK);
	
	if (fd < 0) {
		perror("ioctl");
		return EXIT_FAILURE;
	}

	if (read(fd, buf, sizeof buf) < 0) {
		perror("ioctl");
		return EXIT_FAILURE;
	}
	
	write(1, buf, sizeof buf);

	return 0;
}
