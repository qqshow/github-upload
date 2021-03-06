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
#include "iowritethread.h"
#include <linux/reboot.h>
int sysctl = 1;
extern struct sock *netlink_fd;
FILEREPL_DATA FileReplData;

static struct task_struct * _tsk;


module_param(sysctl, int, 0);

#define DEBUG_PRINT
int rtbprintk(const char *fmt, ...)
{
	int ret = 0;
#ifdef DEBUG_PRINT
	ret = printk(fmt);
#endif

	return ret;
}


static int myreboot(struct notifier_block *self, unsigned long event, void *data)
{
    rtbprintk(KERN_ALERT "Just a test! Event code: %li! System reboot now...", event);
    FileReplData.Config.dwLastNormalShutdown = 2;
    SaveConfig();
    return NOTIFY_OK;
}

static struct notifier_block myreboot_notifier = {
.notifier_call = myreboot,
};


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

	rtbprintk(KERN_ALERT "Init rtbackup!!\n");
	rtbprintk("RTB: V1.0.0.4\n");
	// get needed kernel function address
	ret = kernfunc_init();

	if (IN_ERR(ret))
		return ret;

    //register reboot and shutdown notifier
    register_reboot_notifier(&myreboot_notifier);
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
	netlink_fd = netlink_kernel_create(USER_NETLINK_CMD,netlink_receive_user_sk);
#elif(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
	netlink_fd = netlink_kernel_create(USER_NETLINK_CMD,0,netlink_receive_user_sk,THIS_MODULE);
#else
	netlink_fd = netlink_kernel_create(&init_net, USER_NETLINK_CMD, 0, netlink_recv_packet, NULL, THIS_MODULE);
#endif
	if(NULL == netlink_fd)
	{
		rtbprintk(KERN_ALERT "Init netlink failed!\n");
		return -1;
	}
	rtbprintk(KERN_ALERT "Init netlink success!\n");

    // load needed monitor set and files
	ret = InitMonitorSet();
	if (IN_ERR(ret))
	{
		rtbprintk("initmonitorset error.\n");
		return ret;
	}	

    //create io write thread
    initcompletion();

    _tsk = kthread_run(thread_iowrite, NULL, "thread_iowrite"); 

    if (IS_ERR(_tsk)) 
    {
        rtbprintk(KERN_INFO "create iowrite thread error!\n"); 
        return -2;
    }     
    else
    {
        rtbprintk(KERN_INFO "create iowrite thread ok!\n");  
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
	rtbprintk(PKPRE "removed from kernel\n");

	unregister_reboot_notifier(&myreboot_notifier);
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
	sock_release(netlink_fd->sk_socket);
#else
	netlink_kernel_release(netlink_fd);
#endif

    if (!IS_ERR(_tsk))
    {  
        ret = kthread_stop(_tsk);  

        rtbprintk(KERN_INFO "First thread function has stopped ,return %d\n", ret);
    }  
    
	UninitMonitorSet();
	rtbprintk(KERN_ALERT "Exit netlink!\n");

	return;
}

module_init(init_rtbackup);
module_exit(exit_rtbackup);

MODULE_AUTHOR("lb");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Real Time Backup(RtBackup) Module");
MODULE_VERSION("1.0.0.4");

