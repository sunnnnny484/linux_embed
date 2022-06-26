CC2 := /home/khaidq3/beagle_bone_black/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

vpath = /home/khaidq3/beagle_bone_black/linux_practice/user_mem/src
SRC := test2.c my_alloc.c
INCFLAGS := -I/home/khaidq3/beagle_bone_black/linux_practice/user_mem/include
SRCS := $(addprefix $(vpath)/,$(SRC))
HEADER := my_alloc.h

build: $(SRCS)
	$(CC2) $(INCFLAGS) $(SRCS) -o alloc2

clean:
	rm alloc
