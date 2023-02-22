PWD := $(shell pwd)

BUILD_DIR := /home/khaidq3/beagle_bone_black/linux_practice/reserved_mem/build
SRC_DIR := /home/khaidq3/beagle_bone_black/linux_practice/reserved_mem/src
INC_DIR := /home/khaidq3/beagle_bone_black/linux_practice/reserved_mem/include
MODULE_NAME := reserved_mem

ccflags-y := -Wall -I$(INC_DIR)
obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := ../src/linux_device.o ../src/driver_main.o ../src/pool_manage.o


$(info 111 $(PWD))
KERNEL_DIR := /home/khaidq3/beagle_bone_black/kernelbuildscripts/KERNEL
CC1 := /home/khaidq3/beagle_bone_black/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

target:
	make ARCH=arm CROSS_COMPILE=$(CC1) -C $(KERNEL_DIR) M=$(PWD) modules
	@mv $(SRC_DIR)/*.o $(BUILD_DIR)/
	@mv $(SRC_DIR)/.driver_main.o.cmd $(BUILD_DIR)/
	@mv $(SRC_DIR)/.linux_device.o.cmd $(BUILD_DIR)/
	@mv $(SRC_DIR)/.pool_manage.o.cmd $(BUILD_DIR)

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
