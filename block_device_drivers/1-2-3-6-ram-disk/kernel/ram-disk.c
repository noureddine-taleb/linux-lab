#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/string.h>
#include <linux/kernel.h>

MODULE_AUTHOR("BLACK PANTER");
MODULE_DESCRIPTION("block devices test");
MODULE_LICENSE("GPL");

#define MODULE_NAME "ramdisk"
#define DISK_NAME "myblock"
#define BLK_MAJOR 420
#define BLK_NR_MINORS 1
#define BLK_MINOR 0

#define KERNEL_SECTOR_SIZE 512
#define NR_SECTORS 128

struct blk_struct {
	struct gendisk *gd;
	spinlock_t lock;
	struct request_queue *queue;
} dev;

// fops
static int blk_open(struct block_device *dev, fmode_t mode)
{
	pr_info("%s: blk open called", MODULE_NAME);
	return 0;
}

static void blk_release(struct gendisk *disk, fmode_t mode)
{
	pr_info("%s: blk release called", MODULE_NAME);
}

static struct block_device_operations blk_fops = {
	.owner = THIS_MODULE,
	.open = blk_open,
	.release = blk_release,
};

// set up
static void blk_request(struct request_queue *q)
{
	struct request *rq;
	
	while (1) {

		//rq = blk_fetch_request(q);

		if (!rq)
			return;
	
		if (blk_rq_is_passthrough(rq)) {
			pr_info("%s: skip non fs requests", MODULE_NAME);
			//__blk_end_request_all(rq, -EIO);
			continue;
		}

		/*pr_info("%s: wanted request recieved",
			"pos=%lu bytes=%lu ",
			"cur_byte=%lu dir=%c\n",
			blk_rq_pos(rq), blk_rq_bytes(rq), blk_rq_cur_bytes(rq), rq_data_dir(rq) ? 'w': 'r',  MODULE_NAME);*/
		//__blk_end_request_all(rq, 0);
	}
}

static int create_block_device(struct blk_struct *dev) {
	int err;
	dev->gd = alloc_disk(BLK_NR_MINORS);
	
	if (!dev->gd) {
		err = -ENOMEM;
		goto err_blk_exit;
	}

	// define a request queue
	spin_lock_init(&dev->lock);
	dev->queue = blk_mq_init_sq_queue(blk_request, &dev->lock);
	if (!dev->queue) {
		err = -ENOMEM;
		goto err_blk_queue;
	}
	blk_queue_logical_block_size(dev->queue, KERNEL_SECTOR_SIZE);
	dev->queue->queuedata = dev;

	// fill required fields of gendisk
	dev->gd->major = BLK_MAJOR;
	dev->gd->first_minor = BLK_MINOR;
	dev->gd->fops = &blk_fops;
	dev->gd->queue = dev->queue;
	dev->gd->private_data = dev;
	snprintf(dev->gd->disk_name, 32, DISK_NAME);
	set_capacity(dev->gd, NR_SECTORS);

	add_disk(dev->gd);
	return 0;

err_blk_queue:
	del_gendisk(dev->gd);

err_blk_exit:
	return err;
}

static int __init blk_init(void)
{
	int err;

	if ((err = register_blkdev(BLK_MAJOR, MODULE_NAME)))
		goto err_exit;

	if ((err = create_block_device(&dev)))
		goto err_gendisk;

	pr_info("%s: init called", MODULE_NAME);
	return 0;

err_gendisk:
	unregister_blkdev(BLK_MAJOR, MODULE_NAME);

err_exit:
	return err;

}

static void delete_block_device(struct blk_struct *dev) {
	blk_cleanup_queue(dev->queue);
	del_gendisk(dev->gd);
	put_disk(dev->gd);
}

static void __exit blk_exit(void)
{
	delete_block_device(&dev);
	unregister_blkdev(BLK_MAJOR, MODULE_NAME);
	pr_info("%s: exit called", MODULE_NAME);
}

module_init(blk_init);
module_exit(blk_exit);
