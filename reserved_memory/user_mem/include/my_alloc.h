#ifndef _MY_ALLOC_H_
#define _MY_ALLOC_H_

typedef struct mem_region {
    char *mVirtAddress;
    char *mPhysAddr;
    size_t mSize;
} mem_region_t;

#define ALLOC_CMD _IOWR(12, 1, unsigned int *)
#define FREE_CMD _IOW(3, 2, unsigned int *)

int my_alloc_area_mem(mem_region_t *aMemArea, size_t aAllocSize);
int my_free_mem(mem_region_t *aMemArea);

#endif