/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : getpath.c
 * Author        : lb
 * Date          : 2014-09-19
 * Description   : get absolutely path from dentry or struct file
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-19
 *   Author      : lb
 *   Modification: Created file. Bugs in get file abs path from dentry, missing mount point path.
 *
 * 
 *

*************************************************************************************************************/


#include "module.h"
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


/*****************************************************************************
 * Function      : getfilepath
 * Description   : get abs path from fd
 * Input         : unsigned int fd  
                char *abspath    
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140926
 *   Author      : lb
 *   Modification: Created function
 *   			   
  * 2.Date        : 20141218
 *   Author      : lb
 *   Modification: add support for 2.6.18
*****************************************************************************/
int getfilepath(unsigned int fd, char *abspath)
{
#if(LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18))
	struct dentry *mydentry = current->files->fdt->fd[fd]->f_dentry;
#else
	struct dentry *mydentry = current->files->fdt->fd[fd]->f_path.dentry;
#endif

	
	return getabsfullpathfromdentry(mydentry,abspath);
}

/*****************************************************************************
 * Function      : getabsparentpath
 * Description   : get abs parent path 
 * Input         : const char * pathname  
                char *abspath          
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140919
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int getabsparentpath(const char * pathname,char *abspath)
{
	int ret = 0,depth = 0, i = 0;
	struct nameidata nd;
	struct dentry *polddentry = NULL;
	struct dentry *pdentry = NULL;
	char *ppath = NULL;
	int len = 0;
	
	ret = path_lookup(pathname,LOOKUP_DIRECTORY,&nd);
	rtbprintk("path_lookup return %d.\n",ret);
	if(1)
	{
#if(LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18))
	polddentry = nd.dentry;
	pdentry = nd.dentry->d_parent;
#else
	polddentry = nd.path.dentry;
	pdentry = nd.path.dentry->d_parent;

#endif

	  len = pdentry->d_name.len;
	  ppath = (char *)pdentry->d_name.name;
	  while(pdentry != pdentry->d_parent)
	  {
	  	depth++;
		pdentry = pdentry->d_parent;
		len = pdentry->d_name.len;
		ppath = (char *)pdentry->d_name.name;
	  }

	  if(depth < 1)
	  {
	  	strncat(abspath, ppath, len);
	  }	
	  else
	  {
	  	while(depth > 0)
	  	{
	  	  pdentry = polddentry;
		  for(i=0; i<depth; i++)
		  {
		  	pdentry = pdentry->d_parent;
		  }
		  len = pdentry->d_name.len;
		  ppath = (char *)pdentry->d_name.name;
		  strncat(abspath, "/", 2);
		  strncat(abspath, ppath, len);
		  depth--;
	  	}
	  }
	}
	//rtbprintk("parent abs path %s \n",abspath);
	return ret;

}

/*****************************************************************************
 * Function      : getabsparentpathfromdentry
 * Description   : get parent abs path from dentry
 * Input         : struct dentry *dentry  
                char *abspath          
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140919
 *   Author      : lb
 *   Modification: Created function. Missing mount point path.

*****************************************************************************/
int getabsparentpathfromdentry(struct dentry *dentry,char *abspath)
{
	int ret = 0,depth = 0, i = 0;
	struct dentry *polddentry = NULL;
	struct dentry *pdentry = NULL;
	char *ppath = NULL;
	int len = 0;
	polddentry = dentry;
	if(polddentry)
	{
	  pdentry = polddentry->d_parent;
	  len = pdentry->d_name.len;
	  ppath = (char *)pdentry->d_name.name;
	  while(pdentry != pdentry->d_parent)
	  {
	  	depth++;
		pdentry = pdentry->d_parent;
		len = pdentry->d_name.len;
		ppath = (char *)pdentry->d_name.name;
	  }

	  if(depth < 1)
	  {
	  	strncat(abspath, ppath, len);
	  }	
	  else
	  {
	  	while(depth > 0)
	  	{
	  	  pdentry = polddentry;
		  for(i=0; i<depth; i++)
		  {
		  	pdentry = pdentry->d_parent;
		  }
		  len = pdentry->d_name.len;
		  ppath = (char *)pdentry->d_name.name;
		  strncat(abspath, "/", 2);
		  strncat(abspath, ppath, len);
		  depth--;
	  	}
	  }
	}
	//rtbprintk("parent abs path %s \n",abspath);
	return ret;

}

/*****************************************************************************
 * Function      : getabsfullpathfromdentry
 * Description   : get full abs path from dentry
 * Input         : struct dentry *dentry  
                char *abspath          
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140923
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int getabsfullpathfromdentry(struct dentry *dentry, char *abspath)
{
	int ret = 0;
    if(dentry)
    {
    	if(strncmp(dentry->d_parent->d_name.name,"/",strlen("/")) != 0)
        {
            ret = getabsparentpathfromdentry(dentry,abspath);
        }
		strncat(abspath, "/", strlen("/"));
        strncat(abspath, dentry->d_name.name, dentry->d_name.len);
        //rtbprintk("Full abs path %s \n",abspath);
    }

	return 0;
}

/*****************************************************************************
 * Function      : getabsfullpath
 * Description   : get abs full path
 * Input         : const char *pathname  
                char *abspath         
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140919
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int getabsfullpath(const char *pathname, char *abspath)
{
	int ret = 0;
	if(strncmp(pathname,"/",strlen("/")) == 0)
	{
	  strncpy(abspath,pathname,strlen(pathname));
	}
	else
	{
	  ret = getabsparentpath(pathname,abspath);
	  strncat(abspath, "/", 2);
      strncat(abspath, pathname, strlen(pathname));
	}
	//rtbprintk("Full abs path %s \n",abspath);
	return 0;
}


/*****************************************************************************
 * Function      : getabsfullpathfromstructfile
 * Description   : get abs full path from struct file
 * Input         : struct file * file  
                char *abspath       
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140919
 *   Author      : lb
 *   Modification: Created function
 *
 * 2.Date		 : 20140923
 *	 Author		 : lb
 *   Modification: copy from 
 * 		http://stackoverflow.com/questions/8250078/how-can-i-get-a-filename-from-a-file-descriptor-inside-a-kernel-module
 *      Modify to support kernel < 2.6.25.
 * 

*****************************************************************************/
int getabsfullpathfromstructfile(struct file * file, char *abspath)
{
	int ret = 0;
	
	char *tmp = NULL;
	char *pathname;
	
#if(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18))
	struct path path;	
#endif

	if(file == NULL )
	{
			return ret;
	}
#if(LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18))
	tmp = (char *)__get_free_page(0);
	if(!tmp){
		return -ENOMEM;;
	}
	pathname = get_d_path(file, tmp, PATH_MAX);
	strcat(abspath,pathname);
	if(tmp)
		free_page((unsigned long)tmp);
#else

	path = file->f_path;
	path_get(&file->f_path);
	tmp = (char *)__get_free_page(GFP_TEMPORARY);
	if(!tmp){
		path_put(&path);
		return -ENOMEM;;
	}

	pathname = get_d_path(file, tmp, PATH_MAX);


	path_put(&path);
	strcat(abspath,pathname);
	if(tmp)
		free_page((unsigned long)tmp);
	
#endif


	return ret;
	
}

/**
 * Get abs path name from nameidata
 * @param  nd     nameidata *
 * @param  dentry dentry for file
 * @param  abspath buffer to store abspath
 * @return         0 for success
 */
int getabsfullpathfromnd(struct nameidata *nd,struct dentry *dentry,char * abspath)
{
	int ret = 0;
	char *tmp = NULL;
	char *pathname;
	
#if(LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18))
	struct path path;	
#endif

	if(nd == NULL || dentry == NULL )
	{
			return ret;
	}

	tmp = (char *)__get_free_page(0);
	if(!tmp){
		return -ENOMEM;;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25)
	pathname = d_path(nd->dentry,nd->mnt, tmp, PATH_MAX);
	strcat(abspath,pathname);
	if(tmp)
		free_page((unsigned long)tmp);
	strncat(abspath, "/", strlen("/"));
	strncat(abspath, dentry->d_name.name, dentry->d_name.len);
#else
	pathname = d_path(&nd->path, tmp, PATH_MAX);
	strcat(abspath,pathname);
	if(tmp)
		free_page((unsigned long)tmp);
	strncat(abspath, "/", strlen("/"));
	strncat(abspath, dentry->d_name.name, dentry->d_name.len);
#endif


	return ret;
	
} 
