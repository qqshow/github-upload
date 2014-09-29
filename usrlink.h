/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : usrlink.h
 * Author        : lb
 * Date          : 2014-09-26
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-26
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/

#ifndef USRLINK_H_
#define USRLINK_H_
#include "module.h"

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
    WCHAR   wszBakCacheDir[FILEREPL_MAX_PATH];  //备份日志文件存放目录
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


#define USER_NETLINK_CMD	25
#define MAXMSGLEN 			1024

typedef enum error_e {
	NET_ERROR,
	NET_OK,
	NET_PARAM,
	NET_MEM,
	NET_SOCK,
} netlink_err;

typedef enum module_e {
	RTB_CMD = 1,
} netlink_module;

typedef enum type_e {
	RTB_SET,
	RTB_GET,
} netlink_type;

#endif /* USRLINK_H_ */


