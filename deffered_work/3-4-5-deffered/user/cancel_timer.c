#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/ioctl.h"

#define TIMER_FILE "/dev/timer"

int main(){
	
	int fd;
	unsigned long expired;
	expired = 3;

	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	if (ioctl(fd, ,IOCTL_TIMER_SET expired)) {
		perror("ioctl");
		return 	EXIT_FAILURE;
	}
	
	return 0;
}
