MODULE_NAME := rtbackup

# This subdirectory contains necessary files for both x86 and x86-64.
ARCH_DIR := arch/x86
EXTRA_CFLAGS += -I$(src) -I$(src)/$(ARCH_DIR)/include -I$(obj)/$(ARCH_DIR)/lib

# This auxiliary file will be generated during the build (x86 instruction 
# tables as C code).
INAT_TABLES_FILE := inat-tables.h

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
    rb.c \
    iowritethread.c
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
ccflags-y :=  -I$(src) -I$(src)/$(ARCH_DIR)/include -I$(obj)/$(ARCH_DIR)/lib

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
    rb.o \
    iowritethread.o \
    $(ARCH_DIR)/lib/inat.o \
    $(ARCH_DIR)/lib/insn.o

$(obj)/$(ARCH_DIR)/lib/inat.o: $(obj)/$(ARCH_DIR)/lib/$(INAT_TABLES_FILE) $(src)/$(ARCH_DIR)/lib/inat.c

$(obj)/$(ARCH_DIR)/lib/$(INAT_TABLES_FILE): $(src)/$(ARCH_DIR)/lib/x86-opcode-map.txt 
	LC_ALL=C awk -f $(src)/$(ARCH_DIR)/tools/gen-insn-attr-x86.awk $< > $@
	
endif
