#ifndef __TIMER_IOCTL_H__
#define __TIMER_IOCTL_H__

#include <linux/ioctl.h>
#define IOCTL_TIMER_SET _IOW ('k', 1, unsigned long)
#define IOCTL_TIMER_CANCEL _IO ('k', 2)
#define IOCTL_TIMER_ALLOC _IOW ('k', 3, unsigned long)
#define IOCTL_TIMER_MON	 _IO ('k', 4)

#endif

