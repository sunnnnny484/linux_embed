#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "my_alloc.h"

#define MEM_DEV_FILE "/dev/reserved_mem"

int my_alloc_area_mem(mem_region_t *aMemArea, size_t aAllocSize)
{
    int ret = 0;
    int fd; //Point to reserve memory device file
    unsigned int user_buffer[2]; // size | physAddr

    if (aMemArea == NULL)
    {
        ret = -1;
        printf("Failed to allocate memory: Invalid input.\n");
        goto alloc_area_mem_exit;
    }

    fd = open(MEM_DEV_FILE, O_RDWR);
    if (fd < 0)
    {
        printf("Failed to open memory device file.\n");
        ret = -1;
        goto alloc_area_mem_exit;
    }
    user_buffer[0] = aAllocSize;
    ret = ioctl(fd, ALLOC_CMD, user_buffer);
    printf("Phys addr get: 0x%x\n", user_buffer[1]);
    aMemArea->mPhysAddr = (char *)user_buffer[1];
    if (ret != 0)
    {
        printf("call ioctl failed\n");
        ret = -1;
        goto alloc_area_mem_exit;
    }
    aMemArea->mVirtAddress = (char *)mmap(NULL, aAllocSize, PROT_READ | PROT_WRITE, \
                                 MAP_SHARED, fd, (off_t)aMemArea->mPhysAddr);
    printf("Day la dia chi da map: %x\n", aMemArea->mVirtAddress);
    if (aMemArea->mVirtAddress == (char *)MAP_FAILED)
    {
        ret = -1;
        printf("Failed to map to virtual address.\n");
        goto alloc_area_mem_exit;
    }
    aMemArea->mSize = aAllocSize;

alloc_area_mem_exit:
    close(fd);
    return ret;
}

int my_free_mem(mem_region_t *aMemArea)
{
    int ret = 0;
    unsigned int user_buffer[2]; // size | physAddr
    int fd;

    fd = open(MEM_DEV_FILE, O_RDWR);
    if (fd < 0)
    {
        printf("Failed to open /dev/reserved_mem\n");
        ret = -1;
        goto free_mem_exit;
    }

    if (aMemArea == NULL)
    {
        ret = -1;
        printf("Failed to free memory: Invalid input.\n");
        goto free_mem_exit;
    }
    user_buffer[0] = aMemArea->mSize;
    user_buffer[1] = (unsigned int)aMemArea->mPhysAddr;
    ret = ioctl(fd, FREE_CMD, user_buffer);

    ret = munmap((void *)aMemArea->mVirtAddress, aMemArea->mSize);
    if (ret != 0)
    {
        printf("Failed to unmap virtual address.\n");
    }

free_mem_exit:
    return ret;
}