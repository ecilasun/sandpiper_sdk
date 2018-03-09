#include <linux/module.h>
#include <linux/init.h>

static int __init sandpiper_init()
{
	pr_info("Sandpiper device active\n");
	return 0;
}

static void __exit sandpiper_exit()
{
	pr_info("Sandpiper device inactive\n");
}

module_init(sandpiper_init);
module_exit(sandpiper_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("Engin Cilasun <ecilasun@me.com>");

MODULE_DESCRIPTION("Sandpiper device interface");

