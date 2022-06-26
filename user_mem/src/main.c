#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include "my_alloc.h"

int main(int argc, char **argv)
{
    uintptr_t vaddr = 0;
    mem_region_t my_ptr;

    if(my_alloc_area_mem(&my_ptr, 10))
    {
        return EXIT_FAILURE;
    }

    vaddr = (uintptr_t)(my_ptr.mVirtAddress);
    memset((void *)vaddr, 10, 10);

    printf("Value at  0x%jx is set:\n", (uintmax_t)vaddr);
    for (int i = 0; i < 10; i++)
    {
        printf("%d\n", *(char *)(vaddr + i));
    }
    my_free_mem(&my_ptr);

    return EXIT_SUCCESS;
}