#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/err.h>


#define SET_SIZE_OF_QUEUE _IOW('a','a',int *)
#define PUSH_DATA _IOW('a','b',struct data *)
#define POP_DATA _IOR('a','c',struct data *)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CHILKA");
MODULE_DESCRIPTION("IOCTL calls in Linux device driver");
MODULE_VERSION("1.0");

int32_t value = 0;
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static char *queue_buffer = NULL;
static int queue_size = 0;
static int front,rear,count = 0;

static DECLARE_WAIT_QUEUE_HEAD(queue_read_queue);

struct data
{
	int size;
	char *val;
};

static int vi_open(struct inode *inode,struct file *file)
{
	printk("Device opened...\n");
	return 0;
}

static int vi_release(struct inode *inode,struct file *file)
{
        printk("Device closed...\n");
	return 0;
}

static ssize_t vi_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

static ssize_t vi_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write function\n");
        return len;
}

static long vi_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
	switch(cmd)
	{
		case SET_SIZE_OF_QUEUE:
			// Implementation goes here
			int size;
			if(copy_from_user(&size,(int __user*)arg,sizeof(size))) return -1;
			if(size < 0) return -EINVAL;
			if(queue_buffer)
			{
				kfree(queue_buffer);
				queue_buffer = NULL;
				queue_size = 0;
				front = rear = count = 0;
			}
			queue_buffer = kmalloc(size,GFP_KERNEL);
			if(!queue_buffer) return -ENOMEM;

			queue_size = size;
			front = rear = count = 0;
			printk("Queue initialized with size %d\n",size);
			break;
		case PUSH_DATA:
			// Implementation goes here
			struct data input;
			char *buff;
			// copy structure from user
			if(copy_from_user(&input,(struct data __user*)arg,sizeof(struct data))) return -1;
			if(input.size <= 0 || input.size > queue_size) return -EINVAL;

			buff = kmalloc(input.size,GFP_KERNEL);
			if(!buff) return -ENOMEM;
			// copy the data from user
			if(copy_from_user(buff,input.val,input.size))
			{
				kfree(buff);
				return -EFAULT;
			}
			for(int i=0;i<input.size;i++)
			{
				queue_buffer[rear] = buff[i];
				rear = (rear+1) % queue_size;
				count++;
			}
			//kfree(buff);
			printk("Pushed the %d size data to the queue: %s\n",input.size,buff);
			kfree(buff);
			wake_up_interruptible(&queue_read_queue);
                        break;
		case POP_DATA:
			// Implementation goes here
			struct data input_data;
                        char *temp_buff;
                        // copy structure from user
                        if(copy_from_user(&input_data,(struct data __user*)arg,sizeof(struct data))) return -1;
                        if(input_data.size <= 0 || input_data.size > queue_size) return -EINVAL;

                        temp_buff = kmalloc(input_data.size,GFP_KERNEL);
                        if(!temp_buff) return -ENOMEM;

			if(wait_event_interruptible(queue_read_queue,count >= input_data.size))
			{
				kfree(temp_buff);
				return -EFAULT;
			}

			for(int i=0;i<queue_size;i++)
			{
				temp_buff[i] = queue_buffer[front];
				front = (front+1) % queue_size;
				count--;
			}
			if(copy_to_user(input_data.val,temp_buff,input_data.size))
			{
				kfree(temp_buff);
				return -EFAULT;
			}
			printk("Poped %d bytes from the queue: %s\n",input_data.size,temp_buff);
			kfree(temp_buff);
                        break;
		default:
			pr_info("Default\n");
			break;
	}
	return 0;
}

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = vi_read,
        .write          = vi_write,
        .open           = vi_open,
        .unlocked_ioctl = vi_ioctl,
        .release        = vi_release,
};

static int __init vicharak_init(void)
{
	if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);

        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }

        /*Creating struct class*/
        if(IS_ERR(dev_class = class_create("chilka_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }

        /*Creating device*/
        if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"chilka"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;

	r_device:
        	class_destroy(dev_class);
	r_class:
        	unregister_chrdev_region(dev,1);
        	return -1;
}

static void __exit vicharak_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);

	if(queue_buffer) kfree(queue_buffer);
        pr_info("Device Driver Remove...Done!!!\n");
}

module_init(vicharak_init)
module_exit(vicharak_exit)
