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
 * 1.Date        : 2014-09-25
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/
#include "AnalysisIolog.h"
//#include "../fruk.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>




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


//dump LOG_FILE info
int dump(LOG_FILE *iologfile)
{
	printf("-------------------------------------------------\n");
	printf("%lld\n",iologfile->hdr.liSystemTime.QuadPart);
	switch(iologfile->hdr.ulType)
	{
		case LOG_FILE_TYPE_DELETEFILE:
			printf(RED"%lld\n", iologfile->hdr.ullSeqNo);
			printf(RED"DELETE FILE %s\n",iologfile->hdr.wszFilePath);
			break;
		case LOG_FILE_TYPE_ENDOFFILE:
			printf(LIGHT_RED"%lld\n", iologfile->hdr.ullSeqNo);
			printf(LIGHT_RED"DELETE FILE %s\n",iologfile->hdr.wszFilePath);
			break;
		case LOG_FILE_TYPE_HARDLINK:
			printf(BLUE"%lld\n", iologfile->hdr.ullSeqNo);
			printf(BLUE"CREATE HARDLINK %s > %s\n",iologfile->hdr.wszFilePath,iologfile->Data);
			break;
		case LOG_FILE_TYPE_MOVEIN:
			printf(CYAN"%lld\n", iologfile->hdr.ullSeqNo);
			printf(CYAN"MOVEIN %s > \n",iologfile->hdr.wszFilePath);
			break;
		case LOG_FILE_TYPE_NEWFILE:
			printf(PURPLE"%lld\n", iologfile->hdr.ullSeqNo);
			printf(PURPLE"CREATE NEW FILE %s > \n",iologfile->hdr.wszFilePath);
			break;
		case LOG_FILE_TYPE_RENAMEFILE:
			printf(YELLOW"%lld\n", iologfile->hdr.ullSeqNo);
			printf(YELLOW"RENAME %s  > %s \n",iologfile->hdr.wszFilePath,iologfile->Data);
			break;
		case LOG_FILE_TYPE_SOFTLINK:
			printf(BLUE"%lld\n", iologfile->hdr.ullSeqNo);
			printf(BLUE"CREATE SOFTLINK %s > %s\n",iologfile->hdr.wszFilePath, iologfile->Data);
			break;
		case LOG_FILE_TYPE_WRITE:
			printf(GREEN"%lld\n", iologfile->hdr.ullSeqNo);
            printf(GREEN"%lld\n", iologfile->hdr.liSystemTime.QuadPart);
			printf(GREEN"Write %s > count %d. offset %lld\n ",iologfile->hdr.wszFilePath,\
				iologfile->hdr.write.Length,iologfile->hdr.write.ByteOffset);
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
	int readlen = 0;
	int iolognum = 0;
	int loop = 0;
	int bfirst = 1;
	if(argc > 4 || argc < 3 )
	{
		printf(RED"./AnalysisIolog /iologsdri/ iobeginnum [-loop]\n");
		return 0;
	}
	// loop print io log info, don't exit app
	if(argc == 4 && (strncmp("-loop", argv[3] , sizeof("-loop")) == 0))
		loop = 1;
	printf("Io logs dir is %s.\n",argv[1]);
	printf("begin num is %s.\n",argv[2]);
	iolognum = atoi(argv[2]);
	memset(iologpath,0,1024);

	sprintf(iologpath, "%s%d", argv[1],iolognum);


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
			
			dump(piologfile);
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



