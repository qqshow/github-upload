/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : communicate.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "communicate.h"
#include "comwrapper.h"


netlink_sock *p_sock = NULL;
void* thread_entry(void* context);
pthread_t pid;


//
static int (*DealKernelMsgCallback)(int a,int b)=NULL;

//register call user app
void RegisterUserAppNotify(int (*callback)(int,int))
{
 DealKernelMsgCallback=callback;
}


int InitCommunicate()
{
	int ret = 0;

	//initalized yet
	if(p_sock != NULL)
	{
		printf("initalized yet!\n");
		return NET_OK;
	}
		

	//init
	p_sock = malloc(sizeof(netlink_sock));
	if(p_sock == NULL)
	{
		printf("malloc error.\n");
	}

	ret = netlink_sock_init(p_sock, RTB_CMD, USER_NETLINK_CMD);
	if(NET_OK != ret)
	{
		printf("netlink sock init error\n");
		free(p_sock);
		p_sock = NULL;
	
	}
	int err;
	err = pthread_create(&pid, NULL, thread_entry, NULL);
	if (err != 0)
        printf("can't create thread: %s\n", strerror(err));
	printf("pthread_create return %d.\n",err);
	printf("InitCommunicate return %d.\n",ret);
	return ret;
	
}

int QueryDriverStatus()
{
	//return SendConfig(pBackupData,NOTIFY_TYPE_ADDSET);
	return 0;
}

int QueryLastShutDownStatus()
{
	//return SendConfig(pBackupData,NOTIFY_TYPE_ADDSET);
	return 0;
}

int ResetLastShutDownStatus()
{
	//return SendConfig(pBackupData,NOTIFY_TYPE_ADDSET);
	return 0;

}

int AddBackupSet (REALTIME_BACKUP_DATA * pBackupData)
{
	return SendConfig(pBackupData,NOTIFY_TYPE_ADDSET);
}

int AddBackupItems(REALTIME_BACKUP_DATA * pBackupData)
{
	int ret = 0;
//	int rlen = 0;
//	FILEREPL_NOTIFICATION replnotify = {0};
	ret = SendConfig(pBackupData,NOTIFY_TYPE_ADDITEM);

//	netlink_recv(p_sock,HELLO_CMD,(char *)&replnotify,&rlen);

//	printf("rep [len = %d]: Type is %d. Running statu is %d. CacheDir is %s.\n",rlen,replnotify.Type,replnotify.bNormalRunning,\
//		replnotify.AddOrDel.BackupData.wszBakCacheDir);
	return ret;

}

int DelBackupSet(REALTIME_BACKUP_DATA * pBackupData)
{
	return SendConfig(pBackupData,NOTIFY_TYPE_DELSET);

}

int DelBackupItems(REALTIME_BACKUP_DATA * pBackupData)
{
	return SendConfig(pBackupData,NOTIFY_TYPE_DELITEM);

}

int DelAllBackup()
{
	return SendConfig(NULL,NOTIFY_TYPE_DELALL);
}

int SendConfig(REALTIME_BACKUP_DATA *pBackupData, ULONG ulType)
{
	FILEREPL_NOTIFICATION *pFN = NULL;
	int ulFnSize = 0;
	int ret = 0;
	if(pBackupData == NULL)
	{
		return -1;
	}

	if(p_sock == NULL)
	{
		printf("netlink sock is not initalized.\n");
		return -2;
	}

	ulFnSize = pBackupData->ulSize+sizeof(FILEREPL_NOTIFICATION)-sizeof(REALTIME_BACKUP_DATA);

	pFN=(FILEREPL_NOTIFICATION*)malloc(ulFnSize);
	if(pFN == NULL)
	{
		printf("malloc error\n");
		return -3;
	}
	pFN->Type = ulType;

	memcpy((char *)&(pFN->AddOrDel.BackupData),(char *)pBackupData,pBackupData->ulSize);
	printf("cache dir is %s. ulsize is %d\n,%d.",pFN->AddOrDel.BackupData.wszBakCacheDir,pBackupData->ulSize,sizeof(REALTIME_BACKUP_DATA));
	ret = netlink_send(p_sock,RTB_CMD,(char *)pFN,ulFnSize);

	if(pFN)
	{
		free(pFN);
	}

	return ret;

	
}


int UnInitCommunicate()
{
	printf("Uninit Communicate...\n");
	if(p_sock != NULL)
	{
		netlink_sock_deinit(p_sock);
	}
	p_sock = NULL;
}


int netlink_sock_init(netlink_sock *netlink_s, int module, int protocol)
{
	//create netlink socket
	netlink_s->sock = socket(PF_NETLINK, SOCK_RAW, protocol);
	
	if(netlink_s->sock < 0)
	{
		printf("create socket error\n");
		return NET_SOCK;
	}
		
	memset(&netlink_s->src, 0 ,sizeof(netlink_s->src));
	netlink_s->src.nl_family = AF_NETLINK;
	netlink_s->src.nl_pid = module;
	netlink_s->src.nl_groups = 0;
	netlink_s->src.nl_pad = 0;

	//bind socket addr to sock
	if(bind(netlink_s->sock, (struct sockaddr *)&netlink_s->src, sizeof(netlink_s->src)) < 0)
	{
		printf("bind socket error.\n");
		return NET_SOCK;
	}
		

	//dest addr
	netlink_s->dest.nl_family = AF_NETLINK;
	netlink_s->dest.nl_pid = 0; // dest is kernel or multi broadcast.
	netlink_s->dest.nl_groups = 0;
	netlink_s->dest.nl_pad = 0;

	return NET_OK;
}

int netlink_recv(netlink_sock *netlink_s,int type,char *rbuf, int *rlen)
{
		struct msghdr msg;
		struct nlmsghdr *nlhdr = NULL;
		struct iovec iov;
		int ret;
	
		
		
		nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAXMSGLEN));
		if(NULL == nlhdr)
			return NET_MEM;
	
		memset(nlhdr,0,NLMSG_SPACE(MAXMSGLEN));
		nlhdr->nlmsg_len = NLMSG_SPACE(MAXMSGLEN);
		printf("nlmsg_len %d\n",nlhdr->nlmsg_len);
		nlhdr->nlmsg_pid = netlink_s->src.nl_pid;
		nlhdr->nlmsg_type = type;
		nlhdr->nlmsg_flags = 0;
	
		iov.iov_base = (void *)nlhdr;
		iov.iov_len = nlhdr->nlmsg_len;
	
		memset(&msg,0,sizeof(struct msghdr));
		msg.msg_name = (void *)&(netlink_s->dest);
		msg.msg_namelen = sizeof(netlink_s->dest);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
	

		ret = recvmsg(netlink_s->sock, &msg, 0);
		if(ret < 0)
		{
			printf("Read fail %d\n",ret);
			goto error;
		}
	
		memcpy(rbuf, NLMSG_DATA(nlhdr), nlhdr->nlmsg_len);
		*rlen = nlhdr->nlmsg_len;
		printf("recv len %d.\n",*rlen);
		return NET_OK;
	
	error:
		free(nlhdr);
		return NET_SOCK;


	

}


int netlink_send(netlink_sock *netlink_s, int type, char *sbuf, int slen)
{
	struct msghdr msg;
	struct nlmsghdr *nlhdr = NULL;
	struct iovec iov;
	int ret;

	
	
	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAXMSGLEN));
	if(NULL == nlhdr)
		return NET_MEM;

	memset(nlhdr,0,NLMSG_SPACE(MAXMSGLEN));
	memcpy(NLMSG_DATA(nlhdr), sbuf, slen);
	nlhdr->nlmsg_len = NLMSG_SPACE(slen);
	printf("nlmsg_len %d\n",nlhdr->nlmsg_len);
	nlhdr->nlmsg_pid = netlink_s->src.nl_pid;
	nlhdr->nlmsg_type = type;
	nlhdr->nlmsg_flags = 0;

	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;

	memset(&msg,0,sizeof(struct msghdr));
	msg.msg_name = (void *)&(netlink_s->dest);
	msg.msg_namelen = sizeof(netlink_s->dest);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	printf("before sendmsg\n");
	ret = sendmsg(netlink_s->sock, &msg, 0);
	if(ret < 0)
	{
		printf("Send fail ret is %d. last error is %d.\n",ret,errno);
		
		goto error;
	}
	printf("after sendmsg\n");

	return NET_OK;

error:
	free(nlhdr);
	return NET_SOCK;
}

int netlink_sock_deinit(netlink_sock *netlink_s)
{
	close(netlink_s->sock);
	memset(netlink_s, 0, sizeof(netlink_sock));
	return NET_OK;
}


void* thread_entry(void* context)
{

    int ret = 0;
	int rlen = 0;
	FILEREPL_NOTIFICATION replnotify = {0};
	while(1)
	{
		printf("sub thread...\n");
		if (NET_OK == netlink_recv(p_sock,RTB_CMD,(char *)&replnotify,&rlen))
		{

		printf("rep [len = %d]: Type is %d. Running statu is %d. CacheDir is %s.\n",rlen,replnotify.Type,replnotify.bNormalRunning,\
		replnotify.AddOrDel.BackupData.wszBakCacheDir);
		ProcessRecvMessage(&replnotify);
		}
		sleep(1);
	}
    
    return NULL;
}

int ProcessRecvMessage(PFILEREPL_NOTIFICATION notification)
{
	int hr = 0;
	switch(notification->Type)
	{
	case NOTIFY_TYPE_QUERY_RUNNING_STATUS:
		printf("NOTIFY_TYPE_QUERY_RUNNING_STATUS %d\n",notification->bNormalRunning);
		break;
	case NOTIFY_TYPE_QUERY_LAST_SHUTDOWN_STATUS:
		printf("NOTIFY_TYPE_QUERY_RUNNING_STATUS %d\n",notification->bNormalRunning);
		break;
	default:
		printf("%d %d\n",notification->Type,notification->bNormalRunning);
	    break;
	}
	hr = OnNotifyUserApp(notification->Type,notification->bNormalRunning);
	return hr;
}

int OnNotifyUserApp(int type,int Status)
{
	if(DealKernelMsgCallback == NULL)
	{
		printf("not register callback\n");
		return;
	}
	else
	{
		return DealKernelMsgCallback(type,Status);
	}
}





