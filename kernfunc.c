/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : kernfunc.c
 * Author        : lb
 * Date          : 2014-09-19
 * Description   : find needed kernel symbols
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-19
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/

#include "module.h"

static struct kernsym sym_module_alloc;
static struct kernsym sym_module_free;

static struct kernsym sym_insn_init;
static struct kernsym sym_insn_get_length;
static struct kernsym sym_insn_rip_relative;

// locate the kernel symbols we need that aren't exported

int kernfunc_init(void) {

	int ret;

	ret = find_symbol_address(&sym_module_alloc, "module_alloc");

	if (IN_ERR(ret))
		return ret;

	ret = find_symbol_address(&sym_module_free, "module_free");

	if (IN_ERR(ret))
		return ret;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 18)

	ret = find_symbol_address(&sym_insn_init, "insn_init");

	if (IN_ERR(ret))
		return ret;
	
	ret = find_symbol_address(&sym_insn_get_length, "insn_get_length");

	if (IN_ERR(ret))
		return ret;
	
	ret = find_symbol_address(&sym_insn_rip_relative, "insn_rip_relative");

	if (IN_ERR(ret))
		return ret;
#endif
	
	return 0;
}

// call to module_alloc

void *malloc(unsigned long size) {
	void *(*run)(unsigned long) = sym_module_alloc.run;
	return run(size);
}

// call to module_free

void malloc_free(void *buf) {
	void (*run)(struct module *, void *) = sym_module_free.run;
	if (buf != NULL)
		run(NULL, buf);
}

// call to insn_init

void rtb_insn_init(struct insn *insn, const void *kaddr) {
#if(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18))
	void (*run)(struct insn *, const void *, int) = sym_insn_init.run;
	run(insn, kaddr,
#ifdef CONFIG_X86_64
		1
#else // CONFIG_X86_32
		0
#endif
		);
#else
	insn_init(insn,kaddr,
#ifdef CONFIG_X86_64
		1
#else // CONFIG_X86_32
		0
#endif
		);
#endif
}

// call to insn_get_length

void rtb_insn_get_length(struct insn *insn) {
#if(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18))
	void (*run)(struct insn *) = sym_insn_get_length.run;
	run(insn);
#else
	insn_get_length(insn);
#endif
}

// call to insn_rip_relative

int rtb_insn_rip_relative(struct insn *insn) {
#if(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18))
	int (*run)(struct insn *) = sym_insn_rip_relative.run;
	return run(insn);
#else
	insn_rip_relative(insn);
#endif
}

