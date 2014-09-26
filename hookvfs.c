/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : hookvfs.c
 * Author        : lb
 * Date          : 2014-09-19
 * Description   : hook vfs functions for real time backup data.
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-19
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/

#include "module.h"
#include "createiolog.h"
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/unistd.h>
#include <linux/limits.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/dcache.h>
#include <linux/fdtable.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/time.h>

// vfs function definition
/*
extern int vfs_create(struct inode *, struct dentry *, int, struct nameidata *);
extern int vfs_mkdir(struct inode *, struct dentry *, int);
extern int vfs_mknod(struct inode *, struct dentry *, int, dev_t);
extern int vfs_symlink(struct inode *, struct dentry *, const char *);
extern int vfs_link(struct dentry *, struct inode *, struct dentry *);
extern int vfs_rmdir(struct inode *, struct dentry *);
extern int vfs_unlink(struct inode *, struct dentry *);
extern int vfs_rename(struct inode *, struct dentry *, struct inode *, struct dentry *);
*/

struct kernsym sym_vfs_mkdir;
struct kernsym sym_vfs_write;
struct kernsym sym_sys_write;
struct kernsym sym_vfs_create;
struct kernsym sym_vfs_symlink;
struct kernsym sym_vfs_link;
struct kernsym sym_vfs_rmdir;
struct kernsym sym_vfs_unlink;
struct kernsym sym_vfs_rename;

struct kernsym sym_do_truncate;



// check need to log or not
int checkneedtolog(char *abspath)
{

	if(strncmp(abspath,"/test/",strlen("/test/")) == 0 )
//	if(strncmp(abspath,"/iologs/",strlen("/iologs/")) != 0 && strncmp(abspath,"/dev/",strlen("/dev/")) != 0\
//		&& strncmp(abspath,"/var/log/",strlen("/var/log/")) != 0)
		return 1;
	else
		return 0;
}


int (rtb_do_truncate)(struct dentry *dentry, loff_t length, unsigned int time_attrs,
	struct file *filp)
{
	int(*run)(struct dentry *,loff_t,unsigned int,struct file *) = sym_do_truncate.run;
	int ret;
	char *abspath = NULL;
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		printk("do_truncate.....malloc error\n");
        return run(dentry,length,time_attrs,filp);
	}
	memset(abspath,0,PATH_MAX);

//	getabsfullpathfromstructfile(filp,abspath);

//	if(checkneedtolog( abspath))
//	{
//		printk("do_truncate %s. %lld.\n",abspath,length);
//	}
	if(length < 1000000000)
		printk("do_truncate.....%lld\n",length);
	if(abspath)
		kfree(abspath);

	return run(dentry,length,time_attrs,filp);
}


// hijack for rename file or dir
int (rtb_vfs_rename)(struct inode *old_dir, struct dentry *old_dentry,
	       struct inode *new_dir, struct dentry *new_dentry)
{
    int(*run)(struct inode *,struct dentry *,struct inode *,struct dentry *) = sym_vfs_rename.run;
	int ret;
    char *oldabspath = NULL;
	char *newabspath = NULL;
	oldabspath = kmalloc(PATH_MAX, GFP_KERNEL);
	newabspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(oldabspath == NULL || newabspath == NULL)
	{
		printk("vfs_rmname.....malloc error\n");
       	goto out;
	}
	memset(oldabspath,0,PATH_MAX);
	memset(newabspath,0,PATH_MAX);
    
    getabsfullpathfromdentry(old_dentry,oldabspath);
	getabsfullpathfromdentry(new_dentry,newabspath);
	printk("Rename from %s to %s.\n",oldabspath,newabspath);

out:
	ret = run(old_dir,old_dentry,new_dir,new_dentry);
	/* move include move in and move out. Don't forget to deal with .   add by lb, 2014-09-24 */ 
	if(ret == 0 && (checkneedtolog(oldabspath) || checkneedtolog(newabspath)))
		createiologforrename(getseqno(),oldabspath,newabspath);
	if(oldabspath)
        kfree(oldabspath);

	if(newabspath)
        kfree(newabspath);
    return ret;  


}

//hijack for delete file 
int (rtb_vfs_unlink)(struct inode *dir, struct dentry *dentry)
{
    int(*run)(struct inode *,struct dentry *) = sym_vfs_unlink.run;
	int ret;
    char *abspath = NULL;
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		printk("vfs_unlink.....malloc error\n");
        return run(dir,dentry);
	}
	memset(abspath,0,PATH_MAX);
    if(dir)
    {
        printk("vfs_unlink.....\n");
    }
    
    getabsfullpathfromdentry(dentry,abspath);
	printk("vfs_unlink %s.\n",abspath);
    ret = run(dir,dentry);
	if(ret == 0 && checkneedtolog(abspath))
		createiologforrmfile(getseqno(),abspath);
	if(abspath)
        kfree(abspath);
    return ret;   


}

//hijack for rmdir
int (rtb_vfs_rmdir)(struct inode *dir, struct dentry *dentry)
{
    int(*run)(struct inode *,struct dentry *) = sym_vfs_rmdir.run;
	int ret;
    char *abspath = NULL;
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		printk("vfs_rmdir.....malloc error\n");
        return run(dir,dentry);
	}
	memset(abspath,0,PATH_MAX);
    if(dir)
    {
        printk("vfs_rmdir.....\n");
    }
    
    printk("dentry->dname %s.\n",dentry->d_name.name);
    printk("dentry->dparent  %s.\n",dentry->d_parent->d_name.name);
    getabsfullpathfromdentry(dentry,abspath);
	printk("vfs_rmdir %s.\n",abspath);
    ret = run(dir,dentry);
	printk("vfs_rmdir return %d.\n",abspath);
	if(ret == 0 && checkneedtolog(abspath))
		createiologforrmdir(getseqno(), abspath);
	if(abspath)
        kfree(abspath);
    return ret;   


}

//hijack for create file
int (rtb_vfs_create)(struct inode *dir, struct dentry *dentry, int mode,
		struct nameidata *nd)
{
	int(*run)(struct inode *,struct dentry *, int ,struct nameidata *) = sym_vfs_create.run;
	int ret = 0,iret = 0;
    char *abspath = NULL;
	if(dentry->d_inode != NULL)
	{
		printk("open file\n");
		goto out;
	}
	printk("create new file\n");
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		printk("vfs_create.....malloc error\n");
        goto out;
	}
	memset(abspath,0,PATH_MAX);
    if(dir)
    {
        printk("vfs_create.....\n");
    }
    
	iret = getabsfullpathfromdentry(dentry,abspath);
	printk("vfs_create file %s\n",abspath);

out:
	if(abspath)
		kfree(abspath);
	ret = run(dir,dentry,mode,nd);
	if(ret == 0 && checkneedtolog(abspath))
		createiologforcreatefile(getseqno(),abspath,mode);
	return ret;
}

// hijack for create symbolic link
int (rtb_vfs_symlink)(struct inode *dir, struct dentry *dentry, const char *oldname)
{
		int(*run)(struct inode *,struct dentry *, const char *) = sym_vfs_symlink.run;
		int ret;
		char *oldabspath = NULL;
		char *newabspath = NULL;
		oldabspath = kmalloc(PATH_MAX, GFP_KERNEL);
		newabspath = kmalloc(PATH_MAX, GFP_KERNEL);
		if(oldabspath == NULL || newabspath == NULL)
		{
			printk("vfs_symlink.....malloc error\n");
			goto out;
		}
		memset(oldabspath,0,PATH_MAX);
		memset(newabspath,0,PATH_MAX);
		if(dir)
		{
			printk("vfs_symlink.....\n");
		}
		getabsfullpath(oldname,oldabspath);
		getabsfullpathfromdentry(dentry,newabspath);

		printk("create symlink from %s to %s\n",oldabspath,newabspath);
	out:
		ret = run(dir,dentry,oldname);
		printk("create symlink return %d.\n",ret);
		if(ret == 0 && checkneedtolog(newabspath))
			createiologforcreatesymlink(getseqno(),newabspath,oldabspath);
		if(oldabspath)
			kfree(oldabspath);
		if(newabspath)
			kfree(newabspath);
		return ret;  

}

// hijack for create hark link
int (rtb_vfs_link)(struct dentry *old_dentry, struct inode *dir, struct dentry *new_dentry)
{
		int(*run)(struct dentry *,struct inode *, struct dentry *) = sym_vfs_link.run;
		int ret;
		char *oldabspath = NULL;
		char *newabspath = NULL;
		oldabspath = kmalloc(PATH_MAX, GFP_KERNEL);
		newabspath = kmalloc(PATH_MAX, GFP_KERNEL);
		if(oldabspath == NULL || newabspath == NULL)
		{
			printk("vfs_link.....malloc error\n");
			goto out;
		}
		memset(oldabspath,0,PATH_MAX);
		memset(newabspath,0,PATH_MAX);
		if(dir)
		{
			printk("vfs_link.....\n");
		}
		getabsfullpathfromdentry(old_dentry,oldabspath);
		getabsfullpathfromdentry(new_dentry,newabspath);

		printk("create hardlink from %s to %s\n",oldabspath,newabspath);
	out:
		ret = run(old_dentry,dir,new_dentry);
		if(ret == 0 && checkneedtolog(newabspath))
			createiologforcreatelink(getseqno(),newabspath,oldabspath);
		if(oldabspath)
			kfree(oldabspath);
		if(newabspath)
			kfree(newabspath);
		return ret;  


}

//hijack for mkdir
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
	printk("vfs_mkdir %s.\n",abspath);
    ret = run(dir,dentry,mode);
	printk("vfs_mkdir return %d.\n",ret);
	if(ret == 0 && checkneedtolog(abspath))
		createiologformkdir(getseqno(),abspath, mode);
out:
	if(abspath)
        kfree(abspath);
    return ret;   
}

//hijack for sys_write
long (rtb_sys_write)(unsigned int fd, const char __user *buf, size_t count)
{
    long(*run)(unsigned int, const char __user *, size_t) = sym_sys_write.run;
	long ret;
	loff_t offset = 0;
	struct task_struct *pcurrent = NULL;
	char *abspath = NULL;
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		printk("sys_write.....malloc error\n");
        return run(fd,buf,count);
	}
	memset(abspath,0,PATH_MAX);

	getfilepath(fd,abspath);

	pcurrent = current;
	offset = pcurrent->files->fdt->fd[fd]->f_pos;
	if(checkneedtolog(abspath))
		printk("sys_wirte .... %s. offset is %ld. count is %zu\n",abspath,offset,count);
    ret = run(fd,buf,count);

	if(abspath)
		kfree(abspath);
	
    return ret;
}


//hijack for vfs_write
ssize_t (rtb_vfs_write)(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
    ssize_t(*run)(struct file *, const char __user *, size_t, loff_t *) = sym_vfs_write.run;
	ssize_t ret;
	int iret = 0;
    char *abspath = NULL;
	

	if(file->f_flags == 1)
		return run(file,buf,count,pos);
	
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		return run(file,buf,count,pos);
	}
	memset(abspath,0,PATH_MAX);

	iret = getabsfullpathfromstructfile(file,abspath);

	if(strncmp(abspath,"/var/log/messages",strlen("/var/log/messages")) != 0 && \
			strncmp(abspath,"/ptmx",strlen("/ptmx")) != 0 && \
						strncmp(abspath,"/",strlen("/")) == 0 &&\
						strncmp(abspath,"/dev",strlen("/dev")) != 0
			)
		{
			printk("vfs_write file %s. count is %zu, offset is %lld. f_flags %d\n",abspath,count,*pos,file->f_flags);
			printk("%x\n",run);
    }

	if(checkneedtolog(abspath))
		createiologforwrite(getseqno(),abspath,buf,count,pos);
	ret = run(file,buf,count,pos);
//	if(ret > 0 && checkneedtolog(abspath))
//		createiologforwrite(getseqno(),abspath,buf,count,pos);
	
 out:  
 	if(abspath)
		kfree(abspath);

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
	//{"vfs_write", &sym_do_truncate,(unsigned long *)rtb_do_truncate},
    {"vfs_write", &sym_vfs_write,(unsigned long *)rtb_vfs_write},
    {"vfs_mkdir", &sym_vfs_mkdir, (unsigned long *)rtb_vfs_mkdir},
    {"vfs_rmdir", &sym_vfs_rmdir, (unsigned long *)rtb_vfs_rmdir},
    {"vfs_rename", &sym_vfs_rename, (unsigned long *)rtb_vfs_rename},
    {"vfs_unlink", &sym_vfs_unlink, (unsigned long *)rtb_vfs_unlink},
    {"vfs_symlink", &sym_vfs_symlink, (unsigned long *)rtb_vfs_symlink},
    {"vfs_link", &sym_vfs_link, (unsigned long *)rtb_vfs_link},
    {"vfs_create", &sym_vfs_create, (unsigned long *)rtb_vfs_create},
};

// hijack the needed functions. 
void hijack_syscalls(void) {

	int ret, i;

	for (i = 0; i < ARRAY_SIZE(security2hook); i++) {
		ret = symbol_hijack(security2hook[i].sym, security2hook[i].name, security2hook[i].func);

		if (IN_ERR(ret))
			printfail(security2hook[i].name);
	}

}

//unhijack functions
void undo_hijack_syscalls(void) {
	int i;

	for (i = 0; i < ARRAY_SIZE(security2hook); i++)
		symbol_restore(security2hook[i].sym);
}

