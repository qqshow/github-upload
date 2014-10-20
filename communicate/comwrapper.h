/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : comwrapper.h
 * Author        : lb
 * Date          : 2014-09-29
 * Description   : header file for communicate with rtb kernel mode
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-29
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/

#ifndef _COMWRAPPER_H_
#define _COMWRAPPER_H_
#include <linux/types.h>
#include <stdint.h>

#if defined(__linux__)
    #define ULONG uint32_t
    #define ULONGLONG uint64_t
	#define ulonglong uint64_t
    #define WCHAR char 

    typedef struct _GUID {
        uint32_t  Data1;
        uint16_t Data2;
        uint16_t Data3;
        unsigned char  Data4[ 8 ];
    } GUID;

    #if defined(MIDL_PASS)
    typedef struct _LARGE_INTEGER {
    #else // MIDL_PASS
    typedef union _LARGE_INTEGER {
        struct {
            uint32_t LowPart;
            uint32_t HighPart;
        };
        struct {
            uint32_t LowPart;
            uint32_t HighPart;
        } u;
    #endif //MIDL_PASS
        uint64_t QuadPart;
    } LARGE_INTEGER;
#endif


typedef enum
{
	systeminit,// init state (only used in kernel module)
	systemrunning,//last shutdown state is normal
	systemshutdown,// shutdown state ( used in kernel module)
	systemexceptionreboot//last shutdown state is exception
}
enumShutdownState; 

typedef enum
{
	modulerunning,
	moduleerror
}
enumModuleRunningState; 



#define FILEREPL_MAX_PATH 260


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
// 添加备份集，如果已经存在相同GUID的备份集配置，则会删除
//
#define NOTIFY_TYPE_ADDSET                  6+NOTIFY_TYPE_USER_TO_DRV_START

// 
// 添加备份项，如果已经存在相同GUID的备份集配置，不会删除
//
#define NOTIFY_TYPE_ADDITEM         	    7+NOTIFY_TYPE_USER_TO_DRV_START

//
// 删除备份集，包括备份集下的文件项和目录项
//
#define NOTIFY_TYPE_DELSET                  8+NOTIFY_TYPE_USER_TO_DRV_START

//
// 删除文件项和目录项配置
//
#define NOTIFY_TYPE_DELITEM         	    9+NOTIFY_TYPE_USER_TO_DRV_START

//
// 删除所有备份集设置
//
#define NOTIFY_TYPE_DELALL           	    10+NOTIFY_TYPE_USER_TO_DRV_START

//
// 查询上次关机状态
//
#define NOTIFY_TYPE_QUERY_LAST_SHUTDOWN_STATUS 11+NOTIFY_TYPE_USER_TO_DRV_START

//
// 重置上次关机状态
//
#define NOTIFY_TYPE_RESET_LAST_SHUTDOWN_STATUS 12+NOTIFY_TYPE_USER_TO_DRV_START



#define FILTER_TYPE_INVALID     0
#define FILTER_TYPE_FILE        1
#define FILTER_TYPE_DIR         2
#define FILTER_TYPE_SET         3

typedef struct _FILTER_ITEM_
{
    ULONG   ulFileType;   //0文件；1 目录； 
    WCHAR   wszFilterName[FILEREPL_MAX_PATH];
}FILTER_ITEM,  *PFILTER_ITEM;

typedef struct _REALTIME_BACKUP_DATA_
{
    ULONG   ulSize;                     //数据的总长度
    GUID    guidSetId;                  //一个备份集的ID（实例也可认为是一个集合）
    ULONG   ulFilterItemCounts;         //监控项总数
    char   wszBakCacheDir[FILEREPL_MAX_PATH];  //备份日志文件存放目录
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


/*****************************************************************************
 * Function      : InitCommunicate
 * Description   : Init communicate socket and start msg recv thread
 * Input          : None
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int InitCommunicate();

/*****************************************************************************
 * Function      : QueryDriverStatus
 * Description   : Query driver running status.
 * Input          : None
 * Output        : None
 * Return        : 0 for success.
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int QueryDriverStatus();

/*****************************************************************************
 * Function      : QueryLastShutDownStatus
 * Description   : Query last shutdown status
 * Input          : None
 * Output        : None
 * Return        : >=0 for success.
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int QueryLastShutDownStatus();

/*****************************************************************************
 * Function      : ResetLastShutDownStatus
 * Description   : Reset last shutdown status to normal 
 * Input          : None
 * Output        : None
 * Return        : Success return systemrunning or systemexceptionreboot
 *                 . -1 for error.
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int ResetLastShutDownStatus();

/*****************************************************************************
 * Function      : AddBackupSet
 * Description   : Add backup set
 * Input         : REALTIME_BACKUP_DATA * pBackupData  
 * Output        : None
 * Return        : Success return modulerunning or moduleerror. -1 for error.
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int AddBackupSet (REALTIME_BACKUP_DATA * pBackupData);

/*****************************************************************************
 * Function      : AddBackupItems
 * Description   : Add backup items.
 * Input         : REALTIME_BACKUP_DATA * pBackupData  
 * Output        : None
 * Return        : >= 0 for success
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int AddBackupItems(REALTIME_BACKUP_DATA * pBackupData);

/*****************************************************************************
 * Function      : DelBackupSet
 * Description   : Delete one backup set
 * Input         : REALTIME_BACKUP_DATA * pBackupData  
 * Output        : None
 * Return        : >= 0 for success
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int DelBackupSet(REALTIME_BACKUP_DATA * pBackupData); 

/*****************************************************************************
 * Function      : DelBackupItems
 * Description   : Delete backup items
 * Input         : REALTIME_BACKUP_DATA * pBackupData  
 * Output        : None
 * Return        : >= 0 for success
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int DelBackupItems(REALTIME_BACKUP_DATA * pBackupData);

/*****************************************************************************
 * Function      : DelAllBackup
 * Description   : Delete all backup sets
 * Input          : None
 * Output        : None
 * Return        : >= 0 for success
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int DelAllBackup();

/*****************************************************************************
 * Function      : UnInitCommunicate
 * Description   : Uninitalize communicate socket.
 * Input          : None
 * Output        : None
 * Return        : >= 0 for success
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int UnInitCommunicate();

/*****************************************************************************
 * Function      : RegisterUserAppNotify
 * Description   : Register user app notify callback.\n Used to notify kernel
                   mode information to user app.
 * Input         : int (*callback)(int  
                int)      First para: notify type. Second Para:notify status.     
 * Output        : None
 * Return        : >= 0 for success
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
void RegisterUserAppNotify(int (*callback)(int,int));


/*****************************************************************************
 * Function      : SendConfig
 * Description   : Send backup info to kernel mode
 * Input         : REALTIME_BACKUP_DATA *pBackupData  
                ULONG ulType                       
 * Output        : None
 * Return        : >= 0 for success
 * Others        : 
 * Record
 * 1.Date        : 20140929
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int SendConfig(REALTIME_BACKUP_DATA *pBackupData, ULONG ulType);










#endif



