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
		printk(KERN_ALERT "netlink_to_user skb of buf null!\n");
		return;
	}
	nl = nlmsg_put(skb, 0, 0, 0, NLMSG_SPACE(len) - sizeof(struct nlmsghdr), 0);
	NETLINK_CB(skb).pid = 0;
	NETLINK_CB(skb).dst_group = 0;

	memcpy(NLMSG_DATA(nl), buf, len);
	//nl->nlmsg_len = (len > 2) ? (len - 2):len;
	nl->nlmsg_len = len;
	netlink_unicast(netlink_fd, skb, dest, MSG_DONTWAIT);
	printk(KERN_ALERT "K send packet success\n");
}

int process_hello_get(int dest, void *buf, int len)
{
	PFILEREPL_NOTIFICATION preplnotify = (PFILEREPL_NOTIFICATION )buf;
	printk(KERN_ALERT "In process_hello get!\n");

	printk("Type:%d. Running status %d. CacheDir %s.\n",preplnotify->Type,preplnotify->bNormalRunning,\
		preplnotify->AddOrDel.BackupData.wszBakCacheDir);
	preplnotify->bNormalRunning = 1;
	netlink_to_user(dest, buf, sizeof(FILEREPL_NOTIFICATION));
	//memcpy(buf, "I known you !", 13);
	//netlink_to_user(dest, buf, 13);
	return NET_OK;
}

int process_hello_set(int dest, void *buf, int len)
{
	printk(KERN_ALERT "In process_hello set! %s\n", (char *)buf);
	memcpy(buf, "S known you !", 13);
	netlink_to_user(dest, buf, 13);
	return NET_OK;
}


void netlink_process_packet(struct nlmsghdr *nl)
{
	int ret;

	switch(nl->nlmsg_type)
	{
	case RTB_GET:
		ret = process_hello_get(nl->nlmsg_pid, NLMSG_DATA(nl), nl->nlmsg_len);
		break;
	case RTB_SET:
		ret = process_hello_set(nl->nlmsg_pid, NLMSG_DATA(nl), nl->nlmsg_len);
		break;
	default:break;
	}
}

void netlink_recv_packet(struct sk_buff *__skb)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlhdr;
	printk("netlink_recv_packet in.....\n");
	skb = skb_get(__skb);
	if(skb->len >= sizeof(struct nlmsghdr))
	{
		nlhdr = (struct nlmsghdr *)skb->data;
		printk("get nlmsg_len %d.\n",nlhdr->nlmsg_len);
		if(nlhdr->nlmsg_len >= sizeof(struct nlmsghdr) &&
				__skb->len >= nlhdr->nlmsg_len)
		{
			netlink_process_packet(nlhdr);
		}
	}
	else
		printk(KERN_ALERT "Kernel receive msg length error!\n");
}



