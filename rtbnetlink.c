/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : rtbnetlink.c
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


#include "usrlink.h"
#include "rtbnetlink.h"
#include "monitorset.h"

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))

#include <net/netlink.h>

#endif

struct sock *netlink_fd;



void netlink_to_user(int dest, void *buf, int len)
{
	struct nlmsghdr *nl;
	struct sk_buff *skb;
	int size;

	size = NLMSG_SPACE(len);
	skb = alloc_skb(size, GFP_ATOMIC);
	if(!skb || !buf)
	{
		rtbprintk(KERN_ALERT "netlink_to_user skb of buf null!\n");
		return;
	}

	nl = nlmsg_put(skb, 0, 0, 0, NLMSG_SPACE(len) - sizeof(struct nlmsghdr), 0);

	NETLINK_CB(skb).pid = 0;
	NETLINK_CB(skb).dst_group = 0;

	memcpy(NLMSG_DATA(nl), buf, len);
	//nl->nlmsg_len = (len > 2) ? (len - 2):len;
	nl->nlmsg_len = len;
	netlink_unicast(netlink_fd, skb, dest, MSG_DONTWAIT);
	rtbprintk(KERN_ALERT "K send packet success\n");
}

int notify_user_status(int type,int status)
{
    int ret = NET_OK;
    PFILEREPL_NOTIFICATION preplnotify = kzalloc(sizeof(FILEREPL_NOTIFICATION), GFP_KERNEL);
    preplnotify->Type = type;
    preplnotify->bNormalRunning = status;
    rtbprintk(KERN_ALERT "RTB: notify user status %d. %d.\n",type,status);

    netlink_to_user(RTB_CMD, preplnotify, sizeof(FILEREPL_NOTIFICATION));

    return ret;
}

int process_netlink_cmd(int dest, void *buf, int len)
{
	int ret = NET_OK;
	PFILEREPL_NOTIFICATION preplnotify = (PFILEREPL_NOTIFICATION )buf;
	rtbprintk(KERN_ALERT "In process_hello get!\n");

	if(buf == NULL || len < sizeof(FILEREPL_NOTIFICATION))
		return NET_PARAM;
	rtbprintk("Type:%d. Running status %d. CacheDir %s.\n",preplnotify->Type,preplnotify->bNormalRunning,\
		preplnotify->AddOrDel.BackupData.wszBakCacheDir);

	switch(preplnotify->Type)
	{
		case NOTIFY_TYPE_ADDSET:
			rtbprintk("NOTIFY_TYPE_ADDSET\n");
			ret = ConfigAddMonitorSet(preplnotify,true);
			if(ret != 0)
			{
				rtbprintk("ConfigAddMonitorSet error.\n");
			}
			break;
		case NOTIFY_TYPE_ADDITEM:
			rtbprintk("NOTIFY_TYPE_ADDITEM\n");
			ret = ConfigAddMonitorSet(preplnotify,false);
			if(ret != 0)
			{
				rtbprintk("ConfigAddMonitorSet error.\n");
			}
			break;
		case NOTIFY_TYPE_DELSET:
			rtbprintk("NOTIFY_TYPE_DELSET\n");
			ret = ConfigDelMonitorSet(preplnotify);
			if(ret != 0)
			{
				rtbprintk("ConfigDelMonitorSet error\n");
			}
			break;
		case NOTIFY_TYPE_DELITEM:
			rtbprintk("NOTIFY_TYPE_DELITEM\n");
			ret = ConfigDelMonitorItem(preplnotify);
			if(ret != 0)
			{
				rtbprintk("ConfigDelMonitorItem error\n");
			}
			break;
		case NOTIFY_TYPE_DELALL:
			rtbprintk("NOTIFY_TYPE_DELALL\n");
			ret = ConfigDelAllMonitorSet();
			if(ret != 0)
			{
				rtbprintk("ConfigDelAllMonitorSet error\n");
			}
			break;
		case NOTIFY_TYPE_QUERY_RUNNING_STATUS:
			rtbprintk("NOTIFY_TYPE_QUERY_RUNNING_STATUS\n");
            notify_user_status(NOTIFY_TYPE_QUERY_RUNNING_STATUS,FileReplData.Config.bNormalRunning);
			break;
		case NOTIFY_TYPE_QUERY_LAST_SHUTDOWN_STATUS:
			rtbprintk("NOTIFY_TYPE_QUERY_LAST_SHUTDOWN_STATUS\n");
			FileReplData.Config.dwLastNormalShutdown = systemrunning;
            notify_user_status(NOTIFY_TYPE_QUERY_LAST_SHUTDOWN_STATUS,FileReplData.Config.dwLastNormalShutdown);
			break;
		case NOTIFY_TYPE_RESET_LAST_SHUTDOWN_STATUS:
			rtbprintk("NOTIFY_TYPE_RESET_LAST_SHUTDOWN_STATUS\n");
			FileReplData.Config.dwLastNormalShutdown = systemrunning;
			break;
		default:
			break;
	}


	//preplnotify->bNormalRunning = 1;
	//netlink_to_user(dest, buf, sizeof(FILEREPL_NOTIFICATION));
	return ret;
}

int process_hello_set(int dest, void *buf, int len)
{
	rtbprintk(KERN_ALERT "In process_hello set! %s\n", (char *)buf);
	memcpy(buf, "test !", 13);
	netlink_to_user(dest, buf, 13);
	return NET_OK;
}


void netlink_process_packet(struct nlmsghdr *nl)
{
	int ret;

	switch(nl->nlmsg_type)
	{
	case RTB_GET:
		ret = process_netlink_cmd(nl->nlmsg_pid, NLMSG_DATA(nl), nl->nlmsg_len);
		break;
	case RTB_SET:
		ret = process_hello_set(nl->nlmsg_pid, NLMSG_DATA(nl), nl->nlmsg_len);
		break;
	default:break;
	}
}

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19))

void netlink_receive_user_sk(struct sock *sk, int len)
{
	struct sk_buff *skb;
	unsigned int qlen = skb_queue_len(&sk->sk_receive_queue);

	for (; qlen && (skb = skb_dequeue(&sk->sk_receive_queue)); qlen--) {
		netlink_recv_packet(skb);
		kfree_skb(skb);
	}


}
#endif



void netlink_recv_packet(struct sk_buff *__skb)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlhdr;
	rtbprintk("netlink_recv_packet in.....\n");
	skb = skb_get(__skb);
	rtbprintk("rtb: skb->len is %d. nlmsghdr is %d.\n",skb->len,sizeof(struct nlmsghdr));
	if(skb->len >= sizeof(struct nlmsghdr))
	{
		nlhdr = (struct nlmsghdr *)skb->data;
		rtbprintk("get nlmsg_len %d.\n",nlhdr->nlmsg_len);
		if(nlhdr->nlmsg_len >= sizeof(struct nlmsghdr) &&
				__skb->len >= nlhdr->nlmsg_len)
		{
			netlink_process_packet(nlhdr);
		}
	}
	else
		rtbprintk(KERN_ALERT "Kernel receive msg length error!\n");
}




