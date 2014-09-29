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
// ��ӱ��ݼ�������Ѿ�������ͬGUID�ı��ݼ����ã����ɾ��
//
#define NOTIFY_TYPE_ADDSET                  6+NOTIFY_TYPE_USER_TO_DRV_START

// 
// ��ӱ��������Ѿ�������ͬGUID�ı��ݼ����ã�����ɾ��
//
#define NOTIFY_TYPE_ADDITEM         	    7+NOTIFY_TYPE_USER_TO_DRV_START

//
// ɾ�����ݼ����������ݼ��µ��ļ����Ŀ¼��
//
#define NOTIFY_TYPE_DELSET                  8+NOTIFY_TYPE_USER_TO_DRV_START

//
// ɾ���ļ����Ŀ¼������
//
#define NOTIFY_TYPE_DELITEM         	    9+NOTIFY_TYPE_USER_TO_DRV_START

//
// ɾ�����б��ݼ�����
//
#define NOTIFY_TYPE_DELALL           	    10+NOTIFY_TYPE_USER_TO_DRV_START

//
// ��ѯ�ϴιػ�״̬
//
#define NOTIFY_TYPE_QUERY_LAST_SHUTDOWN_STATUS 11+NOTIFY_TYPE_USER_TO_DRV_START

//
// �����ϴιػ�״̬
//
#define NOTIFY_TYPE_RESET_LAST_SHUTDOWN_STATUS 12+NOTIFY_TYPE_USER_TO_DRV_START


#define FILTER_TYPE_INVALID     0
#define FILTER_TYPE_FILE        1
#define FILTER_TYPE_DIR         2
#define FILTER_TYPE_SET         3

typedef struct _FILTER_ITEM_
{
    ULONG   ulFileType;   //0�ļ���1 Ŀ¼�� 
    WCHAR   wszFilterName[FILEREPL_MAX_PATH];
}FILTER_ITEM,  *PFILTER_ITEM;

typedef struct _REALTIME_BACKUP_DATA_
{
    ULONG   ulSize;                     //���ݵ��ܳ���
    GUID    guidSetId;                  //һ�����ݼ���ID��ʵ��Ҳ����Ϊ��һ�����ϣ�
    ULONG   ulFilterItemCounts;         //���������
    WCHAR   wszBakCacheDir[FILEREPL_MAX_PATH];  //������־�ļ����Ŀ¼
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


