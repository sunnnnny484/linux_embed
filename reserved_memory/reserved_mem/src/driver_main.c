#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/mm.h>
#include "linux_device.h"
#include "pool_manage.h"

#define RES_MEM_DTS_NODE  "/reserved_dram"
#define DEV_FILE_NAME "reserved_mem"

#define ALLOC_CMD _IOWR(12, 1, unsigned int *)
#define FREE_CMD _IOW(3, 2, unsigned int *)

static mem_dev_file_t res_mem_dev;

static int reserved_mem_mmap(struct file *aFile, struct vm_area_struct *aVma)
{
    int ret = 0;

    if ((aFile == NULL) || (aVma == NULL))
    {
        ret = -1;
        printk("Invalid input. %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        goto res_mem_mmap_exit;
    }

    aVma->vm_flags |= (VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_DONTDUMP);

    ret = remap_pfn_range(aVma, aVma->vm_start, aVma->vm_pgoff,
                            aVma->vm_end - aVma->vm_start, aVma->vm_page_prot);
    if (ret != 0)
    {
        printk("Remap page failed. %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
        goto res_mem_mmap_exit;
    }

res_mem_mmap_exit:
    return ret;
}

static long reserved_mem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    unsigned int kernel_buff[2]; // size | physAddr

    printk("Handle ioctl event (cmd: %u)\n", cmd);
    switch(cmd)
    {
        case ALLOC_CMD:
        {
            copy_from_user(kernel_buff, (unsigned int *)arg, sizeof(kernel_buff));
            printk("kernel_buffer: %d   0x%x\n", kernel_buff[0], kernel_buff[1]);
            ret = alloc_chunk(kernel_buff[0], &kernel_buff[1]);
            if(ret == 0)
            {
                copy_to_user((unsigned int *)arg, kernel_buff, sizeof(kernel_buff));
                printk("Alloc success\n");
            }
            else
            {
                printk("Alloc failed\n");
            }
        }
        break;

        case FREE_CMD:
        {
            copy_from_user(kernel_buff, (unsigned int *)arg, sizeof(kernel_buff));
            ret = free_chunk(kernel_buff[0], kernel_buff[1]);
            if (ret == 0)
            {
                printk("Free success\n");
            }
            else
            {
                printk("Free failed\n");
            }
        }
        break;

        default: break;
    }

    return ret;
}

static struct file_operations reserved_mem_fops = {
	.owner	 = THIS_MODULE,
    .mmap    = reserved_mem_mmap,
    .unlocked_ioctl = reserved_mem_ioctl,
};

static struct of_device_id dts_match_table[] = {
    {.compatible = "res_mem_region", },
};

static int __init res_mem_driver_init(struct platform_device *pdev)
{
    int ret = 0;
    int reg_data[2];
    unsigned int size, align, start_address;

    printk(" %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    ret = of_property_read_u32_array(pdev->dev.of_node, "reg", reg_data, 2);
    if (ret < 0)
    {
        printk("Failed to read reg data.\n");
        ret = -1;
        goto res_mem_driver_init_exit;
    }
    start_address = reg_data[0];
    size = reg_data[1];
    printk("Start phys: 0x%x   Size: 0x%x\n",start_address, size);

    ret = of_property_read_u32(pdev->dev.of_node, "alignment", &align);
    if (ret < 0)
    {
        printk("Failed to read alignment.\n");
        ret = -1;
        goto res_mem_driver_init_exit;
    }
    printk("Alignment: 0x%x\n", align);

    ret = pool_init(size, align, start_address);
    if (ret == 0)
    {
        printk("Init pool success\n");
    }
    else
    {
        printk("Failed to init pool\n");
        ret = -1;
        goto res_mem_driver_init_exit;
    }
    ret = init_dev_file(&res_mem_dev, &reserved_mem_fops, DEV_FILE_NAME);

res_mem_driver_init_exit:
    return ret;
}

static int __exit res_mem_driver_exit(struct platform_device *pdev)
{
    printk(KERN_INFO "VCL quas\n");
    printk(" %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
    destroy_dev_file(&res_mem_dev);

    return 0;
}

static struct platform_driver reserved_mem_driver = {
      .remove = __exit_p(res_mem_driver_exit),
      .driver = {
        .name = "reserved_memory",
        .owner = THIS_MODULE,
        .of_match_table = dts_match_table,         // Add the device tree list to the driver
      },
};

module_platform_driver_probe(reserved_mem_driver, res_mem_driver_init);

MODULE_AUTHOR("QuangVA");
MODULE_LICENSE("GPL");