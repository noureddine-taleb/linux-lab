#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/atomic.h>

MODULE_AUTHOR("BLACK PANTER");
MODULE_DESCRIPTION("kthread lab");
MODULE_LICENSE("GPL");

#define MODULE_NAME "KTHREAD"

static struct kthread_data {
	wait_queue_head_t wq_module_exit;
	wait_queue_head_t wq_kthread_exit;
	atomic_t kthread_flag;
	atomic_t module_flag;
} kd;

static int kthread_handler(void *data)
{
	struct kthread_data *__data;
	__data = (struct kthread_data *)data;

	pr_info("%s: yay kthread", MODULE_NAME);
	wait_event(__data->wq_module_exit, atomic_read(&__data->module_flag));
	
	atomic_set(&__data->kthread_flag, 1);
	wake_up(&__data->wq_kthread_exit);
	do_exit(0);
}

static int __init kthread_init(void)
{
	struct task_struct *kt;

	init_waitqueue_head(&kd.wq_module_exit);
	init_waitqueue_head(&kd.wq_kthread_exit);
	atomic_set(&kd.kthread_flag, 0);
	atomic_set(&kd.module_flag, 0);

	kt = kthread_create(kthread_handler, &kd, "Black_Panter%d", 0);
	wake_up_process(kt);

	return 0;
}


static void __exit kthread_exit(void)
{
	atomic_set(&kd.module_flag, 1);
	wake_up(&kd.wq_module_exit);

	wait_event(kd.wq_kthread_exit, atomic_read(&kd.kthread_flag));
}

module_init(kthread_init);
module_exit(kthread_exit);
