#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/err.h>
#include "linux_device.h"

int init_dev_file(mem_dev_file_t *aDev, struct file_operations *aFop,
                        int8_t *aName)
{
    int ret = 0;
    size_t len = 0;

    /* Check length of dev file's name  */
    len = strnlen(aName, MAX_NAME_LEN);
    if (len == MAX_NAME_LEN)
    {
        printk(KERN_INFO "The device's name is too long.\n");
        len = MAX_NAME_LEN - 1;
    }

    /* Create class */
    (void)snprintf(aDev->mName, sizeof(aDev->mName), \
    "%s" CLASS_SUFFIX, aName);
    aDev->mClass = class_create(THIS_MODULE, aDev->mName);
    if (aDev->mClass == NULL)
    {
        printk("Failed to create class.\n");
        ret = -1;
        goto failed_create_class;
    }

    /* Alloc device number */
    (void)snprintf(aDev->mName, sizeof(aDev->mName), \
    "%s" REGION_SUFFIX, aName);
    ret = alloc_chrdev_region(&aDev->mDev, 0, 1, aDev->mName);
    if (ret < 0)
    {
        printk("Failed to alloc device number.\n");
        goto failed_alloc_device_num;
    }

    /* Create device */
    aDev->mDevice = device_create(aDev->mClass, NULL, aDev->mDev, \
    NULL, aName);
    if (IS_ERR(aDev->mDevice))
    {
        ret = -1;
        printk("Failed to create device file.\n");
        goto failed_device_create;
    }

    aDev->mCdev = cdev_alloc();
    if (aDev->mCdev == NULL)
    {
        printk("Failed to alloc cdev structure.\n");
        ret = -1;
        goto failed_alloc_cdev;
    }

    cdev_init(aDev->mCdev, aFop);
    ret = cdev_add(aDev->mCdev, aDev->mDev, 1);
    if (ret < 0)
    {
        printk("Failed to add char device.\n");
        goto failed_alloc_cdev;
    }

    goto init_dev_file_exit;

failed_alloc_cdev:
    cdev_del(aDev->mCdev);
failed_device_create:
    device_destroy(aDev->mClass, aDev->mDev);
failed_alloc_device_num:
    unregister_chrdev_region(aDev->mDev, 1);
failed_create_class:
    class_destroy(aDev->mClass);
init_dev_file_exit:
    return ret;
}

int destroy_dev_file(mem_dev_file_t *aDev)
{
    int ret = 0;

    if (aDev == NULL)
    {
        ret = -1;
        printk("Invalid input");
        goto destroy_dev_file_exit;
    }

    cdev_del(aDev->mCdev);
    device_destroy(aDev->mClass, aDev->mDev);
    unregister_chrdev_region(aDev->mDev, 1);
    class_destroy(aDev->mClass);

destroy_dev_file_exit:
    return ret;
}