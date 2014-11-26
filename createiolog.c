/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : createiolog.c
 * Author        : lb
 * Date          : 2014-09-24
 * Description   : .C file. Write io log info to files.
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-24
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/
#include "createiolog.h"
#include "fruk.h"
#include "iowritethread.h"
#include "monitorset.h"
#include "rtbnetlink.h"


#define ASYNC_IOWRITE


ulonglong gseqno = 0;


ulonglong getseqno(void)
{
	ulonglong seqno = gseqno++;
	return seqno;

}


int createiolog(char *iologpath, PLOG_FILE iologfile, int iologfilesize)
{
	int ret = -1;
	struct file *filep = NULL;
	//filep = file_open(iologpath, O_WRONLY | O_CREAT, 777);     
	printk("RTB: createiolog %s.\n",iologpath);
	filep = file_open(iologpath,O_WRONLY | O_CREAT,777);
	if (IS_ERR(filep))	
	{
        printk("RTB: createiolog error\n");
		notify_user_status(NOTIFY_TYPE_CLIENT_ERROR,(long)filep);
        return ret;
	}    
	if (file_write(filep, 0, (char *)iologfile, iologfile->hdr.ulLogSize) != iologfile->hdr.ulLogSize)
		goto out;
	
	ret = 0;

	if(filep)
		file_close(filep);

    if(ret == 0)
        SaveConfig();
out:
	return ret;
}


int createiologformkdir(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *abspath,int mode)
{
	int iret = -1;
	char *iologpath = NULL;
	PLOG_FILE iologfile = NULL;
    PIOWRITE_CONTEXT piowc = NULL;
	int len = strlen(abspath);
	if(len > 260)
		len = 260;

	iologfile = kmalloc(sizeof(LOG_FILE), GFP_KERNEL);
	if(iologfile == NULL)
	{
	  iret = -1;
	  return iret;
	}
	memset(iologfile,0,sizeof(LOG_FILE));

	iologfile->hdr.ulType = LOG_FILE_TYPE_NEWFILE;
	iologfile->hdr.bIsDir = 1;
	iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
	iologfile->hdr.ullSeqNo = ullSeqNo;
    iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
    iologfile->hdr.liSystemTime.QuadPart = timesecs;

	iologfile->hdr.ulLogSize = sizeof(LOG_FILE);

	strncpy(iologfile->hdr.wszFilePath, abspath, len);
    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
	snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullGlobalSeqNo);

    printk("RTB: creatiologformkdir iologpath %s. strlen %d.\n",iologpath,strlen(iologpath));

#ifdef ASYNC_IOWRITE
    piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
    piowc->logfile = iologfile;
    piowc->iologpath = iologpath;
    spin_lock( &FileReplData.Config.iowritequeuelock);
    list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
    spin_unlock( &FileReplData.Config.iowritequeuelock);
    completecompletion();
    return 0;
#else
    iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
    if(iret != 0)
    	printk("create io log error.\n");

#endif
	
out:	
	if(iologfile)
		kfree(iologfile);
    if(iologpath)
        kfree(iologpath);

	
	return iret;
}

int createiologforcreatefile(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir, char *abspath,int mode)
{

		int iret = -1;
		char *iologpath = NULL;
		PLOG_FILE iologfile = NULL;
        PIOWRITE_CONTEXT piowc = NULL;
		int len = strlen(abspath);
		if(len > 260)
			len = 260;
	
		iologfile = kmalloc(sizeof(LOG_FILE), GFP_KERNEL);
		if(iologfile == NULL)
		{
		  iret = -1;
		  return iret;
		}
		memset(iologfile,0,sizeof(LOG_FILE));
	
		iologfile->hdr.ulType = LOG_FILE_TYPE_NEWFILE;
		iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
		iologfile->hdr.ullSeqNo = ullSeqNo;
        iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
        iologfile->hdr.liSystemTime.QuadPart = timesecs;
		//printk("RTB: timesec is %ld.\n",timesecs);
	
		iologfile->hdr.ulLogSize = sizeof(LOG_FILE);
	
		strncpy(iologfile->hdr.wszFilePath, abspath, len);
	    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullGlobalSeqNo);
	
#ifdef ASYNC_IOWRITE
        piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
        piowc->logfile = iologfile;
        piowc->iologpath = iologpath;
        spin_lock( &FileReplData.Config.iowritequeuelock);
        list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
        spin_unlock( &FileReplData.Config.iowritequeuelock);
        completecompletion();
        return 0;
#else
        iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
    	if(iret != 0)
    		printk("create io log error.\n");

#endif
		
	out:	
		if(iologfile)
			kfree(iologfile);
        if(iologpath)
            kfree(iologpath);
	
		
		return iret;


}


int createiologforrmdir(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath)
{
		int iret = -1;
		char *iologpath = NULL;
		PLOG_FILE iologfile = NULL;
        PIOWRITE_CONTEXT piowc = NULL;
		int len = strlen(abspath);


		
		if(len > 260)
			len = 260;
	
		iologfile = kmalloc(sizeof(LOG_FILE), GFP_KERNEL);
		if(iologfile == NULL)
		{
		  iret = -1;
		  return iret;
		}
		memset(iologfile,0,sizeof(LOG_FILE));
	
		iologfile->hdr.ulType = LOG_FILE_TYPE_DELETEFILE;
		iologfile->hdr.bIsDir = 1;
		iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
		iologfile->hdr.ullSeqNo = ullSeqNo;
        iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
        iologfile->hdr.liSystemTime.QuadPart = timesecs;
	
		iologfile->hdr.ulLogSize = sizeof(LOG_FILE);
	
		strncpy(iologfile->hdr.wszFilePath, abspath, len);
	    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullGlobalSeqNo);
	
#ifdef ASYNC_IOWRITE
        piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
        piowc->logfile = iologfile;
        piowc->iologpath = iologpath;
        spin_lock( &FileReplData.Config.iowritequeuelock);
        list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
        spin_unlock( &FileReplData.Config.iowritequeuelock);
        completecompletion();
        return 0;
#else
        iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
    	if(iret != 0)
    		printk("create io log error.\n");

#endif

		
	out:	
		if(iologfile)
			kfree(iologfile);

        if(iologpath)
            kfree(iologpath);
		
		return iret;


}

int createiologforrmfile(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath)
{
		int iret = -1;
		char *iologpath = NULL;
		PLOG_FILE iologfile = NULL;
        PIOWRITE_CONTEXT piowc = NULL;
		int len = strlen(abspath);
		
		if(len > 260)
			len = 260;
	
		iologfile = kmalloc(sizeof(LOG_FILE), GFP_KERNEL);
		if(iologfile == NULL)
		{
		  iret = -1;
		  return iret;
		}
		memset(iologfile,0,sizeof(LOG_FILE));
	
		iologfile->hdr.ulType = LOG_FILE_TYPE_DELETEFILE;
		iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
		iologfile->hdr.ullSeqNo = ullSeqNo;
        iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
        iologfile->hdr.liSystemTime.QuadPart = timesecs;
	
		iologfile->hdr.ulLogSize = sizeof(LOG_FILE);
	
		strncpy(iologfile->hdr.wszFilePath, abspath, len);
	    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullGlobalSeqNo);
	
#ifdef ASYNC_IOWRITE
        piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
        piowc->logfile = iologfile;
        piowc->iologpath = iologpath;
        spin_lock( &FileReplData.Config.iowritequeuelock);
        list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
        spin_unlock( &FileReplData.Config.iowritequeuelock);
        completecompletion();
        return 0;
#else
        iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
    	if(iret != 0)
    		printk("create io log error.\n");

#endif
		
	out:	
		if(iologfile)
			kfree(iologfile);

        if(iologpath)
            kfree(iologpath);
		
		return iret;


}


int createiologforerror(ulonglong ullSeqNo,ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath)
{
    	int iret = -1;
		char *iologpath= NULL;
		PLOG_FILE iologfile = NULL;
        PIOWRITE_CONTEXT piowc = NULL;
		int len = strlen(abspath);
		if(len > 260)
			len = 260;
	
		iologfile = kmalloc(sizeof(LOG_FILE), GFP_KERNEL);
		if(iologfile == NULL)
		{
		  iret = -1;
		  return iret;
		}
		memset(iologfile,0,sizeof(LOG_FILE));
	
		iologfile->hdr.ulType = LOG_FILE_TYPE_ERROR;
		iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
		iologfile->hdr.ullSeqNo = ullSeqNo;
        iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
        iologfile->hdr.liSystemTime.QuadPart = timesecs;
	
		iologfile->hdr.ulLogSize = sizeof(LOG_FILE);
	
		strncpy(iologfile->hdr.wszFilePath, abspath, len);
	    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullGlobalSeqNo);
	
#ifdef ASYNC_IOWRITE
        piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
        piowc->logfile = iologfile;
        piowc->iologpath = iologpath;
        spin_lock( &FileReplData.Config.iowritequeuelock);
        list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
        spin_unlock( &FileReplData.Config.iowritequeuelock);
        completecompletion();
        return 0;
#else
        iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
    	if(iret != 0)
    		printk("create io log error.\n");

#endif
		
	out:	
		if(iologfile)
			kfree(iologfile);

        if(iologpath)
            kfree(iologpath);
	
		
		return iret;
}

int createiologforrename(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *oldabspath, char *newabspath)
{
		int iret = -1;
		char *iologpath = NULL;
		PLOG_FILE iologfile = NULL;
        PIOWRITE_CONTEXT piowc = NULL;
		int oldlen = strlen(oldabspath);
		int newlen = strlen(newabspath);
		int iologfilelen = 0;
		
		if(oldlen > 260)
			oldlen = 260;
		if(newlen > 260)
			newlen = 260;

		iologfilelen = sizeof(LOG_FILE) + newlen - 4;
		
		iologfile = kmalloc(iologfilelen, GFP_KERNEL);
		if(iologfile == NULL)
		{
		  iret = -1;
		  return iret;
		}
		memset(iologfile,0,iologfilelen);
	
		iologfile->hdr.ulType = LOG_FILE_TYPE_RENAMEFILE;
		iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
		iologfile->hdr.ullSeqNo = ullSeqNo;
        iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
	    iologfile->hdr.liSystemTime.QuadPart = timesecs;
		iologfile->hdr.ulLogSize = iologfilelen;
	
		strncpy(iologfile->hdr.wszFilePath, oldabspath, oldlen);
		strncpy(iologfile->Data, newabspath, newlen);
	    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
		snprintf(iologpath, NAME_MAX, "%s%lld",iologdir,ullGlobalSeqNo);
	
#ifdef ASYNC_IOWRITE
        piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
        piowc->logfile = iologfile;
        piowc->iologpath = iologpath;
        spin_lock( &FileReplData.Config.iowritequeuelock);
        list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
        spin_unlock( &FileReplData.Config.iowritequeuelock);
        completecompletion();
        return 0;
#else
        iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
    	if(iret != 0)
    		printk("create io log error.\n");

#endif
		
	out:	
		if(iologfile)
			kfree(iologfile);
        if(iologpath)
            kfree(iologpath);

		
		return iret;


}

int createiologforcreatesymlink(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *symlinkabspath, char *dstabspath)
{
	int iret = -1;
	char *iologpath = NULL;
	PLOG_FILE iologfile = NULL;
    PIOWRITE_CONTEXT piowc = NULL;
	int oldlen = strlen(dstabspath);
	int newlen = strlen(symlinkabspath);
	int iologfilelen = 0;
	
	if(oldlen > 260)
		oldlen = 260;
	if(newlen > 260)
		newlen = 260;

	iologfilelen = sizeof(LOG_FILE) + newlen - 4;
	
	iologfile = kmalloc(iologfilelen, GFP_KERNEL);
	if(iologfile == NULL)
	{
	  iret = -1;
	  return iret;
	}
	memset(iologfile,0,iologfilelen);

	iologfile->hdr.ulType = LOG_FILE_TYPE_SOFTLINK;
	iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
	iologfile->hdr.ullSeqNo = ullSeqNo;
    iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
    iologfile->hdr.liSystemTime.QuadPart = timesecs;

	iologfile->hdr.ulLogSize = iologfilelen;

	strncpy(iologfile->hdr.wszFilePath, dstabspath, oldlen);
	strncpy(iologfile->Data, symlinkabspath, newlen);
    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
	snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullGlobalSeqNo);
    
#ifdef ASYNC_IOWRITE
    piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
    piowc->logfile = iologfile;
    piowc->iologpath = iologpath;
    spin_lock( &FileReplData.Config.iowritequeuelock);
    list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
    spin_unlock( &FileReplData.Config.iowritequeuelock);
    completecompletion();
    return 0;
#else
    iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
	if(iret != 0)
		printk("create io log error.\n");

#endif
	
out:	
	if(iologfile)
		kfree(iologfile);
    if(iologpath)
        kfree(iologpath);

	
	return iret;


}

int createiologforcreatelink(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,\
    char *linkabspath, char *dstabspath)
{
		int iret = -1;
        PIOWRITE_CONTEXT piowc = NULL;
		char *iologpath = NULL;
		PLOG_FILE iologfile = NULL;
		int oldlen = strlen(dstabspath);
		int newlen = strlen(linkabspath);
		int iologfilelen = 0;
		
		if(oldlen > 260)
			oldlen = 260;
		if(newlen > 260)
			newlen = 260;
	
		iologfilelen = sizeof(LOG_FILE) + newlen - 4;
		
		iologfile = kmalloc(iologfilelen, GFP_KERNEL);
		if(iologfile == NULL)
		{
		  iret = -1;
		  return iret;
		}
		memset(iologfile,0,iologfilelen);
	
		iologfile->hdr.ulType = LOG_FILE_TYPE_HARDLINK;
		iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
    	iologfile->hdr.ullSeqNo = ullSeqNo;
        iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
        iologfile->hdr.liSystemTime.QuadPart = timesecs;
	
		iologfile->hdr.ulLogSize = iologfilelen;
	
		strncpy(iologfile->hdr.wszFilePath, dstabspath, oldlen);
		strncpy(iologfile->Data, linkabspath, newlen);
	    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
		snprintf(iologpath, NAME_MAX, "%s%lld",iologdir,ullGlobalSeqNo);
#ifdef ASYNC_IOWRITE
        piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
        piowc->logfile = iologfile;
        piowc->iologpath = iologpath;
        spin_lock( &FileReplData.Config.iowritequeuelock);
        list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
        spin_unlock( &FileReplData.Config.iowritequeuelock);
        completecompletion();
        return 0;
#else
        iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");

#endif

		
	out:	
		if(iologfile)
			kfree(iologfile);
        if(iologpath)
            kfree(iologpath);
	
		
		return iret;

}

int createiologforwrite(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,\
    char *abspath,char *writebuff, size_t count, loff_t *pos)
{

		int iret = -1;
		char *iologpath = NULL;
		PLOG_FILE iologfile = NULL;
        PIOWRITE_CONTEXT piowc = NULL;
		int len = strlen(abspath);
		int iologfilelen = sizeof(LOG_FILE) + count - 4;
		if(len > 260)
			len = 260;
	
		//iologfile = kmalloc(iologfilelen, GFP_KERNEL);
		iologfile = vmalloc(iologfilelen);
		if(iologfile == NULL)
		{
		  printk("RTB: kmalloc error in createiologforwrite.\n");
		  iret = -1;
		  return iret;
		}
		memset(iologfile,0,iologfilelen);
	
		iologfile->hdr.ulType = LOG_FILE_TYPE_WRITE;
		iologfile->hdr.ulHeaderSize = sizeof(LOG_FILE_HEADER);
		iologfile->hdr.ullSeqNo = ullSeqNo;
        iologfile->hdr.ullGolbalSeqNo = ullGlobalSeqNo;
        iologfile->hdr.liSystemTime.QuadPart = timesecs;
		iologfile->hdr.write.Length = count;
        iologfile->hdr.ulLogSize = iologfilelen;
		iologfile->hdr.write.ByteOffset.QuadPart = *pos;
        memcpy(iologfile->Data,writebuff,count);
		

		strncpy(iologfile->hdr.wszFilePath, abspath, len);
	    iologpath = kzalloc(NAME_MAX,GFP_KERNEL);
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullGlobalSeqNo);

#ifdef ASYNC_IOWRITE
        piowc = kzalloc(sizeof(IOWRITE_CONTEXT), GFP_KERNEL);
        piowc->logfile = iologfile;
        piowc->iologpath = iologpath;
        spin_lock( &FileReplData.Config.iowritequeuelock);
        list_add_tail(&piowc->entry,&FileReplData.Config.iowritequeue);
        spin_unlock( &FileReplData.Config.iowritequeuelock);
        completecompletion();
        return 0;
#else
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
		{
			printk("create io log error. iologpath is %s.\n",iologpath);
			notify_user_status(NOTIFY_TYPE_CLIENT_ERROR,-2);
		}
#endif		
	out:	
		if(iologfile)
			kfree(iologfile);
	    if(iologpath)
            kfree(iologpath);
		
		return iret;


}



