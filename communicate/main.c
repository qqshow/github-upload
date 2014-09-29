#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comwrapper.h"

/*
int parse_ret(int ret)
{
	switch(ret)
	{
	case NET_OK:
		return ret;
	case NET_ERROR:
		printf("error\n");
		goto exit_p;
	case NET_MEM:
		printf("Memory error\n");
		goto exit_p;
	case NET_PARAM:
		printf("Param error\n");
		goto exit_p;
	case NET_SOCK:
		printf("Socket error\n");
		goto exit_p;
	default:break;
	}
exit_p:
	return NET_ERROR;
}
*/

void usage(void)
{
	printf("Usage: Netlink -G <param>\n\t-S <param>\n");
}

int callback(int type,int status)
{
	int ret = 0;
	printf("TYPE %d. status %d.\n",type,status);
	return ret;
}

int main(int argc, char **argv)
{
	int ret, type, slen = 0, rlen = 0;
	FILEREPL_NOTIFICATION replnotify = {0};
	replnotify.Type = NOTIFY_TYPE_ADDITEM;
	strncpy(replnotify.AddOrDel.BackupData.wszBakCacheDir,"/test/",6);
	replnotify.AddOrDel.BackupData.ulSize = sizeof(REALTIME_BACKUP_DATA);
//	ret = netlink_sock_init(&h_sock, HELLO_CMD, USER_NETLINK_CMD);
//	if(NET_OK != parse_ret(ret))
//		goto exit_p;
	RegisterUserAppNotify(callback);

	printf("size of FILEREPL_NOTIFICATION is %d.\n",sizeof(FILEREPL_NOTIFICATION));


	ret = InitCommunicate();

	while(1)
	{
		sleep(1);

		ret = AddBackupItems(&replnotify.AddOrDel.BackupData);
	}
	ret = UnInitCommunicate();


/*
	ret = netlink_send(&h_sock, type, &replnotify, sizeof(FILEREPL_NOTIFICATION), &replnotify, &rlen);
	if(NET_OK != parse_ret(ret))
		goto exit_p;

	if(rlen > 0)
	{
//		rbuf[rlen] = '\0';
//		printf("K rep [len = %d]:%s\n", rlen, rbuf);
		printf("rep [len = %d]: Type is %d. Running statu is %d. CacheDir is %s.\n",rlen,replnotify.Type,replnotify.bNormalRunning,\
		replnotify.AddOrDel.BackupData.wszBakCacheDir);
	}
*/
	//printf("K[len = %d]: %s\n", rlen, rbuf);

exit_p:
	return 0;
}

