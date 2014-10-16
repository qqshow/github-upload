/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : module.c
 * Author        : lb
 * Date          : 2014-09-19
 * Description   : module init 
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-19
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/


#include "module.h"
#include "usrlink.h"
#include "rtbnetlink.h"
#include "fruk.h"
#include "rb.h"
#include "monitorset.h"

int sysctl = 1;
extern struct sock *netlink_fd;
FILEREPL_DATA FileReplData;






module_param(sysctl, int, 0);

/*****************************************************************************
 * Function      : init_rtbackup
 * Description   : module init
 * Input         : void  
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140919
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int init_rtbackup(void) {

	int ret;

	// get some kernel function address
	ret = kernfunc_init();

	if (IN_ERR(ret))
		return ret;


	netlink_fd = netlink_kernel_create(&init_net, USER_NETLINK_CMD, 0, netlink_recv_packet, NULL, THIS_MODULE);
	if(NULL == netlink_fd)
	{
		printk(KERN_ALERT "Init netlink failed!\n");
		return -1;
	}
	printk(KERN_ALERT "Init netlink success!\n");

	ret = InitMonitorSet();
	if (IN_ERR(ret))
	{
		printk("initmonitorset error.\n");
		return ret;
	}	

    //hijack vfs function
	hijack_syscalls();
	return ret;
}

/*****************************************************************************
 * Function      : exit_rtbackup
 * Description   : module exit
 * Input         : void  
 * Output        : None
 * Return        : static
 * Others        : 
 * Record
 * 1.Date        : 20140923
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
static void exit_rtbackup(void) {
	//restore vfs function
	int ret = 0;
	undo_hijack_syscalls();
	printk(PKPRE "removed from kernel\n");

	
	netlink_kernel_release(netlink_fd);

	UninitMonitorSet();
	printk(KERN_ALERT "Exit netlink!\n");
	return;
}

module_init(init_rtbackup);
module_exit(exit_rtbackup);

MODULE_AUTHOR("lb");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Real Time Backup(RtBackup) Module");
MODULE_VERSION("1.0.1");

