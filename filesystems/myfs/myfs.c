#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kernel.h>

MODULE_AUTHOR("BLACK PANTER");
MODULE_DESCRIPTION("HOME MADE FS");
MODULE_LICENSE("GPL");

#define MODULE_NAME "myfs"
#define MYFS_BLOCKSIZE 4096
#define MYFS_BLOCKSIZE_BITS 12
#define MYFS_MAGIC 0xbeefcafe


static struct super_operations myfs_ops = {
	.statfs = simple_statfs,
	.drop_inode = generic_delete_inode,
};

static struct inode *myfs_get_inode(struct super_block *sb, const struct inode *dir, int mode)
{
	struct inode *inode;

	inode = new_inode(sb);
	if (!inode)
		return NULL;

	inode->i_ino = get_next_ino();
	inode_init_owner(inode, dir, mode);
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);

	if (S_ISDIR(mode)) {
		inode->i_op =  &simple_dir_inode_operations;
		inode->i_fop = &simple_dir_operations;
		inc_nlink(inode);
	}

	return inode;
}

static int myfs_fill_sb(struct super_block *sb, void *data, int silent)
{
	struct inode *root_inode;
	struct dentry *root_dentry;

	sb->s_maxbytes = MAX_LFS_FILESIZE;
	sb->s_blocksize = MYFS_BLOCKSIZE;
	sb->s_blocksize_bits = MYFS_BLOCKSIZE_BITS;
	sb->s_magic = MYFS_MAGIC;
	sb->s_op = &myfs_ops;
	
	root_inode = myfs_get_inode(sb, NULL, S_IFDIR | 
			S_IRWXU | 
			S_IRGRP | S_IXGRP |
		       	S_IROTH	| S_IXOTH
			);

	if (!root_inode)
		return -ENOMEM;

	pr_info("%s: root inode has %d link(s)", MODULE_NAME, root_inode->i_nlink);
	
	root_dentry = d_make_root(root_inode);

	if (!root_dentry)
		goto err_no_dentry;

	sb->s_root = root_dentry;

	return 0;

err_no_dentry:
	iput(root_inode);
	return -ENOMEM;
}

static struct dentry *myfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
	return mount_nodev(fs_type, flags, data, myfs_fill_sb);
}

static struct file_system_type myfs = {
	.owner = THIS_MODULE,
	.name = "myfs",
	.mount = myfs_mount,
	.kill_sb = kill_litter_super,
};

static int __init init_myfs(void)
{
	int err;
	err = register_filesystem(&myfs);
	
	if (err)
		return err;
	
	pr_info("%s: fs is loaded", MODULE_NAME);
	return 0;
}

static void __exit exit_myfs(void)
{
	unregister_filesystem(&myfs);
	pr_info("%s: fs is unloaded", MODULE_NAME);
}

module_init(init_myfs);
module_exit(exit_myfs);
