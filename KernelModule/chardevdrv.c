#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/random.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static int rVal;

static int dev_open(struct inode* i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}

static int dev_close(struct inode *i, struct file *f)
{
   
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static ssize_t dev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    // generate a random non-negative integer (4 bytes)
       
    printk(KERN_INFO "Driver: read()\n");
    get_random_bytes(&rVal, 4);
    
    /* You have just a single char in your buffer, so only 0 offset is valid */
    if (*off > 0) return 0; /* End of file */
    if (copy_to_user(buf, &rVal, 4))
        return -EFAULT;
    *off = *off + 4;
    return 4;
}

static ssize_t dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Driver: write()\n");
    // if (copy_from_user(&c, buf + len - 1, 1) != 0)
    // {
    //     return -EFAULT;
    // }
    return len;
}

static struct file_operations randnum_fops =
{
        .owner = THIS_MODULE,
        .open = dev_open,
        .release = dev_close,
        .read = dev_read,
        .write = dev_write
};

static int __init ofcd_init(void) /* Constructor */
{
    printk(KERN_INFO "Driver: ofcd registered");
    if (alloc_chrdev_region(&first, 0, 1, "RandNum") < 0)
    {
        return -1;
    }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(first, 1);
        return -1;
    }
    if (device_create(cl, NULL, first, NULL, "randnum") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    cdev_init(&c_dev, &randnum_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    return 0;
}

static void __exit ofcd_exit(void) /* Destructor */
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Driver: ofcd unregistered");
}

module_init(ofcd_init);
module_exit(ofcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Khanh Hoang");
MODULE_DESCRIPTION("Random 4-byte Number Generator Character Device Driver");