#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>

MODULE_AUTHOR("BLACK PANTER");
MODULE_DESCRIPTION("simple timer module");
MODULE_LICENSE("GPL");

#define MODULE_NAME "timer"
#define TIMER_TIMEOUT 1

static struct timer_list timer;

static void timer_handler(struct timer_list *t)
{
	pr_info("%s: timer handler called", MODULE_NAME);
	mod_timer(&timer, jiffies + TIMER_TIMEOUT * HZ);
}

static int timer_init(void)
{
	timer_setup(&timer, timer_handler, 0);
	
	pr_info("%s: timer setup success", MODULE_NAME);
	mod_timer(&timer, jiffies + TIMER_TIMEOUT * HZ);

	return 0;
}

static void timer_exit(void)
{
	del_timer_sync(&timer);
	pr_info("%s: timer deleted success", MODULE_NAME);
}

module_init(timer_init);
module_exit(timer_exit);
