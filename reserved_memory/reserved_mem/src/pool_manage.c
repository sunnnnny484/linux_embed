#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include "pool_manage.h"

#define BIT_PER_BITSET (sizeof(unsigned long) * 8)

static unsigned int *gPhysPool;
static unsigned long *gBitset;
static unsigned int sAlign;
static int numBitset;
static unsigned int sStartPhysAddr;

int pool_init(unsigned int aTotalSize, unsigned int aAlign, unsigned int aPhysAddr)
{
    int numPool = 0;
    int ret = 0;
    int unusedBit;
    int index;

    sAlign = aAlign;
    sStartPhysAddr = aPhysAddr;
    numPool = aTotalSize / aAlign;
    printk("num pool = %d\n", numPool);
    numBitset = (numPool + BIT_PER_BITSET - 1) / BIT_PER_BITSET;
    unusedBit = numBitset * BIT_PER_BITSET - numPool;

    gPhysPool = (int *)vmalloc(numPool * sizeof(int));
    if (gPhysPool == NULL)
    {
        printk("Allocate pool failed.\n");
        ret = -1;
        goto pool_init_exit;
    }

    for (index = 0; index < numPool; index++)
    {
        gPhysPool[index] = aPhysAddr + index * aAlign;
    }

    gBitset = (unsigned long *)vmalloc(numBitset * sizeof(unsigned long));
    if (gBitset == NULL)
    {
        printk("Allocate bitset failed.\n");
        ret = -1;
        goto pool_init_exit;
    }

    memset((void *)gBitset, 0, numBitset);
    gBitset[numBitset - 1] |= (((unsigned long)1 << unusedBit) - 1);

pool_init_exit:
    return ret;
}

int alloc_chunk(unsigned int aSize, unsigned int *aPhysAddr)
{
    int ret = 0;
    int numChunk;
    int searchBitset;
    int index1, index2;

    numChunk = (aSize + sAlign - 1) / sAlign;
    searchBitset = (1 << numChunk) - 1;
    printk("numBitset = %d\n", numBitset);
    for (index1 = 0; index1 < numBitset; index1++)
    {
        printk("Bit set %d: 0x%lx\n", index1, gBitset[index1]);
        for (index2 = 0; index2 < BIT_PER_BITSET - numChunk; index2++)
        {
            // tim thay chunk free
            printk("Chunk address: 0x%x\n", gPhysPool[index1 * BIT_PER_BITSET + index2]);
            if (((searchBitset << index2) & gBitset[index1]) == 0)
            {
                gBitset[index1] |= (searchBitset << index2);
                *aPhysAddr = gPhysPool[index1 * BIT_PER_BITSET + index2];
                printk("Set address 0x%x free -> used\n", *aPhysAddr);
                goto alloc_chunk_exit;
            }
        }
    }
    printk("No more chunk available\n");
    ret = -1;

alloc_chunk_exit:
    return ret;
}

int free_chunk(unsigned int aSize, unsigned int aPhysAddr)
{
    int ret = 0;
    unsigned int startFreeChunkIndex;
    unsigned int numFreeChunk;

    if (aPhysAddr % sAlign != 0)
    {
        ret = -1;
        printk("Free address is not align\n");
        goto free_chunk_exit;
    }
    startFreeChunkIndex = (aPhysAddr - sStartPhysAddr) / sAlign;

    numFreeChunk = (aSize + sAlign - 1) / sAlign;
    //set bit from used -> free
    gBitset[startFreeChunkIndex / BIT_PER_BITSET] &= ~(((1 << numFreeChunk) - 1) << (startFreeChunkIndex % BIT_PER_BITSET));
    printk("Free address: 0x%x\n", aPhysAddr);

free_chunk_exit:
    return ret;
}