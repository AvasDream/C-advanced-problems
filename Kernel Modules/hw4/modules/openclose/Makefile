ifneq ($(KERNELRELEASE),)
M ?= $(shell basename $(PWD))
obj-m := $(M).o

else
ARCH            ?= aarch64
KERNEL_ARCH     ?= arm64
CROSS_COMPILE   ?= ${ARCH}-linux-gnu-
CC              ?= ${CROSS_COMPILE}gcc
PWD             ?= $(shell pwd)
KDIR             ?= $(PWD)/../../artifacts/linux-4.11/

default:
	ARCH=$(KERNEL_ARCH) CROSS_COMPILE=$(CROSS_COMPILE) $(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif

clean:
	rm -rf *.ko *.o *.mod.c .*.cmd .tmp_versions *.symvers *.order test.o test

test:
	aarch64-linux-gnu-gcc -static -o openclose.ko.test openclosetest.c
	aarch64-linux-gnu-strip --strip-unneeded openclose.ko.test
	mv openclose.ko.test ../../artifacts