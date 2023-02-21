#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/list.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include "utils.h"

#define DEVICE_NAME "iut_device"
static int major;

static long iut_ioctl(struct file *file,
                      unsigned int req,
                      unsigned long pointer) {
    
        char data[100];

    	switch (req) {
        case Write:
            if (copy_from_user(data, (void *) pointer, 100)) {
        	return -EFAULT;
    		}
	    printk(KERN_INFO "this is from user %s\n", data);
            return 1;
	case Read:
	    printk(KERN_INFO "Read is working\n");
	    if (copy_to_user((void *) pointer, "hello from kernel to dear user", 30))
	    {
		    return -EFAULT;
	    }
	    return 1;
    }

    return -EINVAL;
}

static int iut_open(struct inode *inode, struct file *file) {
    printk("device opened\n");
    return 0;
}

static int iut_release(struct inode *inode, struct file *file) {
    printk("device closed\n");
    return 0;
}

static const struct file_operations fops = {
        .open        = iut_open,
        .release    = iut_release,
        .unlocked_ioctl        = iut_ioctl,
};
static int __init iut_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops); 
    if (major < 0) {
        printk(KERN_ALERT
        "Device001 load failed!\n");
        return major;
    }
    printk(KERN_INFO
    "iut device module has been loaded: %d\n", major);
    return 0;
}

static void __exit iut_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO
    "iut device module has been unloaded.\n");
}

module_init(iut_init);
module_exit(iut_exit);
MODULE_LICENSE("GPL");
