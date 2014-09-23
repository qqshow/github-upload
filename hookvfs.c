#include "module.h"

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

struct kernsym sym_sys_mkdir;
struct kernsym sym_sys_write;

struct kernsym sym_vfs_mkdir;
struct kernsym sym_vfs_write;
struct kernsym sym_vfs_create;
struct kernsym sym_vfs_symlink;
struct kernsym sym_vfs_link;
struct kernsym sym_vfs_rmdir;
struct kernsym sym_vfs_unlink;
struct kernsym sym_vfs_rename;


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
	
	if(oldabspath)
        kfree(oldabspath);

	if(newabspath)
        kfree(newabspath);
    return ret;  


}

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
	if(abspath)
        kfree(abspath);
    return ret;   


}

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
    ret = run(dir,dentry);
	if(abspath)
        kfree(abspath);
    return ret;   


}

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
	return ret;
}

// create symbolic link
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
		if(oldabspath)
			kfree(oldabspath);
		if(newabspath)
			kfree(newabspath);
		return ret;  

}

// create hark link
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
			printk("vfs_symlink.....malloc error\n");
			goto out;
		}
		memset(oldabspath,0,PATH_MAX);
		memset(newabspath,0,PATH_MAX);
		if(dir)
		{
			printk("vfs_symlink.....\n");
		}
		getabsfullpathfromdentry(old_dentry,oldabspath);
		getabsfullpathfromdentry(new_dentry,newabspath);

		printk("create hardlink from %s to %s\n",oldabspath,newabspath);
	out:
		ret = run(old_dentry,dir,new_dentry);
		if(oldabspath)
			kfree(oldabspath);
		if(newabspath)
			kfree(newabspath);
		return ret;  


}

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
	if(abspath)
        kfree(abspath);
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
	int iret = 0;
    char *abspath = NULL;
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		goto out;
	}
	memset(abspath,0,PATH_MAX);

	iret = getabsfullpathfromstructfile(file,abspath);

	if(strncmp(abspath,"/var/log/messages",strlen("/var/log/messages")) != 0 && \
			strncmp(abspath,"/ptmx",strlen("/ptmx")) != 0 && \
						strncmp(abspath,"/",strlen("/")) == 0 &&\
						strncmp(abspath,"/dev",strlen("/dev")) != 0
			)
		{
			printk("vfs_write file %s. count is %zu, offset is %lld.\n",abspath,count,*pos);
    }

	
 out:  
 	if(abspath)
		kfree(abspath);
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
    {"vfs_write", &sym_vfs_write,(unsigned long *)rtb_vfs_write},
    {"vfs_mkdir", &sym_vfs_mkdir, (unsigned long *)rtb_vfs_mkdir},
    //{"sys_mkdir", &sym_sys_mkdir, (unsigned long *)rtb_sys_mkdir},
    {"vfs_rmdir", &sym_vfs_rmdir, (unsigned long *)rtb_vfs_rmdir},
    {"vfs_rename", &sym_vfs_rename, (unsigned long *)rtb_vfs_rename},
    {"vfs_unlink", &sym_vfs_unlink, (unsigned long *)rtb_vfs_unlink},
    {"vfs_symlink", &sym_vfs_symlink, (unsigned long *)rtb_vfs_symlink},
    {"vfs_link", &sym_vfs_link, (unsigned long *)rtb_vfs_link},
    {"vfs_create", &sym_vfs_create, (unsigned long *)rtb_vfs_create},
};

// hijack the needed functions. whenever possible

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

