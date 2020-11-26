#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
MODULE_LICENSE("GPL");

static int sched_boot(void)
{
	DEFINE_SPINLOCK(lock);

	spin_lock(&lock);

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(5 * HZ);

	spin_unlock(&lock);

	return 0;

}

static void sched_shutdown(void)
{
}

module_init(sched_boot);
module_exit(sched_shutdown);
