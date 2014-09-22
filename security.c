
#include "module.h"

struct kernsym sym_vfs_mkdir;
struct kernsym sym_sys_mkdir;
struct kernsym sym_sys_write;
struct kernsym sym_vfs_write;

int (rtb_vfs_mkdir)(struct inode *dir, struct dentry *dentry, int mode)
{
    int(*run)(struct inode *,struct dentry *, int) = sym_vfs_mkdir.run;
	int ret;
    char *abspath = NULL;
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		printk("vfs_mkdir.....malloc error\n");
        return run(dir,dentry,mode);
	}
	memset(abspath,0,PATH_MAX);
    if(dir)
    {
        printk("vfs_mkdir.....\n");
    }
    
    printk("dentry->dname %s.\n",dentry->d_name.name);
    printk("dentry->dparent  %s.\n",dentry->d_parent->d_name.name);
    getabsfullpathfromdentry(dentry,abspath);
    ret = run(dir,dentry,mode);
    return ret;   
}

// mmap
long (rtb_sys_mkdir)(const char __user *pathname, int mode)
{
    long(*run)(const char __user *, int) = sym_sys_mkdir.run;
	long ret=0,iret=0;
    
    char *abspath = NULL;
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		goto out;
	}
	memset(abspath,0,PATH_MAX);
	

	printk("mkdir %s\n",pathname);
	iret = getabsfullpath(pathname,abspath);
    
    if(strncmp(pathname,"lbtest123",sizeof("lbtest123")) == 0)
    {
        printk("mkdir %s denied\n",pathname);
        ret = -1;
        goto out;
    }
    
    ret = run(pathname,mode);
    if(ret >= 0)
    {
        printk("mkdir success %ld\n",ret);
    }
    else
    {
        printk("mkdir failed %ld\n",ret);
    }
    out:
    if(abspath)
        kfree(abspath);
    return ret;
}
long (rtb_sys_write)(unsigned int fd, const char __user *buf, size_t count)
{
    long(*run)(unsigned int, const char __user *, size_t) = sym_sys_write.run;
	long ret;
    if(fd)
    {
        printk("sys write...\n");
    }
    ret = run(fd,buf,count);
    return ret;
}

ssize_t rtb_vfs_write(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
    ssize_t(*run)(struct file *, const char __user *, size_t, loff_t *) = sym_vfs_write.run;
	ssize_t ret;
    
    if(file)
    {
        printk("vfs write ....\n");
    }
    
    ret = run(file,buf,count,pos);
    return ret;
}


void printfail(const char *name) {
	printk(PKPRE "warning: unable to implement protections for %s\n", name);
}

struct symhook {
	char *name;
	struct kernsym *sym;
	unsigned long *func;
};

struct symhook security2hook[] = {
    //{"sys_write", &sym_sys_write,(unsigned long *)rtb_sys_write},
    //{"vfs_write", &sym_vfs_write,(unsigned long *)rtb_vfs_write},
    {"vfs_mkdir", &sym_vfs_mkdir, (unsigned long *)rtb_vfs_mkdir},
    {"sys_mkdir", &sym_sys_mkdir, (unsigned long *)rtb_sys_mkdir},
};

// hijack the needed functions. whenever possible, hijack just the LSM function

void hijack_syscalls(void) {

	int ret, i;

	for (i = 0; i < ARRAY_SIZE(security2hook); i++) {
		ret = symbol_hijack(security2hook[i].sym, security2hook[i].name, security2hook[i].func);

		if (IN_ERR(ret))
			printfail(security2hook[i].name);
	}

}

void undo_hijack_syscalls(void) {
	int i;

	for (i = 0; i < ARRAY_SIZE(security2hook); i++)
		symbol_restore(security2hook[i].sym);
}

