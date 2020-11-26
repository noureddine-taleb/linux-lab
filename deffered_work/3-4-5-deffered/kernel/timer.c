#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "../include/ioctl.h"

MODULE_AUTHOR("BLACK PANTER");
MODULE_DESCRIPTION("simple timer module");
MODULE_LICENSE("GPL");

#define MODULE_NAME "timer"
#define TIMER_TIMEOUT 1
#define TIMER_MAJOR 42
#define TIMER_MINOR 0

#define TIMER_TYPE_NONE 0
#define TIMER_TYPE_ALLOC 1
#define TIMER_TYPE_SET 2
#define TIMER_TYPE_MON 3

static struct timer_data {
	struct timer_list timer;
	struct work_struct work;
	struct cdev cdev;
	unsigned int flag;
	struct list_head list;
	spinlock_t lock;
	wait_queue_head_t wq;
} td;

static struct mon_proc {
	struct task_struct *task;
	struct list_head list;
};

static struct mon_proc *get_proc(pid_t pid)
{
	struct task_struct *t;
	struct mon_proc *mp;

	t = pid_task(find_vpid(pid), PIDTYPE_PID);
	
	if (!t)
		return ERR_PTR(-ESRCH);

	mp = kmalloc(sizeof(*mp), GFP_ATOMIC);

	if (!mp)
		return ERR_PTR(-ENOMEM);
	
	get_task_struct(t);
	mp->task = t;

	return mp;
}

// timer
static void alloc_io(void)
{
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(5 * HZ);
	pr_info("%s: allah chb3a5 its 4 interrupt context", MODULE_NAME);
}

static void work_handler(struct work_struct *w)
{
	alloc_io();
}

static void timer_handler(struct timer_list *t)
{
	struct timer_data *data;
	struct list_head *i, *tmp;
	struct mon_proc *tmp_mp;

	data  = container_of(t, struct timer_data, timer);

	switch (data->flag) {
	
	case TIMER_TYPE_SET:
		pr_info("%s: current=%s, pid=%d", MODULE_NAME, current->comm, current->pid);
		data->flag = TIMER_TYPE_NONE;
		wake_up_interruptible(&data->wq);
		break;

	case TIMER_TYPE_ALLOC:
		schedule_work(&data->work);
		break;

	case TIMER_TYPE_MON:
		spin_lock(&data->lock);
		list_for_each_safe(i, tmp, &data->list) {
			tmp_mp = list_entry(i, struct mon_proc, list);
			if (tmp_mp->task->state == TASK_DEAD) {
				pr_info("%s: delete dead process: pid=%d comm=%s", MODULE_NAME, tmp_mp->task->pid, tmp_mp->task->comm);
				list_del(i);
				put_task_struct(tmp_mp->task);
				kfree(tmp_mp);
			}
		}
		spin_unlock(&data->lock);

		mod_timer(&data->timer, jiffies + HZ);
		break;
	}
}

// file operation
static int timer_open(struct inode *inode, struct file *filp)
{
	pr_info("%s: open called", MODULE_NAME);
	filp->private_data = container_of(inode->i_cdev, struct timer_data, cdev);
	return 0;
}

static long timer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	long ret;
	struct timer_data *data;
	struct mon_proc *mp;

	data = filp->private_data;
	ret = 0;

	pr_info("%s: ioctl called", MODULE_NAME);
	switch (cmd) {
	
	case IOCTL_TIMER_SET:
		data->flag = TIMER_TYPE_SET;
		pr_info("%s: ioctl requested delay: %lu", MODULE_NAME, arg);
		mod_timer(&data->timer, jiffies + HZ * arg);
		wait_event_interruptible(data->wq, data->flag == TIMER_TYPE_NONE);
		break;

	case IOCTL_TIMER_CANCEL:
		del_timer(&data->timer);
		break;

	case IOCTL_TIMER_ALLOC:
		data->flag = TIMER_TYPE_ALLOC;
		mod_timer(&data->timer, jiffies + HZ * arg);
		break;

	case IOCTL_TIMER_MON:
		mp = get_proc(arg);
		
		if (IS_ERR(mp)) {
			ret = PTR_ERR(mp);
			break;
		}
		
		spin_lock_bh(&data->lock);
		list_add(&mp->list, &data->list);
		spin_unlock_bh(&data->lock);

		data->flag = TIMER_TYPE_MON;
		mod_timer(&data->timer, jiffies + HZ);
		ret = 0;
		
		break;

	default:
		ret = -ENOTTY;
		break;
	}

	return ret;
}

static int timer_release(struct inode *inode, struct file *filp)
{
	pr_info("%s: release called", MODULE_NAME);
	return 0;
}

static struct file_operations timer_fops = {
	.owner = THIS_MODULE,
	.open = timer_open,
	.unlocked_ioctl = timer_ioctl,
	.release = timer_release,
};

// main
static int __init timer_init(void)
{
	register_chrdev_region(MKDEV(TIMER_MAJOR, TIMER_MINOR), 1, MODULE_NAME);
	cdev_init(&td.cdev, &timer_fops);
	cdev_add(&td.cdev, MKDEV(TIMER_MAJOR, TIMER_MINOR), 1);
	
	INIT_LIST_HEAD(&td.list);
	spin_lock_init(&td.lock);

	timer_setup(&td.timer, timer_handler, 0);
	INIT_WORK(&td.work, work_handler);
	td.flag = TIMER_TYPE_NONE;
	init_waitqueue_head(&td.wq);

	pr_info("%s: timer setup success", MODULE_NAME);
	return 0;
}

static void __exit timer_exit(void)
{
	cdev_del(&td.cdev);
	unregister_chrdev_region(MKDEV(TIMER_MAJOR, TIMER_MINOR), 1);
	pr_info("%s: timer deleted success", MODULE_NAME);
	cancel_work_sync(&td.work);
	del_timer_sync(&td.timer);
}

module_init(timer_init);
module_exit(timer_exit);
