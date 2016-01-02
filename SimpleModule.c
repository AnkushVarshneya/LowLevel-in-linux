#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init simple_init(void)
{
	printk("The value of zero as a byte:%d\n", *((int *) 0));
	printk ("Hello kernel world!\n");
	return 0;
}

static void __exit simple_exit(void)
{
	printk ("Goodbye kernel world.\n");
	return;
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ankush Varshneya");
MODULE_DESCRIPTION("A simple module");
    