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

// Define the physical address you want to map
#define PHYS_ADDR 0x18000000
// Define the size of the memory region you want to map (32MB)
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
static long    dev_ioctl(struct file *, struct file *, unsigned int, unsigned long);

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .unlocked_ioctl = dev_ioctl,
    .release = dev_release,
};

static int my_probe(struct platform_device *pdev)
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

    drvdata->device = device_create(class_create(THIS_MODULE, DEVICE_NAME), NULL, dev_num, NULL, DEVICE_NAME);
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

static int my_remove(struct platform_device *pdev)
{
    struct my_driver_data *drvdata = platform_get_drvdata(pdev);

    device_destroy(class_create(THIS_MODULE, DEVICE_NAME), MKDEV(MAJOR(drvdata->cdev.dev), MINOR(drvdata->cdev.dev)));
    class_destroy(THIS_MODULE);

    cdev_del(&drvdata->cdev);

    unregister_chrdev_region(drvdata->cdev.dev, 1);

    iounmap(drvdata->virt_addr);

    dev_info(&pdev->dev, "Virtual address unmapped and character device removed\n");

    return 0;
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

static struct platform_driver my_platform_driver = {
    .probe  = my_probe,
    .remove = my_remove,
    .driver = {
        .name = "my_device",
        .owner = THIS_MODULE,
    },
};

static int __init my_init(void)
{
    printk(KERN_INFO "Driver init\n");
    return platform_driver_register(&my_platform_driver);
}

static void __exit my_exit(void)
{
    platform_driver_unregister(&my_platform_driver);
    printk(KERN_INFO "Driver exit\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Engin Cilasun");
MODULE_DESCRIPTION("Map the 32Mbyte reserved memory region for sandpiper");


/*
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

#define DEVICE_NAME "/dev/sandpiper" // Corrected device name
#define MY_IOCTL_GET_VIRT_ADDR _IOR('k', 0, void*)

int main() {
    int fd;
    void *virt_addr;
    unsigned int *ptr; // Pointer to access the memory

    fd = open(DEVICE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return 1;
    }

    if (ioctl(fd, MY_IOCTL_GET_VIRT_ADDR, &virt_addr) < 0) {
        perror("Failed to execute ioctl");
        close(fd);
        return 1;
    }

    printf("Virtual address from driver: %p\n", virt_addr);

    // Example: Access the first 4 bytes of the mapped memory
    ptr = (unsigned int*)virt_addr;
    printf("Value at virtual address: 0x%x\n", *ptr);

    close(fd);

    return 0;
}
*/