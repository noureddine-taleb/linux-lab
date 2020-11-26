#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>

MODULE_AUTHOR("black panter");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("kernel lists lab");

LIST_HEAD(list);

struct task_info {
	pid_t pid;
	char* comm;
	unsigned long timestamp;
	struct list_head list;
};

static struct task_info *alloc_task_info(const struct task_struct *t)
{
	struct task_info *ti = kmalloc(sizeof(*ti), GFP_KERNEL);
	ti->timestamp = jiffies;
	ti->pid = t->pid;
	ti->comm = t->comm;
	return ti;
}

static void task_info_add_to_list(const struct task_struct *task)
{
	struct task_info *ti;
	ti = alloc_task_info(task);
	// add the ti to the list
	list_add(&ti->list, &list);
}

static void task_info_add_for_current(void)
{
	task_info_add_to_list(current);
	task_info_add_to_list(current->parent);
	task_info_add_to_list(next_task(current));
	task_info_add_to_list(next_task(next_task(current)));
}

static void task_info_pr(const struct task_info *ti)
{
	pr_info("pid: %d timestamp: %lu name: %s\n", ti->pid, ti->timestamp, ti->comm);
}

static void task_info_pr_list(void)
{
	struct list_head *i, *tmp;
	struct task_info *ti;

	list_for_each_safe(i, tmp, &list) {
		ti = list_entry(i, struct task_info, list);
		task_info_pr(ti);
	}
}

static void task_info_purge_list(void)
{
	struct list_head *i, *tmp;
	struct task_info *ti;
	list_for_each_safe(i, tmp, &list) {
		ti = list_entry(i, struct task_info, list);
		list_del(i); // very important
		kfree(ti);
	}
}

static int list_init(void)
{
	task_info_add_for_current();
	return 0;
}

static void list_exit(void)
{
	task_info_pr_list();
	task_info_purge_list();
}

module_init(list_init);
module_exit(list_exit);
