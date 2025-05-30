#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fb.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/platform_device.h>

#define DRIVER_NAME "simplefb"
#define FB_PHYS_ADDR 0x40000000
#define FB_SIZE (1024 * 768 * 4)  // Example: 1024x768 with 32bpp

static struct fb_info *fbinfo;
static void __iomem *framebuffer_base;

static struct fb_fix_screeninfo simplefb_fix = {
    .id = DRIVER_NAME,
    .smem_start = FB_PHYS_ADDR,
    .smem_len = FB_SIZE,
    .type = FB_TYPE_PACKED_PIXELS,
    .visual = FB_VISUAL_TRUECOLOR,
    .xpanstep = 0,
    .ypanstep = 0,
    .line_length = 1024 * 4, // Example: 1024 pixels * 4 bytes per pixel
    .mmio_start = 0,
    .mmio_len = 0,
    .accel = FB_ACCEL_NONE,
};

static struct fb_var_screeninfo simplefb_var = {
    .xres = 1024,
    .yres = 768,
    .xres_virtual = 1024,
    .yres_virtual = 768,
    .bits_per_pixel = 32,
    .red = { .offset = 16, .length = 8, .msb_right = 0 },
    .green = { .offset = 8, .length = 8, .msb_right = 0 },
    .blue = { .offset = 0, .length = 8, .msb_right = 0 },
    .transp = { .offset = 24, .length = 8, .msb_right = 0 },
    .activate = FB_ACTIVATE_NOW,
};

static int simplefb_probe(struct platform_device *pdev)
{
    int ret = 0;

    fbinfo = framebuffer_alloc(0, &pdev->dev);
    if (!fbinfo) {
        dev_err(&pdev->dev, "Framebuffer allocation failed\n");
        return -ENOMEM;
    }

    fbinfo->screen_base = ioremap(FB_PHYS_ADDR, FB_SIZE);
    if (!fbinfo->screen_base) {
        dev_err(&pdev->dev, "ioremap failed\n");
        ret = -ENOMEM;
        goto err_release_fb;
    }

    framebuffer_base = fbinfo->screen_base;

    fbinfo->var = simplefb_var;
    fbinfo->fix = simplefb_fix;
    fbinfo->pseudo_palette = NULL; // Optional: If you need a palette

    fbinfo->fbops = &((struct fb_ops) {
        .owner = THIS_MODULE,
        .fb_fillrect = sys_fillrect,
        .fb_copyarea = sys_copyarea,
        .fb_imageblit = sys_imageblit,
    });

    fbinfo->flags = FBINFO_FLAG_DEFAULT;
    fbinfo->device = &pdev->dev;

    ret = register_framebuffer(fbinfo);
    if (ret < 0) {
        dev_err(&pdev->dev, "Framebuffer registration failed\n");
        goto err_iounmap;
    }

    platform_set_drvdata(pdev, fbinfo);

    dev_info(&pdev->dev, "simplefb: Framebuffer at 0x%lx (virtual 0x%p), size %d\n",
             (unsigned long)FB_PHYS_ADDR, fbinfo->screen_base, FB_SIZE);

    return 0;

err_iounmap:
    iounmap(fbinfo->screen_base);
err_release_fb:
    framebuffer_release(fbinfo);
    return ret;
}

static int simplefb_remove(struct platform_device *pdev)
{
    struct fb_info *fbinfo = platform_get_drvdata(pdev);

    if (fbinfo) {
        unregister_framebuffer(fbinfo);
        iounmap(fbinfo->screen_base);
        framebuffer_release(fbinfo);
    }
    return 0;
}

static struct platform_driver simplefb_driver = {
    .probe = simplefb_probe,
    .remove = simplefb_remove,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
    },
};

static int __init simplefb_init(void)
{
    return platform_driver_register(&simplefb_driver);
}

static void __exit simplefb_exit(void)
{
    platform_driver_unregister(&simplefb_driver);
}

module_init(simplefb_init);
module_exit(simplefb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GitHub Copilot");
MODULE_DESCRIPTION("Simple framebuffer driver for a fixed physical address");