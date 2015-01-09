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
#if(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18))
#include <linux/path.h>
#include <linux/fdtable.h>
#endif
#include <linux/dcache.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/time.h>
#include "monitorset.h"
#include "rtbnetlink.h"

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

extern asmlinkage long sys_mkdir(const char __user *pathname, int mode)
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

struct kernsym sym_sys_mkdir;

long (rtb_sys_mkdir)(const char __user *pathname, int mode)
{
	int(*run)(const char __user *pathname, int mode) = sym_sys_mkdir.run;
	long ret;

	printk("RTB: sys_mkdir %s.\n",pathname);

	return run(pathname,mode);
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
		rtbprintk("do_truncate.....malloc error\n");
        return run(dentry,length,time_attrs,filp);
	}
	memset(abspath,0,PATH_MAX);

//	getabsfullpathfromstructfile(filp,abspath);

//	if(checkneedtolog( abspath))
//	{
//		rtbprintk("do_truncate %s. %lld.\n",abspath,length);
//	}
	if(length < 1000000000)
		rtbprintk("do_truncate.....%lld\n",length);
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
    PMONITOR_FILE_ENTRY pmfe = NULL;
    int ineedtolog = 0;
    ULONGLONG ullseqno = 0;
    ULONGLONG ullGlobalSeqno = 0;
    ULONGLONG timesec = 0;
    char *iologdir  = NULL;

    if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
    {
        return run(old_dir,old_dentry,new_dir,new_dentry);
    }
    
    iologdir = kmalloc(PATH_MAX,GFP_KERNEL);
	oldabspath = kmalloc(PATH_MAX, GFP_KERNEL);
	newabspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(oldabspath == NULL || newabspath == NULL || iologdir == NULL)
	{
		rtbprintk("vfs_rmname.....malloc error\n");
       	goto out;
	}
	memset(oldabspath,0,PATH_MAX);
	memset(newabspath,0,PATH_MAX);
    memset(iologdir,0,PATH_MAX);
    
    
	getabsfullpathfromdentry(new_dentry,newabspath);
    getabsfullpathfromdentry(old_dentry,oldabspath);
    pmfe = GetMonitorFileEntryByabspath(newabspath);
    if(pmfe != NULL)
    {
        ineedtolog = 1;
        
        
    }
    else
    {
        pmfe = GetMonitorFileEntryByabspath(oldabspath);
        if(pmfe != NULL)
        {
            ineedtolog = 1;
        }
    }
    rtbprintk("Rename from %s to %s.\n",oldabspath,newabspath);
    if(ineedtolog)
    {
        GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
        rtbprintk("RTB: Rename seq: %lld. gseq: %lld. time: %lld.\n",ullseqno,ullGlobalSeqno,timesec);
    }


out:
	ret = run(old_dir,old_dentry,new_dir,new_dentry);
	/* move include move in and move out. Don't forget to deal with .   add by lb, 2014-09-24 */ 
	if(ret == 0 && ineedtolog)
		createiologforrename(ullseqno,ullGlobalSeqno,timesec,iologdir,oldabspath,newabspath);
    else if(ineedtolog)
        createiologforerror(ullseqno, ullGlobalSeqno, timesec,iologdir, newabspath);

    
	if(oldabspath)
        kfree(oldabspath);

	if(newabspath)
        kfree(newabspath);

    if(iologdir)
        kfree(iologdir);
    return ret;  


}

//hijack for delete file 
int (rtb_vfs_unlink)(struct inode *dir, struct dentry *dentry)
{
    int(*run)(struct inode *,struct dentry *) = sym_vfs_unlink.run;
	int ret;
    char *abspath = NULL;
    PMONITOR_FILE_ENTRY pmfe = NULL;
    int ineedtolog = 0;
    ULONGLONG ullseqno = 0;
    ULONGLONG ullGlobalSeqno = 0;
    ULONGLONG timesec = 0;
    char *iologdir = NULL;

    if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
    {
        return run(dir,dentry);
    }
    
    iologdir = kmalloc(PATH_MAX, GFP_KERNEL);
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL || iologdir == NULL)
	{
		rtbprintk("vfs_unlink.....malloc error\n");
        return run(dir,dentry);
	}
	memset(abspath,0,PATH_MAX);
    memset(iologdir,0,PATH_MAX);
    if(dir)
    {
        //rtbprintk("vfs_unlink.....\n");
    }
    
    getabsfullpathfromdentry(dentry,abspath);
	//rtbprintk("vfs_unlink %s.\n",abspath);

    
    pmfe = GetMonitorFileEntryByabspath(abspath);
    if(pmfe != NULL)
    {
        ineedtolog = 1;   
        GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
        rtbprintk("RTB: unlink seq: %lld. gseq: %lld. time: %lld.\n",ullseqno,ullGlobalSeqno,timesec);
    }
    ret = run(dir,dentry);
	if(ret == 0 && ineedtolog)
		createiologforrmfile(ullseqno, ullGlobalSeqno, timesec, iologdir,abspath);
    else if(ineedtolog)
        createiologforerror(ullseqno, ullGlobalSeqno, timesec, iologdir,abspath);
	if(abspath)
        kfree(abspath);

    if(iologdir)
        kfree(iologdir);
    return ret;   


}

//hijack for rmdir
int (rtb_vfs_rmdir)(struct inode *dir, struct dentry *dentry)
{
    int(*run)(struct inode *,struct dentry *) = sym_vfs_rmdir.run;
	int ret;
    char *abspath = NULL;
    //char iologdir[PATH_MAX] = {0};
    char *iologdir  = NULL;
    
    PMONITOR_FILE_ENTRY pmfe = NULL;
    int ineedtolog = 0;
    ULONGLONG ullseqno = 0;
    ULONGLONG ullGlobalSeqno = 0;
    ULONGLONG timesec = 0;  
    if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
    {
        return run(dir,dentry);
    }
    iologdir = kmalloc(PATH_MAX, GFP_KERNEL);
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		rtbprintk("vfs_rmdir.....malloc error\n");
        return run(dir,dentry);
	}
	memset(abspath,0,PATH_MAX);
    memset(iologdir,0,PATH_MAX);
    if(dir)
    {
        rtbprintk("vfs_rmdir.....\n");
    }
    
    rtbprintk("dentry->dname %s.\n",dentry->d_name.name);
    rtbprintk("dentry->dparent  %s.\n",dentry->d_parent->d_name.name);
    getabsfullpathfromdentry(dentry,abspath);
	rtbprintk("vfs_rmdir %s.\n",abspath);
     pmfe = GetMonitorFileEntryByabspath(abspath);
    if(pmfe != NULL)
    {
        ineedtolog = 1;
        GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
        rtbprintk("RTB: rmdir seq: %lld. gseq: %lld. time: %lld.\n",ullseqno,ullGlobalSeqno,timesec);
        
    }
    ret = run(dir,dentry);
	rtbprintk("vfs_rmdir return %d.\n",ret);
	if(ret == 0 && ineedtolog)
		createiologforrmdir(ullseqno, ullGlobalSeqno, timesec, iologdir,abspath);
    else if(ineedtolog)
        createiologforerror(ullseqno, ullGlobalSeqno, timesec, iologdir,abspath);
	if(abspath)
        kfree(abspath);

    if(iologdir)
        kfree(iologdir);
    return ret;   


}

//hijack for create file
int (rtb_vfs_create)(struct inode *dir, struct dentry *dentry, int mode,
		struct nameidata *nd)
{
	int(*run)(struct inode *,struct dentry *, int ,struct nameidata *) = sym_vfs_create.run;
	int ret = 0,iret = 0;
    char *abspath = NULL;
    char *iologdir = NULL;
    PMONITOR_FILE_ENTRY pmfe = NULL;
    int ineedtolog = 0;
    ULONGLONG ullseqno = 0;
    ULONGLONG ullGlobalSeqno = 0;
    ULONGLONG timesec = 0;  

/*	char *testbuff = NULL;
	char *pathPtr = NULL;
	int testbuffsize = PATH_MAX;
	*/

    if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
    {
        return run(dir,dentry,mode,nd);
    }
    
	if(dentry->d_inode != NULL)
	{
		rtbprintk("open file\n");
		goto out;
	}
	rtbprintk("create new file\n");
/*
	testbuff = kmalloc(PATH_MAX,GFP_KERNEL);
	memset(testbuff,0,PATH_MAX);
	pathPtr = d_path(nd->dentry, nd->mnt, testbuff, testbuffsize);
	printk("RTB: test: %s.\n",pathPtr);
	printk("RTB: test: %s.\n",testbuff);
	*/

	
    iologdir = kmalloc(PATH_MAX, GFP_KERNEL);
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL || iologdir == NULL)
	{
		rtbprintk("vfs_create.....malloc error\n");
        goto out;
	}
	memset(abspath,0,PATH_MAX);
    memset(iologdir,0,PATH_MAX);
    if(dir)
    {
        rtbprintk("vfs_create.....\n");
    }
    
	iret = getabsfullpathfromdentry(dentry,abspath);
	rtbprintk("vfs_create file %s\n",abspath);

     pmfe = GetMonitorFileEntryByabspath(abspath);
    if(pmfe != NULL)
    {
        ineedtolog = 1;
        
        GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
        rtbprintk("RTB: create seq: %ld. gseq: %ld. time: %ld.\n",ullseqno,ullGlobalSeqno,timesec);
    }

out:
	ret = run(dir,dentry,mode,nd);
	if(ret == 0 && ineedtolog)
		createiologforcreatefile(ullseqno,ullGlobalSeqno,timesec,iologdir,abspath,mode);
    else if(ineedtolog)
        createiologforerror(ullseqno,ullGlobalSeqno,timesec,iologdir,abspath);


    if(abspath)
		kfree(abspath);

    if(iologdir)
        kfree(iologdir);
	return ret;
}

// hijack for create symbolic link
int (rtb_vfs_symlink)(struct inode *dir, struct dentry *dentry, const char *oldname)
{
		int(*run)(struct inode *,struct dentry *, const char *) = sym_vfs_symlink.run;
		int ret;
		char *oldabspath = NULL;
		char *newabspath = NULL;
        char *iologdir = NULL;
        PMONITOR_FILE_ENTRY pmfe = NULL;
        int ineedtolog = 0;
        ULONGLONG ullseqno = 0;
        ULONGLONG ullGlobalSeqno = 0;
        ULONGLONG timesec = 0;  

        if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
        {
            return run(dir,dentry,oldname);
        }
        
        iologdir = kmalloc(PATH_MAX, GFP_KERNEL);
		oldabspath = kmalloc(PATH_MAX, GFP_KERNEL);
		newabspath = kmalloc(PATH_MAX, GFP_KERNEL);
		if(oldabspath == NULL || newabspath == NULL || iologdir == NULL)
		{
			rtbprintk("vfs_symlink.....malloc error\n");
			goto out;
		}
		memset(oldabspath,0,PATH_MAX);
		memset(newabspath,0,PATH_MAX);
        memset(iologdir,0,PATH_MAX);
		if(dir)
		{
			rtbprintk("vfs_symlink.....\n");
		}
		getabsfullpath(oldname,oldabspath);
		getabsfullpathfromdentry(dentry,newabspath);

		rtbprintk("create symlink from %s to %s\n",oldabspath,newabspath);
        pmfe = GetMonitorFileEntryByabspath(newabspath);
        if(pmfe != NULL)
        {
            ineedtolog = 1;
            GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
            rtbprintk("RTB: synlink seq: %lld. gseq: %lld. time: %lld.\n",ullseqno,ullGlobalSeqno,timesec);         
            
        }

 	
	out:
		ret = run(dir,dentry,oldname);
		rtbprintk("create symlink return %d.\n",ret);
		if(ret == 0 && ineedtolog)
			createiologforcreatesymlink(ullseqno,ullGlobalSeqno,timesec,iologdir,newabspath,oldabspath);
        else if(ineedtolog)
            createiologforerror(ullseqno,ullGlobalSeqno,timesec,iologdir,newabspath);
		if(oldabspath)
			kfree(oldabspath);
		if(newabspath)
			kfree(newabspath);
        if(iologdir)
            kfree(iologdir);
		return ret;  

}

// hijack for create hark link
int (rtb_vfs_link)(struct dentry *old_dentry, struct inode *dir, struct dentry *new_dentry)
{
		int(*run)(struct dentry *,struct inode *, struct dentry *) = sym_vfs_link.run;
		int ret;
		char *oldabspath = NULL;
		char *newabspath = NULL;
        PMONITOR_FILE_ENTRY pmfe = NULL;
        int ineedtolog = 0;
        ULONGLONG ullseqno = 0;
        ULONGLONG ullGlobalSeqno = 0;
        ULONGLONG timesec = 0;  
        char *iologdir = NULL;

        if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
        {
            return run(old_dentry,dir,new_dentry);
        }
        
		oldabspath = kmalloc(PATH_MAX, GFP_KERNEL);
		newabspath = kmalloc(PATH_MAX, GFP_KERNEL);
        iologdir = kmalloc(PATH_MAX, GFP_KERNEL);
		if(oldabspath == NULL || newabspath == NULL || iologdir == NULL)
		{
			rtbprintk("vfs_link.....malloc error\n");
			goto out;
		}
		memset(oldabspath,0,PATH_MAX);
		memset(newabspath,0,PATH_MAX);
        memset(iologdir,0,PATH_MAX);
		if(dir)
		{
			rtbprintk("vfs_link.....\n");
		}
		getabsfullpathfromdentry(old_dentry,oldabspath);
		getabsfullpathfromdentry(new_dentry,newabspath);

		rtbprintk("create hardlink from %s to %s\n",oldabspath,newabspath);

        pmfe = GetMonitorFileEntryByabspath(newabspath);
        if(pmfe != NULL)
        {
            ineedtolog = 1;
            GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
            rtbprintk("RTB: hardlink seq: %lld. gseq: %lld. time: %lld.\n",ullseqno,ullGlobalSeqno,timesec);         
            
        }
	out:
		ret = run(old_dentry,dir,new_dentry);
		if(ret == 0 && ineedtolog)
			createiologforcreatelink(ullseqno,ullGlobalSeqno,timesec,iologdir,newabspath,oldabspath);
        else if(ineedtolog)
            createiologforerror(ullseqno,ullGlobalSeqno,timesec,iologdir,newabspath);
		if(oldabspath)
			kfree(oldabspath);
		if(newabspath)
			kfree(newabspath);
        if(iologdir)
            kfree(iologdir);
		return ret;  


}

//hijack for mkdir
int (rtb_vfs_mkdir)(struct inode *dir, struct dentry *dentry, int mode)
{
    int(*run)(struct inode *,struct dentry *, int) = sym_vfs_mkdir.run;
	int ret;
    char *abspath = NULL;
    PMONITOR_FILE_ENTRY pmfe = NULL;
    int ineedtolog = 0;
    ULONGLONG ullseqno = 0;
    ULONGLONG ullGlobalSeqno = 0;
    ULONGLONG timesec = 0;   
    char *iologdir  = NULL;
/*
	struct vfsmount * mnt;
	struct dentry *mydentry;
*/
    if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
    {
        return run(dir,dentry,mode);
    }


    
    iologdir = kmalloc(PATH_MAX, GFP_KERNEL);
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL || iologdir == NULL)
	{
		rtbprintk("vfs_mkdir.....malloc error\n");
        ret = run(dir,dentry,mode);
        goto out;
	}
	memset(abspath,0,PATH_MAX);
    memset(iologdir,0,PATH_MAX);
    if(dir)
    {
        rtbprintk("vfs_mkdir.....\n");
    }

/*
				read_lock(&current->fs->lock);
				if (current->fs->altroot) {
					mnt = mntget(current->fs->altrootmnt);
					mydentry = dget(current->fs->altroot);
					read_unlock(&current->fs->lock);
					printk("RTB: test1...\n");

				}
				mnt = mntget(current->fs->rootmnt);
				mydentry = dget(current->fs->root);
				printk("RTB: test2...\n");
				read_unlock(&current->fs->lock);
			
    rtbprintk("dentry->dname %s.\n",mydentry->d_name.name);
    rtbprintk("dentry->dparent  %s.\n",mydentry->d_parent->d_name.name);
    getabsfullpathfromdentry(mydentry,abspath);		
	rtbprintk("vfs_mkdir %s.\n",abspath);

	memset(abspath,0,PATH_MAX);
*/
	
    rtbprintk("dentry->dname %s.\n",dentry->d_name.name);
    rtbprintk("dentry->dparent  %s.\n",dentry->d_parent->d_name.name);
    getabsfullpathfromdentry(dentry,abspath);		
	rtbprintk("vfs_mkdir %s.\n",abspath);

    pmfe = GetMonitorFileEntryByabspath(abspath);
    if(pmfe != NULL)
    {
        ineedtolog = 1;
        GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
        rtbprintk("RTB: mkdir seq: %lld. gseq: %lld. time: %lld.\n",ullseqno,ullGlobalSeqno,timesec);   
        
    }

    
    ret = run(dir,dentry,mode);
	if(ret == 0 && ineedtolog)
		createiologformkdir(ullseqno,ullGlobalSeqno,timesec,iologdir,abspath, mode);
    else if(ineedtolog)
        createiologforerror(ullseqno,ullGlobalSeqno,timesec,iologdir,abspath);
out:
	if(abspath)
        kfree(abspath);
    if(iologdir)
        kfree(iologdir);
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

    if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
    {
        return run(fd,buf,count);
    }
    
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL)
	{
		rtbprintk("sys_write.....malloc error\n");
        return run(fd,buf,count);
	}
	memset(abspath,0,PATH_MAX);

	getfilepath(fd,abspath);

	pcurrent = current;
	offset = pcurrent->files->fdt->fd[fd]->f_pos;
	if(checkneedtolog(abspath))
		rtbprintk("sys_wirte .... %s. offset is %ld. count is %zu\n",abspath,offset,count);
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
	PMONITOR_FILE_ENTRY pmfe = NULL;
    int ineedtolog = 0;
    loff_t offset = *pos;
    ULONGLONG ullseqno = 0;
    ULONGLONG ullGlobalSeqno = 0;
    ULONGLONG timesec = 0;  
    char *iologdir = NULL;

    if(!FileReplData.Config.bNormalRunning || !FileReplData.Config.bValid)
    {
        return run(file,buf,count,pos);
    }
    
	if(file->f_flags == 1)
		return run(file,buf,count,pos);
	
	abspath = kmalloc(PATH_MAX, GFP_KERNEL);
    iologdir = kmalloc(PATH_MAX, GFP_KERNEL);
	if(abspath == NULL || iologdir == NULL)
	{
		return run(file,buf,count,pos);
	}
	memset(abspath,0,PATH_MAX);
    memset(iologdir,0,PATH_MAX);
	iret = getabsfullpathfromstructfile(file,abspath);

     pmfe = GetMonitorFileEntryByabspath(abspath);
    if(pmfe != NULL)
    {
        ineedtolog = 1;
        GetMonitorFileSeqNoByMonitorFileEntry(pmfe, &ullseqno, &ullGlobalSeqno, &timesec,iologdir);
        rtbprintk("RTB: write seq: %lld. gseq: %lld. time: %lld.\n",ullseqno,ullGlobalSeqno,timesec);     
        rtbprintk("vfs_write file %s. count is %zu, offset is %lld. f_flags %d\n",abspath,count,*pos,file->f_flags);
    }    

	ret = run(file,buf,count,pos);
	if(ret > 0 && ineedtolog)
		iret = createiologforwrite(ullseqno,ullGlobalSeqno,timesec,iologdir,abspath,buf,count,&offset);
    else if(ineedtolog)
        iret = createiologforerror(ullseqno,ullGlobalSeqno,timesec,iologdir,abspath);
	if(iret != 0)
	{
		rtbprintk("RTB: create io log for write error......\n");
		FileReplData.Config.bNormalRunning = false;
		notify_user_status(NOTIFY_TYPE_CLIENT_ERROR,-3);
	}
 out:  
 	if(abspath)
		kfree(abspath);
    if(iologdir)
        kfree(iologdir);


    return ret;
}


void printfail(const char *name) {
	rtbprintk(PKPRE "warning: unable to implement protections for %s\n", name);
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

