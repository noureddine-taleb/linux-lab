#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include "../include/so2_cdev.h"
#include <linux/wait.h>

MODULE_AUTHOR("black panter");
MODULE_DESCRIPTION("char dev lab");
MODULE_LICENSE("GPL");

#define SO2_MAJOR 42
#define SO2_MINOR 0
#define SO2_MAX_MINORS 1
#define MODULE_NAME "so2_cdev"
#define LOCKED 0
#define UNLOCKED 1
#define BUFSIZ 4096
#define MESSAGE "hello\n"
#define IOCTL_MESSAGE "Hello ioctl"

// device specifics
static struct so2_device_data {
	struct cdev cdev;
	atomic_t access;
	wait_queue_head_t q;
	wait_queue_head_t io_wq;
	int io_flag;
	int wq_flag;
	char buf[BUFSIZ];
};

struct so2_device_data so2;

// file operaions
static int so2_open(struct inode *inode, struct file *filp)
{
	//if (atomic_cmpxchg(&so2.access, UNLOCKED, LOCKED) == LOCKED)
	//	return -EBUSY;
	filp->private_data = container_of(inode->i_cdev, struct so2_device_data, cdev);
	pr_info("%s: open\n", MODULE_NAME);
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(3 * HZ);
	return 0;
}

static int so2_release(struct inode *inode, struct file *filp)
{
	// atomic_set(&so2.access, UNLOCKED);
	pr_info("%s: close\n", MODULE_NAME);
	return 0;
}

static ssize_t so2_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	size_t safe_size;

	if (filp->f_flags & O_NONBLOCK)
	       return -EAGAIN;
	
	so2.io_flag = 0;
	if (wait_event_interruptible(so2.io_wq, so2.io_flag != 0))
		return -ERESTARTSYS;

	if (*off >= BUFSIZ)
		return 0;

	safe_size = min(BUFSIZ - *off, len);
	pr_warn("%s: user requested size: %lu, max allowed size: %lu, off: %lli", MODULE_NAME, len, safe_size, *off);

	if (copy_to_user(buf, so2.buf + *off, safe_size))
		return -EFAULT;
	
	*off += safe_size;
	return safe_size;
}

static ssize_t so2_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	size_t safe_size;
	safe_size = min(BUFSIZ, len);

	if (copy_from_user(so2.buf, buf, safe_size))
		return -EFAULT;
	
	*off = safe_size;
	
	so2.io_flag = 1;
	wake_up_interruptible(&so2.io_wq);
	return safe_size;
}

static long so2_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
	
	case SO2_IOCTL_PRINT:
		memcpy(so2.buf, IOCTL_MESSAGE, sizeof(IOCTL_MESSAGE));
		break;
	
	case SO2_IOCTL_GET_BUFFER:
		if (copy_to_user((char __user *)arg, so2.buf, BUFFER_SIZE))
			return -EFAULT;
		break;

	case SO2_IOCTL_SET_BUFFER:
		if (copy_from_user(so2.buf, (char __user *)arg, BUFFER_SIZE))
			return -EFAULT;
		break;

	case SO2_IOCTL_DOWN:
		so2.wq_flag = 0;
		wait_event_interruptible(so2.q, so2.wq_flag != 0);
		break;

	case SO2_IOCTL_UP:
		so2.wq_flag = 1;
		wake_up_interruptible(&so2.q);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static const struct file_operations so2_ops = {
	.owner = THIS_MODULE,
	.open = so2_open,
	.release = so2_release,
	.read = so2_read,
	.write = so2_write,
	.unlocked_ioctl = so2_ioctl,
};

// driver main body
static int so2_cdev_init(void)
{
	pr_info("%s: register success\n", MODULE_NAME);
	register_chrdev_region(MKDEV(SO2_MAJOR, SO2_MINOR), SO2_MAX_MINORS, MODULE_NAME);
	cdev_init(&so2.cdev, &so2_ops);
	cdev_add(&so2.cdev, MKDEV(SO2_MAJOR, SO2_MINOR), SO2_MAX_MINORS);
	atomic_set(&so2.access, UNLOCKED);
	memcpy(so2.buf, MESSAGE, sizeof(MESSAGE));
	init_waitqueue_head(&so2.q);
	init_waitqueue_head(&so2.io_wq);
	so2.wq_flag = 0;
	so2.io_flag = 0;
	return 0;
}

static void so2_cdev_exit(void)
{
	pr_info("%s: unregister success\n", MODULE_NAME);
	unregister_chrdev_region(MKDEV(SO2_MAJOR, SO2_MINOR), SO2_MAX_MINORS);
	cdev_del(&so2.cdev);
}

module_init(so2_cdev_init);
module_exit(so2_cdev_exit);
