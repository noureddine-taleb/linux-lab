#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/ioctl.h"
#include <stdlib.h>
#include <stdio.h>

#define TIMER_FILE "/dev/timer"

int main(int argc, char **argv) {
	
	int fd;
	unsigned long pid;

	if (argc < 2) {
		printf("no enough args\n");
		exit(1);
	}

	fd = open(TIMER_FILE, O_RDONLY);

	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	pid = atoi(argv[1]);
	if (ioctl(fd, IOCTL_TIMER_MON, pid)) {
		perror("ioctl");
		return 	EXIT_FAILURE;
	}

	return 0;
}
