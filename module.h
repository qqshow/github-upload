/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : module.h
 * Author        : lb
 * Date          : 2014-09-19
 * Description   : module header file.
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-19
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/

#ifndef MODULE_H_INCLUDED
#define MODULE_H_INCLUDED

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/file.h>
#include <linux/mman.h>
#include <linux/binfmts.h>
#include <linux/version.h>
#include <linux/utsname.h>
#include <linux/kallsyms.h>
#include <linux/dcache.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/sysctl.h>
#include <linux/err.h>
#include <linux/namei.h>
#include <linux/fs_struct.h>
#include <linux/mount.h>


#include <asm/uaccess.h>
#include <asm/insn.h>

#ifndef CONFIG_SECURITY
#error "This module requires CONFIG_SECURITY to be enabled"
#endif


#if defined(__linux__)
    #define ULONG unsigned long
    #define ULONGLONG unsigned long long
	#define ulonglong unsigned long long
    #define WCHAR char

    typedef struct _GUID {
        unsigned long  Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char  Data4[ 8 ];
    } GUID;

    #if defined(MIDL_PASS)
    typedef struct _LARGE_INTEGER {
    #else // MIDL_PASS
    typedef union _LARGE_INTEGER {
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        };
        struct {
            unsigned long LowPart;
            unsigned long HighPart;
        } u;
    #endif //MIDL_PASS
        unsigned long long QuadPart;
    } LARGE_INTEGER;
#endif





#define MODULE_NAME "rtbackup"
#define PKPRE "[" MODULE_NAME "] "
#define MAX_FILE_LEN 256
#define TPE_HARDCODED_PATH_LEN 1024

#define LOG_FLOODTIME 5
#define LOG_FLOODBURST 5

#define OP_JMP_SIZE 5

#define IN_ERR(x) (x < 0)

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 28)
#define get_task_uid(task) task->uid
#define get_task_parent(task) task->parent
#else
#define get_task_uid(task) task->cred->uid
#define get_task_parent(task) task->real_parent
#endif

// d_path changed argument types. lame

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25)
#define get_d_path(file, buf, len) d_path(file->f_dentry, file->f_vfsmnt, buf, len);
#else
#define get_d_path(file, buf, len) d_path(&file->f_path, buf, len);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
#define get_inode(file) file->f_dentry->d_inode;
#define get_parent_inode(file) file->f_dentry->d_parent->d_inode;
#else
#define get_inode(file) file->f_path.dentry->d_inode;
#define get_parent_inode(file) file->f_path.dentry->d_parent->d_inode;
#endif

// 
struct kernsym {
	void *addr; // orig addr
	void *end_addr;
	unsigned long size;
	char *name;
	bool name_alloc; // whether or not we alloc'd memory for char *name
	u8 orig_start_bytes[OP_JMP_SIZE];
	void *new_addr;
	unsigned long new_size;
	bool found;
	bool hijacked;
	void *run;
};

extern struct kernsym sym_vfs_mkdir;
extern struct kernsym sym_vfs_write;
extern struct kernsym sym_vfs_create;
extern struct kernsym sym_vfs_symlink;
extern struct kernsym sym_vfs_link;
extern struct kernsym sym_vfs_rmdir;
extern struct kernsym sym_vfs_unlink;
extern struct kernsym sym_vfs_rename;



int symbol_hijack(struct kernsym *, const char *, unsigned long *);
void symbol_restore(struct kernsym *);

void hijack_syscalls(void);
void undo_hijack_syscalls(void);

void symbol_info(struct kernsym *);

int find_symbol_address(struct kernsym *, const char *);

int kernfunc_init(void);

void rtb_insn_init(struct insn *, const void *);
void rtb_insn_get_length(struct insn *insn);
int rtb_insn_rip_relative(struct insn *insn);

void *malloc(unsigned long size);
void malloc_free(void *buf);

int getabsparentpath(const char * pathname,char *abspath);
int getabsparentpathfromdentry(struct dentry *dentry,char *abspath);
int getabsfullpathfromdentry(struct dentry *dentry, char *abspath);
int getabsfullpath(const char *pathname, char *abspath);
int getabsfullpathfromstructfile(struct file * file, char *abspath);





#endif
