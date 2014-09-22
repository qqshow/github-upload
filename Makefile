MODULE_NAME := rtbackup

EXTRA_CFLAGS += -I$(src)

ifeq ($(KERNELRELEASE),)


MODULE_SOURCES := \
    module.c \
    getpath.c \
	security.c \
	symbols.c \
	kernfunc.c \
	hijacks.c

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
	security.o \
	symbols.o \
	kernfunc.o \
	hijacks.o

endif
