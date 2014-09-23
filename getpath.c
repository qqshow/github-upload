#include "module.h"

int getabsparentpath(const char * pathname,char *abspath)
{
	int ret = 0,depth = 0, i = 0;
	struct nameidata nd;
	struct dentry *polddentry = NULL;
	struct dentry *pdentry = NULL;
	char *ppath = NULL;
	int len = 0;
	
	ret = path_lookup(pathname,LOOKUP_DIRECTORY,&nd);
	printk("path_lookup return %d.\n",ret);
	if(1)
	{
	  polddentry = nd.path.dentry;
	  pdentry = nd.path.dentry->d_parent;
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
	//printk("parent abs path %s \n",abspath);
	return ret;

}

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
	//printk("parent abs path %s \n",abspath);
	return ret;

}

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
        //printk("Full abs path %s \n",abspath);
    }

	return 0;
}


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
	//printk("Full abs path %s \n",abspath);
	return 0;
}

// copy and modify from 
//http://stackoverflow.com/questions/8250078/how-can-i-get-a-filename-from-a-file-descriptor-inside-a-kernel-module
//by lb 20140923
int getabsfullpathfromstructfile(struct file * file, char *abspath)
{
	int ret = 0;
	
	char *tmp = NULL;
	char *pathname;
	struct path path;
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

	return ret;
	
}
