#ifndef _LINUX_DEVICE_H_
#define _LINUX_DEVICE_H_

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>

#define MAX_NAME_LEN 64
#define REGION_SUFFIX "_reg"
#define CLASS_SUFFIX "_class"
#define DEV_NAME_MAX_LEN (MAX_NAME_LEN + \
                        sizeof(CLASS_SUFFIX))

typedef struct mem_dev_file {
    uint8_t mName[DEV_NAME_MAX_LEN];
    struct class *mClass;
    dev_t mDev;
    struct device *mDevice;
    struct device_node *mDevNode;
    struct cdev *mCdev;
} mem_dev_file_t;

int32_t init_dev_file(mem_dev_file_t *aDev, struct file_operations *aFop,
                        int8_t *aName);

int32_t destroy_dev_file(mem_dev_file_t *aDev);

#endif