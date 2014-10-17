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
	FILEREPL_NOTIFICATION * preplnotify = NULL;
	replnotify.Type = NOTIFY_TYPE_ADDITEM;
	strncpy(replnotify.AddOrDel.BackupData.wszBakCacheDir,"/test/task1/",strlen("/test/task1/"));
	replnotify.AddOrDel.BackupData.ulSize = sizeof(REALTIME_BACKUP_DATA);
//	ret = netlink_sock_init(&h_sock, HELLO_CMD, USER_NETLINK_CMD);
//	if(NET_OK != parse_ret(ret))
//		goto exit_p;
    printf("cache dir is %s.\n",replnotify.AddOrDel.BackupData.wszBakCacheDir);
	RegisterUserAppNotify(callback);

	printf("size of FILEREPL_NOTIFICATION is %d.\n",sizeof(FILEREPL_NOTIFICATION));


	ret = InitCommunicate();

	// add backup set
	replnotify.Type = NOTIFY_TYPE_ADDSET;
	replnotify.AddOrDel.BackupData.guidSetId.Data1 =1;
	

	ret = AddBackupSet(&replnotify.AddOrDel.BackupData);

	sleep(5);

	ret = AddBackupSet(&replnotify.AddOrDel.BackupData);

	sleep(5);

	//add backup item
	int filerepllen = sizeof(FILEREPL_NOTIFICATION) + sizeof(FILTER_ITEM);
	preplnotify = malloc(filerepllen);
	memset(preplnotify,0,filerepllen);
	
	preplnotify->Type = NOTIFY_TYPE_ADDITEM;
	preplnotify->AddOrDel.BackupData.guidSetId.Data1 = 2;
	preplnotify->AddOrDel.BackupData.ulFilterItemCounts = 2;
	strncpy(preplnotify->AddOrDel.BackupData.wszBakCacheDir,"/test/task2/",strlen("/test/task2/"));
	preplnotify->AddOrDel.BackupData.ulSize = sizeof(REALTIME_BACKUP_DATA) + sizeof(FILTER_ITEM);
	preplnotify->AddOrDel.BackupData.FilterItems[0].ulFileType = FILTER_TYPE_FILE;
	strncpy(preplnotify->AddOrDel.BackupData.FilterItems[0].wszFilterName,"/bbbbb/",strlen("/bbbbb/"));
	preplnotify->AddOrDel.BackupData.FilterItems[1].ulFileType = FILTER_TYPE_FILE;
	strncpy(preplnotify->AddOrDel.BackupData.FilterItems[1].wszFilterName,"/cccccccccc/",strlen("/ccccccccc/"));

    printf("cache dir is %s.\n",preplnotify->AddOrDel.BackupData.wszBakCacheDir);
	ret = AddBackupItems(&preplnotify->AddOrDel.BackupData);

	sleep(5);


	ret = AddBackupItems(&preplnotify->AddOrDel.BackupData);
	sleep(5);

	//delete backup item
//	preplnotify->Type = NOTIFY_TYPE_DELITEM;
//	ret = DelBackupItems(&preplnotify->AddOrDel.BackupData);
	
	
	//delete all backup set
//	replnotify.Type = NOTIFY_TYPE_DELALL;
//	ret = DelAllBackup(&replnotify.AddOrDel.BackupData);
//	
//	ret = UnInitCommunicate();



exit_p:
	return 0;
}

