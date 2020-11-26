#ifndef __SO2CDEV_H__
#define __SO2CDEV_H__

#include <asm/ioctl.h>

#define BUFFER_SIZE 256
#define SO2_IOCTL_PRINT _IOC(_IOC_NONE, 'k', 1, 0)
#define SO2_IOCTL_SET_BUFFER _IOC(_IOC_WRITE, 'k', 2, BUFFER_SIZE)
#define SO2_IOCTL_GET_BUFFER _IOC(_IOC_READ, 'k', 3, BUFFER_SIZE)
#define SO2_IOCTL_DOWN _IOC(_IOC_NONE, 'k', 4, 0)
#define SO2_IOCTL_UP _IOC(_IOC_NONE, 'k', 5, 0)

#endif
