MODULE_NAME := rtbackup

EXTRA_CFLAGS += -I$(src)

ifeq ($(KERNELRELEASE),)


MODULE_SOURCES := \
    module.c \
    getpath.c \
    monitorset.c \
	hookvfs.c \
	symbols.c \
	kernfunc.c \
	hijacks.c \
    fileop.c \
    createiolog.c \
    rtbnetlink.c \
    rb.c
KBUILD_DIR=$(shell sh ./scripts/find_kernel_src.sh)
UNAME=$(shell uname -r)
PWD := $(shell pwd)

all: $(MODULE_NAME).ko

$(MODULE_NAME).ko: $(MODULE_SOURCES)

	$(MAKE) -C $(KBUILD_DIR) M=$(PWD) modules
	
clean:
	$(MAKE) -C $(KBUILD_DIR) M=$(PWD) clean

	rm -f Module* $(TESTS) tests.out

.PHONY: all clean

else
# KBuild part. 
# It is used by the kernel build system to actually build the module.
ccflags-y :=  -I$(src)

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-y := \
	module.o \
    getpath.o \
        monitorset.o \
	hookvfs.o \
	symbols.o \
	kernfunc.o \
	hijacks.o \
    fileop.o \
    createiolog.o \
    rtbnetlink.o \
    rb.o
endif
