#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>


#define SET_SIZE_OF_QUEUE_IOW('a','a',int *)
#define PUSH_DATA_IOW('a','b',struct data *)
#define POP_DATA_IOR('a','c',struct data *)

struct data
{
	int size;
	char *val;
};

static struct file_operations fops = {
	.open = ch_open,
	.release = ch_release,
	.unlocked_ioctl = ch_ioctl,
};


static int ch_open(struct inode *inode,struct file *file)
{
	printk("Device opened...\n");
}

static int ch_release(struct inode *inode,struct file *file)
{
        printk("Device closed...\n");
}

static long ch_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case SET_SIZE_OF_QUEUE:
			// Implementation goes here
			break;
		case PUSH_DATA:
			// Implementation goes here
                        break;
		case POP_DATA:
			// Implementation goes here
                        break;
		default:
			pr_info("Default\n");
			break;
	}
	return 0;
}
