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
DEFINE_RWLOCK(rw_lock);

struct task_info {
	pid_t pid;
	const char* comm;
	unsigned long timestamp;
	struct list_head list;
	atomic_t count;
};

static struct task_info *alloc_task_info(const struct task_struct *t)
{
	struct task_info *ti = kmalloc(sizeof(*ti), GFP_KERNEL);
	ti->timestamp = jiffies;
	ti->pid = t->pid;
	ti->comm = t->comm;
	atomic_set(&ti->count, 0);
	return ti;
}

static struct task_info *task_info_find_pid(const struct task_struct *t)
{
	struct list_head *i, *tmp;
	struct task_info *ti;

	list_for_each_safe(i, tmp, &list) {
		ti = list_entry(i, struct task_info, list);
		if (ti->pid == t->pid)
			return ti;
	}

	return NULL;
}

void task_info_remove_expired(void)
{
	struct list_head *i, *tmp;
	struct task_info *ti;

	write_lock(&rw_lock);
	list_for_each_safe(i, tmp, &list) {
		ti = list_entry(i, struct task_info, list);
		if ((jiffies - ti->timestamp > 3 * HZ) && (atomic_read(&ti->count) < 5)) {
			list_del(i);
			kfree(ti);
		}

	}
	write_unlock(&rw_lock);
}    
EXPORT_SYMBOL(task_info_remove_expired);

static void task_info_add_to_list(const struct task_struct *task)
{
	struct task_info *ti;
	
	write_lock(&rw_lock);
	ti = task_info_find_pid(task);
	if (ti != NULL) {
		atomic_inc(&ti->count);
		ti->timestamp = jiffies;
		write_unlock(&rw_lock);
		return;
	}
	write_unlock(&rw_lock);

	ti = alloc_task_info(task);
	// add the ti to the list
	write_lock(&rw_lock);
	list_add(&ti->list, &list);
	write_unlock(&rw_lock);
}

void task_info_add_for_current(void)
{
	task_info_add_to_list(current);
	task_info_add_to_list(current->parent);
	task_info_add_to_list(next_task(current));
	task_info_add_to_list(next_task(next_task(current)));
}
EXPORT_SYMBOL(task_info_add_for_current);

static void task_info_pr(const struct task_info *ti)
{
	pr_info("pid: %d timestamp: %lu name: %s\n", ti->pid, ti->timestamp, ti->comm);
}

void task_info_pr_list(const char *msg)
{
	struct list_head *i, *tmp;
	struct task_info *ti;
	
	pr_info("%s\n", msg);
	read_lock(&rw_lock);
	list_for_each_safe(i, tmp, &list) {
		ti = list_entry(i, struct task_info, list);
		task_info_pr(ti);
	}
	read_unlock(&rw_lock);
}
EXPORT_SYMBOL(task_info_pr_list);

static void task_info_purge_list(void)
{
	struct list_head *i, *tmp;
	struct task_info *ti;
	write_lock(&rw_lock);
	list_for_each_safe(i, tmp, &list) {
		ti = list_entry(i, struct task_info, list);
		list_del(i); // very important
		kfree(ti);
	}
	write_unlock(&rw_lock);
}

static int list_sync_init(void)
{
	task_info_add_for_current();
	task_info_pr_list("after first add:");

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(5 * HZ);

	return 0;
}

static void list_sync_exit(void)
{
	// TODO 2
	struct task_info *ti;
	ti = list_entry(list.next, struct task_info, list);
	atomic_set(&ti->count, 5);

	// task_info_remove_expired();
	task_info_pr_list("last print:");
	task_info_purge_list();
}

module_init(list_sync_init);
module_exit(list_sync_exit);
