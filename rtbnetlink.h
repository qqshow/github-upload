/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : rtbnetlink.h
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
#ifndef _RTBNETLINK_H_
#define _RTBNETLINK_H_

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/sock.h>	


void netlink_to_user(int dest, void *buf, int len);
int process_netlink_cmd(int dest, void *buf, int len);
int process_rtb_set(int dest, void *buf, int len);
void netlink_process_packet(struct nlmsghdr *nl);
void netlink_recv_packet(struct sk_buff *__skb);
int notify_user_status(int type,int status);

#endif



