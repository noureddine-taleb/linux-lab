#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

MODULE_AUTHOR("black panter");
MODULE_DESCRIPTION("unit test for module");
MODULE_LICENSE("GPL");

extern void task_info_remove_expired(void);
extern void task_info_pr_list(const char *msg);
extern void task_info_add_for_current(void);

static int list_test_init(void)
{
	task_info_add_for_current();
	task_info_pr_list("test: after new addition");
	
	return 0;
}

static void list_test_exit(void)
{
	task_info_remove_expired();
	task_info_pr_list("test: after removing expired");
}

module_init(list_test_init);
module_exit(list_test_exit);
