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
	filep = file_open(iologpath,O_WRONLY | O_CREAT,777);
	if (IS_ERR(filep))	
	{
        printk("RTB: createiolog error\n");
        return ret;
	}    
	if (file_write(filep, 0, (char *)iologfile, iologfile->hdr.ulLogSize) != iologfile->hdr.ulLogSize)
		goto out;
	
	ret = 0;

	if(filep)
		file_close(filep);
out:
		return ret;
}


int createiologformkdir(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *abspath,int mode)
{
	int iret = -1;
	char iologpath[NAME_MAX] = {0};
	PLOG_FILE iologfile = NULL;
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

	snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullSeqNo);

    printk("RTB: creatiologformkdir iologpath %s. strlen %d.\n",iologpath,strlen(iologpath));

	iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
	if(iret != 0)
		printk("create io log error.\n");
	
out:	
	if(iologfile)
		kfree(iologfile);

	
	return iret;
}

int createiologforcreatefile(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir, char *abspath,int mode)
{

		int iret = -1;
		char iologpath[NAME_MAX] = {0};
		PLOG_FILE iologfile = NULL;
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
	
		iologfile->hdr.ulLogSize = sizeof(LOG_FILE);
	
		strncpy(iologfile->hdr.wszFilePath, abspath, len);
	
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullSeqNo);
	
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");
		
	out:	
		if(iologfile)
			kfree(iologfile);
	
		
		return iret;


}


int createiologforrmdir(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath)
{
		int iret = -1;
		char iologpath[NAME_MAX] = {0};
		PLOG_FILE iologfile = NULL;
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
	
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullSeqNo);
	
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");

		
	out:	
		if(iologfile)
			kfree(iologfile);
		
		return iret;


}

int createiologforrmfile(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath)
{
		int iret = -1;
		char iologpath[NAME_MAX] = {0};
		PLOG_FILE iologfile = NULL;
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
	
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullSeqNo);
	
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");
		
	out:	
		if(iologfile)
			kfree(iologfile);
		
		return iret;


}


int createiologforerror(ulonglong ullSeqNo,ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath)
{
    	int iret = -1;
		char iologpath[NAME_MAX] = {0};
		PLOG_FILE iologfile = NULL;
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
	
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullSeqNo);
	
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");
		
	out:	
		if(iologfile)
			kfree(iologfile);
	
		
		return iret;
}

int createiologforrename(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *oldabspath, char *newabspath)
{
		int iret = -1;
		char iologpath[NAME_MAX] = {0};
		PLOG_FILE iologfile = NULL;
		int oldlen = strlen(oldabspath);
		int newlen = strlen(newabspath);
		int iologfilelen = 0;
		
		if(oldlen > 260)
			oldlen = 260;
		if(newlen > 260)
			newlen = 260;

		iologfilelen = sizeof(LOG_FILE) + newlen - 1;
		
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
	
		snprintf(iologpath, NAME_MAX, "%s%lld",iologdir,ullSeqNo);
	
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");
		
	out:	
		if(iologfile)
			kfree(iologfile);

		
		return iret;


}

int createiologforcreatesymlink(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *symlinkabspath, char *dstabspath)
{
	int iret = -1;
	char iologpath[NAME_MAX] = {0};
	PLOG_FILE iologfile = NULL;
	int oldlen = strlen(dstabspath);
	int newlen = strlen(symlinkabspath);
	int iologfilelen = 0;
	
	if(oldlen > 260)
		oldlen = 260;
	if(newlen > 260)
		newlen = 260;

	iologfilelen = sizeof(LOG_FILE) + newlen - 1;
	
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

	snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullSeqNo);

	iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
	if(iret != 0)
		printk("create io log error.\n");
	
out:	
	if(iologfile)
		kfree(iologfile);

	
	return iret;


}

int createiologforcreatelink(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,\
    char *linkabspath, char *dstabspath)
{
		int iret = -1;
		char iologpath[NAME_MAX] = {0};
		PLOG_FILE iologfile = NULL;
		int oldlen = strlen(dstabspath);
		int newlen = strlen(linkabspath);
		int iologfilelen = 0;
		
		if(oldlen > 260)
			oldlen = 260;
		if(newlen > 260)
			newlen = 260;
	
		iologfilelen = sizeof(LOG_FILE) + newlen - 1;
		
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
	
		snprintf(iologpath, NAME_MAX, "%s%lld",iologdir,ullSeqNo);
	
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");
		
	out:	
		if(iologfile)
			kfree(iologfile);
	
		
		return iret;

}

int createiologforwrite(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,\
    char *abspath,char *writebuff, size_t count, loff_t *pos)
{

		int iret = -1;
		char iologpath[NAME_MAX] = {0};
		PLOG_FILE iologfile = NULL;
		int len = strlen(abspath);
		int iologfilelen = sizeof(LOG_FILE) + count - 1;
		if(len > 260)
			len = 260;
	
		iologfile = kmalloc(iologfilelen, GFP_KERNEL);
		if(iologfile == NULL)
		{
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
	
		snprintf(iologpath, NAME_MAX, "%s%lld", iologdir,ullSeqNo);
	
		iret = createiolog(iologpath,iologfile,iologfile->hdr.ulLogSize);
		if(iret != 0)
			printk("create io log error.\n");
		
	out:	
		if(iologfile)
			kfree(iologfile);
	
		
		return iret;


}



