#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
MODULE_LICENSE("GPL");

struct task_info {
	pid_t pid;
	char* comm;
	unsigned long timestamp;
};

static struct task_info *taskc;
static struct task_info *taskcp;
static struct task_info *taskn;
static struct task_info *tasknn;

// TODO 1
static struct task_info *alloc_task_info(struct task_struct *t)
{
	struct task_info *ti = kmalloc(sizeof(*ti), GFP_KERNEL);
	ti->timestamp = jiffies;
	ti->pid = t->pid;
	ti->comm = t->comm;
	return ti;
}

static int memory_init(void)
{
	// TODO 2
	taskc = alloc_task_info(current); 
	taskcp = alloc_task_info(current->parent); 
	taskn = alloc_task_info(next_task(current)); 
	tasknn = alloc_task_info(next_task(next_task(current))); 
	
	// TODO 3
	pr_info("pid: %d timestamp: %lu name: %s\n", taskcp->pid, taskcp->timestamp, taskcp->comm);
	pr_info("pid: %d timestamp: %lu name: %s\n", taskc->pid, taskc->timestamp, taskc->comm);
	pr_info("pid: %d timestamp: %lu name: %s\n", taskn->pid, taskn->timestamp, taskn->comm);
	pr_info("pid: %d timestamp: %lu name: %s\n", tasknn->pid, tasknn->timestamp, tasknn->comm);
	
	return 0;
err:
	return -ENOMEM;
}

static void memory_exit(void)
{
	kfree(taskcp);
	kfree(taskc);
	kfree(taskn);
	kfree(tasknn);
}

module_init(memory_init);
module_exit(memory_exit);
