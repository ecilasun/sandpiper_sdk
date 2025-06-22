#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

// Reserved memory region start
#define PHYS_ADDR 0x18000000
// 32Mbytes of reserved memory
#define MEM_SIZE 0x2000000

// Character device name
#define DEVICE_NAME "sandpiper"

// IOCTL command definition
#define MY_IOCTL_GET_VIRT_ADDR _IOR('k', 0, void*)

struct my_driver_data {
	void __iomem *virt_addr;
    struct cdev cdev;
    struct device *device;
};

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static long    dev_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .unlocked_ioctl = dev_ioctl,
    .release = dev_release,
};

static int sandpiper_probe(struct platform_device *pdev)
{
    struct my_driver_data *drvdata;
    int ret = 0;
    dev_t dev_num = 0;

    drvdata = devm_kzalloc(&pdev->dev, sizeof(struct my_driver_data), GFP_KERNEL);
    if (!drvdata)
        return -ENOMEM;

    drvdata->virt_addr = ioremap(PHYS_ADDR, MEM_SIZE);
    if (!drvdata->virt_addr) {
        dev_err(&pdev->dev, "ioremap failed\n");
        return -ENOMEM;
    }

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        dev_err(&pdev->dev, "Failed to allocate character device region\n");
        iounmap(drvdata->virt_addr);
        return ret;
    }

    cdev_init(&drvdata->cdev, &fops);
    drvdata->cdev.owner = THIS_MODULE;

    ret = cdev_add(&drvdata->cdev, dev_num, 1);
    if (ret < 0) {
        dev_err(&pdev->dev, "Failed to add character device\n");
        unregister_chrdev_region(dev_num, 1);
        iounmap(drvdata->virt_addr);
        return ret;
    }

    drvdata->device = device_create(class_create(DEVICE_NAME), NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(drvdata->device)) {
        dev_err(&pdev->dev, "Failed to create device\n");

		cdev_del(&drvdata->cdev);
        unregister_chrdev_region(dev_num, 1);
        iounmap(drvdata->virt_addr);

		return PTR_ERR(drvdata->device);
    }

    platform_set_drvdata(pdev, drvdata);

    dev_info(&pdev->dev, "Physical address 0x%x mapped to virtual address 0x%p\n", PHYS_ADDR, drvdata->virt_addr);
    dev_info(&pdev->dev, "Character device /dev/%s created\n", DEVICE_NAME);

    return 0;
}

static void sandpiper_remove(struct platform_device *pdev)
{
    struct my_driver_data *drvdata = platform_get_drvdata(pdev);

    device_destroy(class_create(DEVICE_NAME), MKDEV(MAJOR(drvdata->cdev.dev), MINOR(drvdata->cdev.dev)));
    class_destroy(class_create(DEVICE_NAME));

    cdev_del(&drvdata->cdev);
    unregister_chrdev_region(drvdata->cdev.dev, 1);
    iounmap(drvdata->virt_addr);

    dev_info(&pdev->dev, "Virtual address unmapped and character device removed\n");
}

static int dev_open(struct inode *inode, struct file *file)
{
    struct my_driver_data *drvdata = container_of(inode->i_cdev, struct my_driver_data, cdev);
    file->private_data = drvdata;
    printk(KERN_INFO "%s: Device opened\n", DEVICE_NAME);
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "%s: Device released\n", DEVICE_NAME);
    return 0;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct my_driver_data *drvdata = (struct my_driver_data*)file->private_data;
    void __iomem *virt_addr = drvdata->virt_addr;

    switch (cmd) {
        case MY_IOCTL_GET_VIRT_ADDR:
            if (copy_to_user((void __user *)arg, &virt_addr, sizeof(virt_addr)))
                return -EFAULT;
            break;

        default:
            return -ENOTTY;
    }

    return 0;
}

static struct platform_driver sandpiper_driver = {
	.probe		= sandpiper_probe,
	.remove		= sandpiper_remove,
	.driver = {
		.name = "sandpiperdevice",
		.owner = THIS_MODULE,
	},
};

static int __init sandpiper_init(void)
{
	printk("<1>sandpiper alive\n");
	return platform_driver_register(&sandpiper_driver);
}


static void __exit sandpiper_exit(void)
{
	platform_driver_unregister(&sandpiper_driver);
	printk(KERN_ALERT "sandpiper retired\n");
}

module_init(sandpiper_init);
module_exit(sandpiper_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Engin Cilasun");
MODULE_DESCRIPTION("sandpiper - system driver for sandpiper device");
