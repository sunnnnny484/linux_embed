#ifndef POOL_H
#define POOL_H

int pool_init(unsigned int aTotalSize, unsigned int aAlign, unsigned int aPhysAddr);
int alloc_chunk(unsigned int aSize, unsigned int *aPhysAddr);
int free_chunk(unsigned int aSize, unsigned int aPhysAddr);

#endif