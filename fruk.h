/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : fruk.h
 * Author        : lb
 * Date          : 2014-09-23
 * Description   : Io log defintion 
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-23
 *   Author      : lb
 *   Modification: Created file. Copy from windows real time backup module. Changed some difintion.

*************************************************************************************************************/

/*++

Copyright (c) 1999-2002  Microsoft Corporation

Module Name:

    fruk.h

Abstract:

    Header file which contains the structures, type definitions,
    constants, global variables and function prototypes that are
    shared between kernel and user mode.

Environment:

    Kernel & user mode

--*/

#ifndef _FILEREPLUK_H
#define _FILEREPLUK_H

#include <linux/list.h>
#include <linux/rbtree.h>
#include "module.h"


#pragma pack(1)

//
//  Name of port used to communicate
//

#define FileReplPortName L"\\FileReplPort"

#ifndef MAX_PATH
#if defined(__linux__)
#define MAX_PATH	260
#else
#define MAX_PATH	260
#endif

#endif

#define MIN_SECTOR_SIZE         	512
#define FILEREPL_READ_BUFFER_SIZE   1024*8      //����С��1024*8
#define FILEREPL_MAX_PATH       	MAX_PATH
#define BOOL int
#define ALIGN_SIZE(s,a) \
            (s) += (a) - 1;\
            (s) /= (a);\
            (s) *= (a);




#define RET_SUCCESS         	0
#define RET_UNKONOW_NOTIFY_TYPE	0xF0000001
#define RET_ERROR				0xF0000002

typedef struct _FILEREPL_REPLY 
{
    ULONG RetVal;
    
} FILEREPL_REPLY, *PFILEREPL_REPLY;

// #define LOG_FILE_TMP_EXT            L"TMP"
#define LOG_FILE_EXT                L"LOG"

#define LOG_FILE_HEADER_SIZE        1024

#define LOG_FILE_TYPE_WRITE         1
#define LOG_FILE_TYPE_STOP_INIT     2  // ��ʼ��ֹͣ��־
#define LOG_FILE_TYPE_ENDOFFILE     3  // �����ļ���С
#define LOG_FILE_TYPE_NEWFILE       4  // �½��ļ��������Ǹ���ԭ�ļ���Ҳ�������滻ԭ�ļ�
#define LOG_FILE_TYPE_DELETEFILE    5  // ɾ���ļ�
#define LOG_FILE_TYPE_RENAMEFILE    6  // �ļ�����
#define LOG_FILE_TYPE_MOVEIN        7  //����⵽�����
#define LOG_FILE_TYPE_HARDLINK      8  // ����Ӳ����
#define LOG_FILE_TYPE_SOFTLINK	    9  // ����������
#define LOG_FILE_TYPE_ERROR         10 // ʧ��

//
// IO��־�ļ�ͷ����ͷ�ṹ�̶�����LOG_FILE_HEADER_SIZE�ֽڣ�ͷ�ṹ����IO���ļ�����
//
typedef struct _LOG_HEAD_ENDOFFILE_
{
    LARGE_INTEGER EndOfFile;   
}LOG_HEAD_ENDOFFILE;

typedef struct _LOG_FILE_HEADER_
{
    ULONG       ulHeaderSize;   // ͷ��С
    ULONG       ulType;         // ���ͣ�������������д��IO,������ʼ����ֹ���ļ�����...
    ULONGLONG   ullSeqNo;       // ˳���
	ULONGLONG   ullGolbalSeqNo;  // ȫ��˳���
    ULONG       ulLogSize;       // ������־�ļ��Ĵ�С
    ULONG       bIsDir;          // �Ƿ�Ŀ¼
	WCHAR	    wszFilePath[FILEREPL_MAX_PATH];  // ����ļ���

    LARGE_INTEGER   liSystemTime;   // io��ʱ��,UTC

    union
    {
        struct
        {
            ULONG Length;              // Length of transfer,�ļ����ݵĴ�С
            LARGE_INTEGER ByteOffset;  // Offset to write to,�������ļ��е�λ��

        } write;

        LOG_HEAD_ENDOFFILE endoffile;

        ULONG       ExtraData[1];        
    };

} LOG_FILE_HEADER, *PLOG_FILE_HEADER;

typedef struct _LOG_FILE_
{
    LOG_FILE_HEADER hdr;
    char            Reserved[LOG_FILE_HEADER_SIZE-sizeof(LOG_FILE_HEADER)];


    ULONG       Data[1];        //��������,�����������Ϣ...  

} LOG_FILE, *PLOG_FILE;








//
// �������ļ���Ŀ¼���Ǳ��ݼ����������������ͬ��ͷ�ṹ
//
typedef struct _MONITOR_ENTRY_HEAD_
{
    struct rb_node  rbnode;
    GUID        guidSetId;          // ���ݼ�ʵ��Ψһ��ʶ
}MONITOR_ENTRY_HEAD,*PMONITOR_ENTRY_HEAD;

//
// һ�����ݼ���ΨһGUID��ʶ����Ψһ����io��־����Ŀ¼
//
typedef struct _MONITOR_SET_ENTRY_
{
	MONITOR_ENTRY_HEAD  hdr;          //�����ǵ�һ����Ա

    char       wcsSetCacheDir[FILEREPL_MAX_PATH];  // �ļ�������־Ŀ¼��������NULL��β \??\C:\BAKDIR\C\DIR\CACHE
	ULONGLONG   ullSetSeqNo;        // ���ݼ���IO��־�ļ�˳���
	ULONG       ulFileCount;        // �ñ��ݼ����ļ�����

} MONITOR_SET_ENTRY, *PMONITOR_SET_ENTRY;



//���������ݶ���
typedef struct _MONITOR_FILE_ENTRY_
{
    MONITOR_ENTRY_HEAD  hdr;          //�����ǵ�һ����Ա

    BOOL            bInited;
    BOOL            bMonitor;
    ULONGLONG       ullSeqNo;
	
	///////////////////////////////////////////////
	//�������Ƿ�Ŀ¼[�����ļ�����Ŀ¼��������]
    BOOL            bDirectory;     
	ULONG           ulLength; //�ù������
    ///////////////////////////////////////////////

	char	        wcsMonitorFile[FILEREPL_MAX_PATH];  //������NULL��β    C:\DIR\FILE.EXT
    MONITOR_SET_ENTRY  *pSetEntry;      // ���ݼ�����ָ�룬Ϊ�˷��ʿ�ݣ���ֵ��Ҫ��ʼ�������ܴӱ�����������ݶ�ȡ

} MONITOR_FILE_ENTRY, *PMONITOR_FILE_ENTRY;

//���������ò��������ݶ��壬���ö��к��������洢��ͬ�����ݽṹָ��
typedef struct _MONITOR_ITEM_DATA_
{
	struct list_head              listHead;
	ULONG                   ulCounts;  // ��������������
	struct kmem_cache *  ItemLookAsideList; //�������ݶ���
	struct rb_root           RBTree;           //������
    ULONG                   ulSize;     // �ṹ�Ĵ�С
} MONITOR_ITEM_DATA, *PMONITOR_ITEM_DATA;


//���ݼ����ļ�[Ŀ¼]������������������ʹ����ͬ�Ľṹ
typedef MONITOR_ITEM_DATA MONITOR_SET_DATA,*PMONITOR_SET_DATA;
typedef MONITOR_ITEM_DATA MONITOR_FILE_DATA,*PMONITOR_FILE_DATA;

typedef struct _FILEREPL_CONFIG
{
    BOOL                    bMonitorFilesLoaded;
    WCHAR	                wcsConfigDir[FILEREPL_MAX_PATH];  // �����ļ����Ŀ¼
    MONITOR_FILE_DATA       MonitorFiles;       //�ļ�

	/////////////////////////////////////////////////////////////
	//������ȫ������MonitorFiles�У��������ļ���Ŀ¼
    //MONITOR_FILE_DATA       MonitorDirs;        //Ŀ¼
    /////////////////////////////////////////////////////////////

	MONITOR_SET_DATA        MonitorSet;         //���ݼ�
    //NPAGED_LOOKASIDE_LIST   RBTreeLookAsideList;

    //ERESOURCE	            SyncResource;
    BOOL                    bValid;             // ��־�������Ƿ���
    BOOL                    bNormalRunning;     // ��־�������Ƿ���������
	ULONG					dwLastNormalShutdown; //��־ϵͳ�ϴιػ�״̬

    BOOL                    bDirty;             //���ڶ�ʱ������������ʱ�ж����������Ƿ�ı�
    struct list_head        iowritequeue;       //�첽io��־д����
    spinlock_t              iowritequeuelock;
	spinlock_t				gseqnolock;
    
} FILEREPL_CONFIG, *PFILEREPL_CONFIG;

//
// �����ļ����ļ�ͷ����С���ܳ���FILEREPL_READ_BUFFER_SIZE
// #define FILEREPL_READ_BUFFER_SIZE   1024*8
//
#define DEFAULT_CONFIG_FILE_HEADER_SIZE FILEREPL_READ_BUFFER_SIZE
typedef struct _CONFIG_FILE_HEADER_
{
    ULONG       ulHeaderSize;       //�����ļ�ͷ�Ĵ�С���̶�ΪFILEREPL_READ_BUFFER_SIZE

    ULONG       ulOffsetSet;        //���ݼ��������ļ��е�ƫ��
    ULONG       ulSizeSet;          //���ݼ��������ļ��еĴ�С

    ULONG       ulOffsetFiles;        //�ļ����������ļ��е�ƫ��
    ULONG       ulSizeFiles;          //�ļ����������ļ��еĴ�С

	ULONG		ulLastShutdownStatus; //ϵͳ�ϴιػ�״̬

//////////////////////////////////////////////////////////////////////////
    //ULONG       ulOffsetDirs;        //Ŀ¼���������ļ��е�ƫ��
    //ULONG       ulSizeDirs;          //Ŀ¼���������ļ��еĴ�С
//////////////////////////////////////////////////////////////////////////

}CONFIG_FILE_HEADER,*PCONFIG_FILE_HEADER;


typedef struct _FILEREPL_DATA {


	FILEREPL_CONFIG	Config;


    ULONG           ulConfigRefreshFlag;  // ������ø����ˣ����ֵ+1


    BOOL            bStopTask;
    BOOL            bTaskStoped;

    BOOL            bInsertTasking;

	BOOL			bInsertWriteTasking;

	BOOL			bWaittingTask;
	BOOL			bWaittingWriteTask;


	long			MaxIoToWrite;
	ULONG			WriteBuffLen;
	long			WriteIoCached;
	long			WriteIoAsynchronous;
	long			WriteIo;
	long			nneedtosavemonitor;
	ULONG			dbgprintlevel;
	ULONGLONG		ullIoBuffSize;           //
	ULONGLONG		ullIoCounts;
    ULONG           nlmsg_pid;               //net link communicate port
} FILEREPL_DATA, *PFILEREPL_DATA;

extern FILEREPL_DATA FileReplData;




typedef struct _IOWRITE_CONTEXT_
{
    struct list_head entry;
    char *iologpath;
    PLOG_FILE logfile;
} IOWRITE_CONTEXT, *PIOWRITE_CONTEXT;















#pragma pack()

#endif //  _FILEREPLUK_H