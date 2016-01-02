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


#define DEVICE_NAME "repeatchar"
#define CLASS_NAME "comp3000"

static struct class* repeatchar_class = NULL;
static struct device* repeatchar_device = NULL;
static int repeatchar_major;
char pattern = '1'; // tracks what to print for question#8

static int repeatchar_open(struct inode *the_inode, struct file *f)
{
    return 0;
}

// make read use the global variable pattern which write function can change
static ssize_t repeatchar_read(struct file *f, char *buf, size_t len, loff_t *offset)
{
    size_t i;

    for (i = 0; i < len; i++) {
    //print pattern which is default '1' but can be changed from write
    //needed for question#8
        put_user(pattern, buf++);
    }

    return i;
}


// add a write function for question#8 so use can write a char from buffer the pattern variable
static ssize_t repeatchar_write(struct file *f, const char *buf, size_t len, loff_t *offset){
    // copy first characters if there is any
    if(len>0)
        get_user(pattern, buf);
	return len;
}


static int repeatchar_release(struct inode *the_inode, struct file *f)
{
    printk(KERN_ALERT "repeatchar device closed\n");
    return 0;
}


static struct file_operations repeatchar_fops = {
    .open = repeatchar_open,
    .read = repeatchar_read,
    .write = repeatchar_write, // added a write function for question#8
    .release = repeatchar_release,
};


static char *repeatchar_devnode(struct device *dev, umode_t *mode)
{
    // changes mode from 0444 to 0666 for question #8 to allow for read/write for all users
    if (mode)
        *mode = 0666;
    return NULL;
}

static int __init repeatchar_init(void)
{
    int retval;

    repeatchar_major = register_chrdev(0, DEVICE_NAME, &repeatchar_fops);
    if (repeatchar_major < 0) {
        err("failed to register device: error %d\n", repeatchar_major);
        retval = repeatchar_major;
        goto failed_chrdevreg;
    }

    repeatchar_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(repeatchar_class)) {
        err("failed to register device class '%s'\n", CLASS_NAME);
        retval = PTR_ERR(repeatchar_class);
        goto failed_classreg;
    }

    repeatchar_class->devnode = repeatchar_devnode;

    repeatchar_device = device_create(repeatchar_class, NULL, MKDEV(repeatchar_major, 0),
    NULL, DEVICE_NAME);

    if (IS_ERR(repeatchar_device)) {
        err("failed to create device '%s'\n", DEVICE_NAME);
        retval = PTR_ERR(repeatchar_device);
        goto failed_devreg;
    }
    
    info("repeatchar device registered using major %d.\n", repeatchar_major);
    
    return 0;
    
    failed_devreg:
    class_unregister(repeatchar_class);
    class_destroy(repeatchar_class);
    failed_classreg:
    unregister_chrdev(repeatchar_major, DEVICE_NAME);
    failed_chrdevreg:
    return -1;
}

static void __exit repeatchar_exit(void)
{
    device_destroy(repeatchar_class, MKDEV(repeatchar_major, 0));
    class_unregister(repeatchar_class);
    class_destroy(repeatchar_class);
    unregister_chrdev(repeatchar_major, "repeatchar");
    info("Unloading repeatchar module.\n");
    return;
}

module_init(repeatchar_init);
module_exit(repeatchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ankush Varshneya");
MODULE_DESCRIPTION("A write repeatchar character device module");