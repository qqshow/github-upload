/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : AnalysisIolog.c
 * Author        : lb
 * Date          : 2014-09-25
 * Description   : Analysis Io Logs.
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-10-29
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/
#include "AnalysisIolog.h"
//#include "../fruk.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>





#define NONE		 "\033[m" 
#define RED 		 "\033[0;32;31m" 
#define LIGHT_RED	 "\033[1;31m" 
#define GREEN		 "\033[0;32;32m" 
#define LIGHT_GREEN  "\033[1;32m" 
#define BLUE		 "\033[0;32;34m" 
#define LIGHT_BLUE	 "\033[1;34m" 
#define DARY_GRAY	 "\033[1;30m" 
#define CYAN		 "\033[0;36m" 
#define LIGHT_CYAN	 "\033[1;36m" 
#define PURPLE		 "\033[0;35m" 
#define LIGHT_PURPLE "\033[1;35m" 
#define BROWN		 "\033[0;33m" 
#define YELLOW		 "\033[1;33m" 
#define LIGHT_GRAY	 "\033[0;37m" 
#define WHITE		 "\033[1;37m"



/**
 * return parent dir string
 * p MUST not end in a /
 */
int
dir_parent(char *p,char *parentdir)
{

	char *n;
	char *copy;

	if (!p)
		return -1;

	if (p[0] == '/' && p[1] == '\0')
	{
        strcpy(parentdir,p);
        return 0;
	}

    copy = malloc(strlen(p));
	strncpy(copy,p,strlen(p));
	n = strrchr(copy, '/');
	if (n) {
		*(n+1) = '\0';
		strcpy(parentdir,copy);

		*n= '/';
        if(copy)
            free(copy);
		return 0;
	}
    if(copy)
        free(copy);
	return -1;
}


/*
 * do the action like "mkdir(1) -p"
 */
static int
mkdir_p(char * path)
{
    char * ptr = NULL;
    int i, len, ret;

    len = strlen(path);
    if (NULL == (ptr = malloc(len + 2))) {
        return (-1);
    }
    strcpy(ptr, path);

    /* 
     * convert "a/b/c/d" to "a/b/c/d/"
     */
    if (ptr[len - 1] != '/') {
        ptr[len] = '/'; ptr[len + 1] = '\0';
    }

    /*
     * create dir recursively
     */
    for (i = 1; i < len + 1; i++) {
        if (ptr[i] == '/') {
            ptr[i] = '\0';
            ret = mkdir(ptr, 0755);
            if (ret < 0 && errno != EEXIST) {
                free(ptr);
                return (-1);
            }
            ptr[i] = '/';
        }
    }

    free(ptr);
    return (0);
}

//dump LOG_FILE info
int restore(LOG_FILE *iologfile,char *targetdir)
{
    int ret = 0;
    int fhandle = 0;
    char targetpath[1024] = {0};
    char targetoldpath[1024] = {0};
    strcat(targetpath,targetdir);
    strcat(targetpath,iologfile->hdr.wszFilePath);
    strcat(targetoldpath,targetdir);
	printf("-------------------------------------------------\n");
	switch(iologfile->hdr.ulType)
	{
		case LOG_FILE_TYPE_DELETEFILE:
			printf(RED"%lld\n", iologfile->hdr.ullSeqNo);
			printf(RED"DELETE FILE %s\n",iologfile->hdr.wszFilePath);
            if(iologfile->hdr.bIsDir)
                rmdir(targetpath);
            else
                unlink(targetpath);
                
			break;
		case LOG_FILE_TYPE_ENDOFFILE:
			printf(LIGHT_RED"%lld\n", iologfile->hdr.ullSeqNo);
			printf(LIGHT_RED"DELETE FILE %s\n",iologfile->hdr.wszFilePath);
			break;
		case LOG_FILE_TYPE_HARDLINK:
			printf(BLUE"%lld\n", iologfile->hdr.ullSeqNo);
			printf(BLUE"CREATE HARDLINK %s > %s\n",iologfile->hdr.wszFilePath,iologfile->Data);
            strcat(targetoldpath,iologfile->Data);
            link(targetoldpath,targetpath);
			break;
		case LOG_FILE_TYPE_MOVEIN:
			printf(CYAN"%lld\n", iologfile->hdr.ullSeqNo);
			printf(CYAN"MOVEIN %s > \n",iologfile->hdr.wszFilePath);
			break;
		case LOG_FILE_TYPE_NEWFILE:
			printf(PURPLE"%lld\n", iologfile->hdr.ullSeqNo);
			printf(PURPLE"CREATE NEW FILE %s > \n",iologfile->hdr.wszFilePath);
            char *dirparent = malloc(1024);
            dir_parent(targetpath, dirparent);
            mkdir_p(dirparent);
            if(dirparent)
                free(dirparent);
            fhandle= open(targetpath, O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            if(fhandle)
                close(fhandle);
			break;
		case LOG_FILE_TYPE_RENAMEFILE:
			printf(YELLOW"%lld\n", iologfile->hdr.ullSeqNo);
			printf(YELLOW"RENAME %s  > %s \n",iologfile->hdr.wszFilePath,iologfile->Data);
            strcat(targetoldpath,iologfile->Data);
            rename(targetoldpath,targetpath);
			break;
		case LOG_FILE_TYPE_SOFTLINK:
			printf(BLUE"%lld\n", iologfile->hdr.ullSeqNo);
			printf(BLUE"CREATE SOFTLINK %s > %s\n",iologfile->hdr.wszFilePath, iologfile->Data);
            strcat(targetoldpath,iologfile->Data);
            symlink(targetoldpath,targetpath);
			break;
		case LOG_FILE_TYPE_WRITE:
			printf(GREEN"%lld\n", iologfile->hdr.ullSeqNo);
            printf(GREEN"%lld\n", iologfile->hdr.liSystemTime.QuadPart);
			printf(GREEN"Write %s > count %d. offset %lld\n ",iologfile->hdr.wszFilePath,\
				iologfile->hdr.write.Length,iologfile->hdr.write.ByteOffset);
            fhandle= open(targetpath, O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
            if(fhandle)
            {
                lseek(fhandle,iologfile->hdr.write.ByteOffset.QuadPart,SEEK_SET);
                write(fhandle,iologfile->Data,iologfile->hdr.write.Length);
                close(fhandle);
            }
            
			break;
		case LOG_FILE_TYPE_STOP_INIT:
			printf(BROWN"%lld\n", iologfile->hdr.ullSeqNo);
			printf(BROWN"******************STOPINIT************************");
			break;
	}
	return 0;

}

int main(int argc, char* argv[])
{
	LOG_FILE_HEADER iologfileheader = {0};
	PLOG_FILE piologfile = NULL;
	FILE *fp = NULL;
	char iologpath[1024] = {0};
    char targetdir[1024] = {0};
	int readlen = 0;
	int iolognum = 0;
	int loop = 0;
	int bfirst = 1;
	if(argc > 5 || argc < 4 )
	{
		printf(RED"./RestoreIolog /iologsdir/ /restoredir/ iobeginnum [-loop]\n");
		return 0;
	}
	// loop print io log info, don't exit app
	if(argc == 5 && (strncmp("-loop", argv[4] , sizeof("-loop")) == 0))
		loop = 1;
	printf("Io logs dir is %s.\n",argv[1]);
	printf("begin num is %s.\n",argv[3]);
	iolognum = atoi(argv[3]);
	memset(iologpath,0,1024);

	sprintf(iologpath, "%s%d", argv[1],iolognum);
    sprintf(targetdir,"%s",argv[2]);

	while(loop || bfirst)
	{
		fp = fopen(iologpath,"rw");
		while(fp != NULL)
		{	
			iolognum++;
			readlen = fread(&iologfileheader,sizeof(char),sizeof(LOG_FILE_HEADER),fp);
			if(readlen != sizeof(LOG_FILE_HEADER))
				break;
			piologfile = malloc(iologfileheader.ulLogSize);
			if(piologfile == NULL)
				break;
			memset(piologfile,0,iologfileheader.ulLogSize);

			fseek(fp,0,SEEK_SET);
			readlen = fread(piologfile,sizeof(char),iologfileheader.ulLogSize,fp);
			if(readlen != iologfileheader.ulLogSize)
				break;
			
			restore(piologfile,targetdir);
			fclose(fp);
			if(piologfile)
				free(piologfile);
			
			memset(iologpath,0,1024);
			sprintf(iologpath, "%s%d", argv[1],iolognum);
			fp = fopen(iologpath,"rw");
		}
		bfirst = 0;
		sleep(1);
	}

    return 0;
}
