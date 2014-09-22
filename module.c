
#include "module.h"


int sysctl = 1;

module_param(sysctl, int, 0);

int init_rtbackup(void) {

	int ret;

	ret = kernfunc_init();

	if (IN_ERR(ret))
		return ret;

	hijack_syscalls();

	printk(PKPRE "added to kernel\n");

	return ret;
}

static void exit_rtbackup(void) {

	undo_hijack_syscalls();
	printk(PKPRE "removed from kernel\n");

	return;
}

module_init(init_rtbackup);
module_exit(exit_rtbackup);

MODULE_AUTHOR("lb");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Real Time Backup(RtBackup) Module");
MODULE_VERSION("1.0.1");

