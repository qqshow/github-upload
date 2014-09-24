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
#define FILEREPL_READ_BUFFER_SIZE   1024*8      //²»ÄÜÐ¡ÓÚ1024*8
#define FILEREPL_MAX_PATH       	MAX_PATH

#define ALIGN_SIZE(s,a) \
            (s) += (a) - 1;\
            (s) /= (a);\
            (s) *= (a);

//
// Notification messages
//

#define NOTIFY_TYPE_USER_TO_DRV_START       0x4000
// #define NOTIFY_TYPE_SETFILTERFILE	    1+NOTIFY_TYPE_USER_TO_DRV_START
//#define NOTIFY_TYPE_ADD_BACKUP_FILE	        2+NOTIFY_TYPE_USER_TO_DRV_START
//#define NOTIFY_TYPE_STOP_INIT_FILE	        3+NOTIFY_TYPE_USER_TO_DRV_START
//#define NOTIFY_TYPE_STOP_BACKUP_FILE	    4+NOTIFY_TYPE_USER_TO_DRV_START
#define NOTIFY_TYPE_QUERY_RUNNING_STATUS    5+NOTIFY_TYPE_USER_TO_DRV_START

//
// Ìí¼Ó±¸·Ý¼¯£¬Èç¹ûÒÑ¾­´æÔÚÏàÍ¬GUIDµÄ±¸·Ý¼¯ÅäÖÃ£¬Ôò»áÉ¾³ý
//
#define NOTIFY_TYPE_ADDSET                  6+NOTIFY_TYPE_USER_TO_DRV_START

// 
// Ìí¼Ó±¸·ÝÏî£¬Èç¹ûÒÑ¾­´æÔÚÏàÍ¬GUIDµÄ±¸·Ý¼¯ÅäÖÃ£¬²»»áÉ¾³ý
//
#define NOTIFY_TYPE_ADDITEM         	    7+NOTIFY_TYPE_USER_TO_DRV_START

//
// É¾³ý±¸·Ý¼¯£¬°üÀ¨±¸·Ý¼¯ÏÂµÄÎÄ¼þÏîºÍÄ¿Â¼Ïî
//
#define NOTIFY_TYPE_DELSET                  8+NOTIFY_TYPE_USER_TO_DRV_START

//
// É¾³ýÎÄ¼þÏîºÍÄ¿Â¼ÏîÅäÖÃ
//
#define NOTIFY_TYPE_DELITEM         	    9+NOTIFY_TYPE_USER_TO_DRV_START

//
// É¾³ýËùÓÐ±¸·Ý¼¯ÉèÖÃ
//
#define NOTIFY_TYPE_DELALL           	    10+NOTIFY_TYPE_USER_TO_DRV_START

//
// ²éÑ¯ÉÏ´Î¹Ø»ú×´Ì¬
//
#define NOTIFY_TYPE_QUERY_LAST_SHUTDOWN_STATUS 11+NOTIFY_TYPE_USER_TO_DRV_START

//
// ÖØÖÃÉÏ´Î¹Ø»ú×´Ì¬
//
#define NOTIFY_TYPE_RESET_LAST_SHUTDOWN_STATUS 12+NOTIFY_TYPE_USER_TO_DRV_START


#define FILTER_TYPE_INVALID     0
#define FILTER_TYPE_FILE        1
#define FILTER_TYPE_DIR         2
#define FILTER_TYPE_SET         3


typedef struct _FILTER_ITEM_
{
    ULONG   ulFileType;   //0ÎÄ¼þ£»1 Ä¿Â¼£» 
    WCHAR   wszFilterName[FILEREPL_MAX_PATH];
}FILTER_ITEM,  *PFILTER_ITEM;

typedef struct _REALTIME_BACKUP_DATA_
{
    ULONG   ulSize;                     //Êý¾ÝµÄ×Ü³¤¶È
    GUID    guidSetId;                  //Ò»¸ö±¸·Ý¼¯µÄID£¨ÊµÀýÒ²¿ÉÈÏÎªÊÇÒ»¸ö¼¯ºÏ£©
    ULONG   ulFilterItemCounts;         //¼à¿ØÏî×ÜÊý
    WCHAR   wszBakCacheDir[FILEREPL_MAX_PATH];  //±¸·ÝÈÕÖ¾ÎÄ¼þ´æ·ÅÄ¿Â¼
    FILTER_ITEM FilterItems[1];  
}REALTIME_BACKUP_DATA;

typedef struct _FILEREPL_NOTIFICATION {

	ULONG Type;             // for quad-word alignement of the Contents structure
	ULONG bNormalRunning;   // driver's running status
	union
	{
        struct  
        {
            REALTIME_BACKUP_DATA     BackupData;

        } AddOrDel;
	};
    
} FILEREPL_NOTIFICATION, *PFILEREPL_NOTIFICATION;

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
#define LOG_FILE_TYPE_STOP_INIT     2  // ³õÊ¼»¯Í£Ö¹±êÖ¾
#define LOG_FILE_TYPE_ENDOFFILE     3  // ÖØÉèÎÄ¼þ´óÐ¡
#define LOG_FILE_TYPE_NEWFILE       4  // ÐÂ½¨ÎÄ¼þ£¬¿ÉÄÜÊÇ¸²¸ÇÔ­ÎÄ¼þ£¬Ò²¿ÉÄÜÊÇÌæ»»Ô­ÎÄ¼þ
#define LOG_FILE_TYPE_DELETEFILE    5  // É¾³ýÎÄ¼þ
#define LOG_FILE_TYPE_RENAMEFILE    6  // ÎÄ¼þ¸ÄÃû
#define LOG_FILE_TYPE_MOVEIN        7  //¼à¿ØÍâµ½¼à¿ØÄÚ
#define LOG_FILE_TYPE_HARDLINK      8  // ´´½¨ÈÓ²Á´½Ó
#define LOG_FILE_TYPE_SOFTLINK	    9  // ´´½¨ÈíÁ´½Ó

//
// IOÈÕÖ¾ÎÄ¼þÍ·£¬¸ÃÍ·½á¹¹¹Ì¶¨³¤¶ÈLOG_FILE_HEADER_SIZE×Ö½Ú£¬Í·½á¹¹ºóÊÇIOµÄÎÄ¼þÊý¾Ý
//
typedef struct _LOG_HEAD_ENDOFFILE_
{
    LARGE_INTEGER EndOfFile;   
}LOG_HEAD_ENDOFFILE;

typedef struct _LOG_FILE_HEADER_
{
    ULONG       ulHeaderSize;   // Í·´óÐ¡
    ULONG       ulType;         // ÀàÐÍ£¬ÒÔÇø±ðÊÇÕý³£Ð´µÄIO,±íÃ÷³õÊ¼»¯ÖÕÖ¹£¬ÎÄ¼þ¸ÄÃû...
    ULONGLONG   ullSeqNo;       // Ë³ÐòºÅ
	ULONGLONG   ullGolbalSeqNo;  // È«¾ÖË³ÐòºÅ
    ULONG       ulLogSize;       // Õû¸öÈÕÖ¾ÎÄ¼þµÄ´óÐ¡
    ULONG       bIsDir;          // ÊÇ·ñÄ¿Â¼
	WCHAR	    wszFilePath[FILEREPL_MAX_PATH];  // ¼à¿ØÎÄ¼þÃû

    LARGE_INTEGER   liSystemTime;   // ioµÄÊ±¼ä,UTC

    union
    {
        struct
        {
            ULONG Length;              // Length of transfer,ÎÄ¼þÊý¾ÝµÄ´óÐ¡
            LARGE_INTEGER ByteOffset;  // Offset to write to,Êý¾ÝÔÚÎÄ¼þÖÐµÄÎ»ÖÃ

        } write;

        LOG_HEAD_ENDOFFILE endoffile;

        ULONG       ExtraData[1];        
    };

} LOG_FILE_HEADER, *PLOG_FILE_HEADER;

typedef struct _LOG_FILE_
{
    LOG_FILE_HEADER hdr;
    char            Reserved[LOG_FILE_HEADER_SIZE-sizeof(LOG_FILE_HEADER)];


    ULONG       Data[1];        //¸½¼ÓÊý¾Ý,±ÈÈç¸ÄÃûµÄÐÅÏ¢...  

} LOG_FILE, *PLOG_FILE;

#pragma pack()

#endif //  _FILEREPLUK_H