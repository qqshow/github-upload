/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : communicate.h
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

#include <sys/types.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/socket.h>
#include <linux/netlink.h>



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

typedef struct usr_sock_h {
	int sock;
	struct sockaddr_nl dest;
	struct sockaddr_nl src;
} netlink_sock;



int netlink_sock_init(netlink_sock *netlink_s, int module, int protocol);
int netlink_sock_deinit(netlink_sock *netlink_s);
int netlink_recv(netlink_sock *netlink_s,int type,char *rbuf, int *rlen);
int netlink_send(netlink_sock *netlink_s, int type, char *sbuf, int slen);

#endif /* USRLINK_H_ */



