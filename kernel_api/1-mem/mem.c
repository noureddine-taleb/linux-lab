#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ctype.h>

MODULE_LICENSE("GPL");

const char *buf;

static int module_boot(void)
{
	buf = kmalloc(4096 * sizeof(*buf), GFP_KERNEL);

	if(buf == NULL)
		goto err;

	pr_info("chars: ");

	int i;
	for(i=0; i < 4096; i++)
		// if(isalpha(buf[i]))
			pr_info("%c", buf[i]);

	pr_info("\n");

	return 0;
err:
	return -ENOMEM;

}

static void module_shutdown(void)
{
	kfree(buf);
}

module_init(module_boot);
module_exit(module_shutdown);
