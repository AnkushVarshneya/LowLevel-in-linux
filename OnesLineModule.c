// Code derived from:
// https://appusajeev.wordpress.com/2011/06/18/writing-a-linux-character-device-driver/
// http://pete.akeo.ie/2011/08/writing-linux-device-driver-for-kernels.html

#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>

#define dbg(format, arg...) do { if (debug) pr_info(CLASS_NAME ": %s: " format, __FUNCTION__, ## arg); } while (0)
#define err(format, arg...) pr_err(CLASS_NAME ": " format, ## arg)
#define info(format, arg...) pr_info(CLASS_NAME ": " format, ## arg)
#define warn(format, arg...) pr_warn(CLASS_NAME ": " format, ## arg)


#define DEVICE_NAME "oneslines"
#define CLASS_NAME "comp3000"

static struct class* ones_lines_class = NULL;
static struct device* ones_lines_device = NULL;
static int ones_lines_major;

static int ones_lines_open(struct inode *the_inode, struct file *f)
{
	return 0;
}

static ssize_t ones_lines_read(struct file *f, char *buf, size_t len, loff_t *offset)
{
	size_t i;

	for (i = 0; i < len; i++) {
		//every 78 characters also print a new line char needed for question #7
		if(i%78==0)
			put_user('\n', buf++);
		put_user('1', buf++);
	}

	return i;
}

static int ones_lines_release(struct inode *the_inode, struct file *f)
{
	printk(KERN_ALERT "oneslines device closed\n");
	return 0;
}


static struct file_operations ones_lines_fops = {
	.open = ones_lines_open,
	.read = ones_lines_read,
	.release = ones_lines_release,
};


static char *ones_lines_devnode(struct device *dev, umode_t *mode)
{
	if (mode)
	*mode = 0444;
	return NULL;
}

static int __init ones_lines_init(void)
{
	int retval;

	ones_lines_major = register_chrdev(0, DEVICE_NAME, &ones_lines_fops);
	if (ones_lines_major < 0) {
		err("failed to register device: error %d\n", ones_lines_major);
		retval = ones_lines_major;
		goto failed_chrdevreg;
	}

	ones_lines_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(ones_lines_class)) {
		err("failed to register device class '%s'\n", CLASS_NAME);
		retval = PTR_ERR(ones_lines_class);
		goto failed_classreg;
	}

	ones_lines_class->devnode = ones_lines_devnode;

	ones_lines_device = device_create(ones_lines_class, NULL, MKDEV(ones_lines_major, 0),
	NULL, DEVICE_NAME);

	if (IS_ERR(ones_lines_device)) {
		err("failed to create device '%s'\n", DEVICE_NAME);
		retval = PTR_ERR(ones_lines_device);
		goto failed_devreg;
	}
	
	info("oneslines device registered using major %d.\n", ones_lines_major);
	
	return 0;
	
	failed_devreg:
	class_unregister(ones_lines_class);
	class_destroy(ones_lines_class);
	failed_classreg:
	unregister_chrdev(ones_lines_major, DEVICE_NAME);
	failed_chrdevreg:
	return -1;
}

static void __exit ones_lines_exit(void)
{
	device_destroy(ones_lines_class, MKDEV(ones_lines_major, 0));
	class_unregister(ones_lines_class);
	class_destroy(ones_lines_class);
	unregister_chrdev(ones_lines_major, "oneslines");
	info("Unloading oneslines module.\n");
	return;
}

module_init(ones_lines_init);
module_exit(ones_lines_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ankush Varshneya");
MODULE_DESCRIPTION("A write oneslines character device module");
