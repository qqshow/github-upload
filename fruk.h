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
#define FILEREPL_READ_BUFFER_SIZE   1024*8      //不能小于1024*8
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
#define LOG_FILE_TYPE_STOP_INIT     2  // 初始化停止标志
#define LOG_FILE_TYPE_ENDOFFILE     3  // 重设文件大小
#define LOG_FILE_TYPE_NEWFILE       4  // 新建文件，可能是覆盖原文件，也可能是替换原文件
#define LOG_FILE_TYPE_DELETEFILE    5  // 删除文件
#define LOG_FILE_TYPE_RENAMEFILE    6  // 文件改名
#define LOG_FILE_TYPE_MOVEIN        7  //监控外到监控内
#define LOG_FILE_TYPE_HARDLINK      8  // 创建硬链接
#define LOG_FILE_TYPE_SOFTLINK	    9  // 创建软链接
#define LOG_FILE_TYPE_ERROR         10 // 失败

//
// IO日志文件头，该头结构固定长度LOG_FILE_HEADER_SIZE字节，头结构后是IO的文件数据
//
typedef struct _LOG_HEAD_ENDOFFILE_
{
    LARGE_INTEGER EndOfFile;   
}LOG_HEAD_ENDOFFILE;

typedef struct _LOG_FILE_HEADER_
{
    ULONG       ulHeaderSize;   // 头大小
    ULONG       ulType;         // 类型，以区别是正常写的IO,表明初始化终止，文件改名...
    ULONGLONG   ullSeqNo;       // 顺序号
	ULONGLONG   ullGolbalSeqNo;  // 全局顺序号
    ULONG       ulLogSize;       // 整个日志文件的大小
    ULONG       bIsDir;          // 是否目录
	WCHAR	    wszFilePath[FILEREPL_MAX_PATH];  // 监控文件名

    LARGE_INTEGER   liSystemTime;   // io的时间,UTC

    union
    {
        struct
        {
            ULONG Length;              // Length of transfer,文件数据的大小
            LARGE_INTEGER ByteOffset;  // Offset to write to,数据在文件中的位置

        } write;

        LOG_HEAD_ENDOFFILE endoffile;

        ULONG       ExtraData[1];        
    };

} LOG_FILE_HEADER, *PLOG_FILE_HEADER;

typedef struct _LOG_FILE_
{
    LOG_FILE_HEADER hdr;
    char            Reserved[LOG_FILE_HEADER_SIZE-sizeof(LOG_FILE_HEADER)];


    ULONG       Data[1];        //附加数据,比如改名的信息...  

} LOG_FILE, *PLOG_FILE;








//
// 不管是文件、目录或是备份集的配置项，都有着相同的头结构
//
typedef struct _MONITOR_ENTRY_HEAD_
{
    struct rb_node  rbnode;
    GUID        guidSetId;          // 备份集实例唯一标识
}MONITOR_ENTRY_HEAD,*PMONITOR_ENTRY_HEAD;

//
// 一个备份集有唯一GUID标识，有唯一备份io日志保存目录
//
typedef struct _MONITOR_SET_ENTRY_
{
	MONITOR_ENTRY_HEAD  hdr;          //必须是第一个成员

    char       wcsSetCacheDir[FILEREPL_MAX_PATH];  // 文件缓存日志目录，必须以NULL结尾 \??\C:\BAKDIR\C\DIR\CACHE
	ULONGLONG   ullSetSeqNo;        // 备份集的IO日志文件顺序号
	ULONG       ulFileCount;        // 该备份集下文件个数

} MONITOR_SET_ENTRY, *PMONITOR_SET_ENTRY;



//过滤项数据定义
typedef struct _MONITOR_FILE_ENTRY_
{
    MONITOR_ENTRY_HEAD  hdr;          //必须是第一个成员

    BOOL            bInited;
    BOOL            bMonitor;
    ULONGLONG       ullSeqNo;
	
	///////////////////////////////////////////////
	//过滤项是否目录[区分文件或者目录两种类型]
    BOOL            bDirectory;     
	ULONG           ulLength; //该过滤项长度
    ///////////////////////////////////////////////

	char	        wcsMonitorFile[FILEREPL_MAX_PATH];  //必须以NULL结尾    C:\DIR\FILE.EXT
    MONITOR_SET_ENTRY  *pSetEntry;      // 备份集数据指针，为了访问快捷，该值需要初始化，不能从保存的配置数据读取

} MONITOR_FILE_ENTRY, *PMONITOR_FILE_ENTRY;

//搜索、配置查找项数据定义，配置队列和搜索树存储相同的数据结构指针
typedef struct _MONITOR_ITEM_DATA_
{
	struct list_head              listHead;
	ULONG                   ulCounts;  // 包含的子项总数
	struct kmem_cache *  ItemLookAsideList; //配置数据队列
	struct rb_root           RBTree;           //搜索树
    ULONG                   ulSize;     // 结构的大小
} MONITOR_ITEM_DATA, *PMONITOR_ITEM_DATA;


//备份集、文件[目录]过滤搜索、配置数据使用相同的结构
typedef MONITOR_ITEM_DATA MONITOR_SET_DATA,*PMONITOR_SET_DATA;
typedef MONITOR_ITEM_DATA MONITOR_FILE_DATA,*PMONITOR_FILE_DATA;

typedef struct _FILEREPL_CONFIG
{
    BOOL                    bMonitorFilesLoaded;
    WCHAR	                wcsConfigDir[FILEREPL_MAX_PATH];  // 配置文件存放目录
    MONITOR_FILE_DATA       MonitorFiles;       //文件

	/////////////////////////////////////////////////////////////
	//过滤项全部放在MonitorFiles中，不区分文件和目录
    //MONITOR_FILE_DATA       MonitorDirs;        //目录
    /////////////////////////////////////////////////////////////

	MONITOR_SET_DATA        MonitorSet;         //备份集
    //NPAGED_LOOKASIDE_LIST   RBTreeLookAsideList;

    //ERESOURCE	            SyncResource;
    BOOL                    bValid;             // 标志该驱动是否监控
    BOOL                    bNormalRunning;     // 标志该驱动是否正常运行
	ULONG					dwLastNormalShutdown; //标志系统上次关机状态

    BOOL                    bDirty;             //用于定时保存配置数据时判断配置数据是否改变
    struct list_head        iowritequeue;       //异步io日志写队列
    spinlock_t              iowritequeuelock;
	spinlock_t				gseqnolock;
    
} FILEREPL_CONFIG, *PFILEREPL_CONFIG;

//
// 配置文件的文件头，大小不能超过FILEREPL_READ_BUFFER_SIZE
// #define FILEREPL_READ_BUFFER_SIZE   1024*8
//
#define DEFAULT_CONFIG_FILE_HEADER_SIZE FILEREPL_READ_BUFFER_SIZE
typedef struct _CONFIG_FILE_HEADER_
{
    ULONG       ulHeaderSize;       //配置文件头的大小，固定为FILEREPL_READ_BUFFER_SIZE

    ULONG       ulOffsetSet;        //备份集在配置文件中的偏移
    ULONG       ulSizeSet;          //备份集在配置文件中的大小

    ULONG       ulOffsetFiles;        //文件项在配置文件中的偏移
    ULONG       ulSizeFiles;          //文件项在配置文件中的大小

	ULONG		ulLastShutdownStatus; //系统上次关机状态

//////////////////////////////////////////////////////////////////////////
    //ULONG       ulOffsetDirs;        //目录项在配置文件中的偏移
    //ULONG       ulSizeDirs;          //目录项在配置文件中的大小
//////////////////////////////////////////////////////////////////////////

}CONFIG_FILE_HEADER,*PCONFIG_FILE_HEADER;


typedef struct _FILEREPL_DATA {


	FILEREPL_CONFIG	Config;


    ULONG           ulConfigRefreshFlag;  // 如果配置更新了，则该值+1


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