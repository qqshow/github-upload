#include "module.h"
#include "monitorset.h"
#include "rb.h"
#include "fileop.h"

void *EntryAlloc(struct kmem_cache * pmemcache)
{
	if(pmemcache == NULL)
		return NULL;
	return kmem_cache_zalloc(pmemcache,GFP_KERNEL);
}

void EntryFree(struct kmem_cache * pmemcache, void *p)
{
	if(pmemcache != NULL && p != NULL)
		kmem_cache_free(pmemcache,p);
	
}

int checkneedtolog(char *path)
{
    PMONITOR_FILE_ENTRY pmfe = NULL;
	pmfe = mi_search(&FileReplData.Config.MonitorFiles.RBTree,path);
	if(pmfe)
		return 1;
	else
		return 0;
}

 PMONITOR_SET_ENTRY MonitorSetEntryAlloc(void)
 {	 
 	 PMONITOR_SET_ENTRY pmse = NULL;
 	 pmse = (PMONITOR_SET_ENTRY)EntryAlloc(FileReplData.Config.MonitorSet.ItemLookAsideList);
	 if(pmse == NULL)
	 	printk("alloc mem for monitorset failed.\n");
	 
	 return pmse;
 }


 void DumpPfn(PFILEREPL_NOTIFICATION pfn)
 {
 	
 }
 
 void	 
 MonitorSetEntryFree(
						   PMONITOR_SET_ENTRY pmse
						   )
 {
	 if(pmse != NULL)
	 	EntryFree(FileReplData.Config.MonitorSet.ItemLookAsideList,pmse);
 }

 PMONITOR_FILE_ENTRY MonitorFileEntryAlloc(void)
 {	 
 	 PMONITOR_FILE_ENTRY pmfe = NULL;
	 pmfe =(PMONITOR_FILE_ENTRY)EntryAlloc(FileReplData.Config.MonitorFiles.ItemLookAsideList);
	 if(pmfe == NULL)
	 	printk("alloc mem for monitorfile failed.\n");
	 return pmfe;
 }
 
 void	 
 MonitorFileEntryFree(
						   PMONITOR_FILE_ENTRY pmfe
						   )
 {
	 if(pmfe != NULL)
	 	EntryFree(FileReplData.Config.MonitorFiles.ItemLookAsideList,pmfe);
 }



 int DelAllMonitorSet()
 {
	 int status = 0;
	 struct rb_node *node = NULL;
	 PMONITOR_ENTRY_HEAD pmeh = NULL;
	 printk("DelAllmonitorSet\n");
		 
		 //释放RB树
	 for (node = rb_first(&FileReplData.Config.MonitorFiles.RBTree); node; node = rb_next(node))
	 {
		 pmeh = rb_entry(node, MONITOR_ENTRY_HEAD, rbnode);
		 if(pmeh != NULL)
		 {
		 	 rb_erase(pmeh,&FileReplData.Config.MonitorFiles.RBTree);
			 EntryFree(FileReplData.Config.MonitorFiles.ItemLookAsideList,pmeh);
			 printk("delete from monitor files.\n");
		 }
				 
	 }

	 FileReplData.Config.MonitorFiles.ulCounts=0;
	 
	 for (node = rb_first(&FileReplData.Config.MonitorSet.RBTree); node; node = rb_next(node))
	 {
		 pmeh = rb_entry(node, MONITOR_ENTRY_HEAD, rbnode);
		 if(pmeh != NULL)
		 {
		 	 rb_erase(pmeh,&FileReplData.Config.MonitorSet.RBTree);
			 EntryFree(FileReplData.Config.MonitorSet.ItemLookAsideList,pmeh);
			 printk("delete from monitor set.\n");
		 }
				 
	 }

	FileReplData.Config.MonitorSet.ulCounts=0;

 	return status;


 }

int ConfigDelAllMonitorSet()
{
    SaveConfig();
	return DelAllMonitorSet();
}


int DumpAllMonitorSet()
{

	 int status = 0;
	 struct rb_node *node = NULL;
	 PMONITOR_FILE_ENTRY pmfe = NULL;
	 PMONITOR_SET_ENTRY pmse = NULL;
	 printk("DUMP ALL MONITOR IN.....\n");
		 
	for (node = rb_first(&FileReplData.Config.MonitorSet.RBTree); node; node = rb_next(node))
	 {
		 pmse = rb_entry(node, MONITOR_SET_ENTRY, hdr.rbnode);
		 if(pmse != NULL)
		 {
			printk( "Monitorset, guid %ld, ulFileCount = %ld, ullSetSeqNo = %lld, cache dir %s.\n", \
			pmse->hdr.guidSetId.Data1, \
            pmse->ulFileCount, \
            pmse->ullSetSeqNo, \
            pmse->wcsSetCacheDir
            );
		 }
				 
	 }
	
	 for (node = rb_first(&FileReplData.Config.MonitorFiles.RBTree); node; node = rb_next(node))
	 {
		 pmfe = rb_entry(node, MONITOR_FILE_ENTRY, hdr.rbnode);
		 if(pmfe != NULL)
		 {
			printk( "UnitMonitorFiles, %s, ullSeqNo = %lld, bMonitor = 0x%x, bInited = 0x%x\n", 
            pmfe->wcsMonitorFile, 
            pmfe->ullSeqNo,
            pmfe->bMonitor,
            pmfe->bInited );
		 }
				 
	 }


	
	printk("DUMP ALL MONITOR OUT.....\n");
	return status;


}



 int ConfigDelMonitorItem(PFILEREPL_NOTIFICATION pfn)
 {
	 int status=0;
	 ULONG		 ulI;
	 REALTIME_BACKUP_DATA	 *pBackupData;
	 FILTER_ITEM *pItem;
	 MONITOR_FILE_ENTRY  mfe;
	 PMONITOR_FILE_ENTRY  pmfeFound = NULL;
	 MONITOR_FILE_DATA *pMonitorItems;
 
	 if(pfn == NULL)
	 {
		 return -1;
	 }
 
	 pBackupData = &pfn->AddOrDel.BackupData;
	 pItem = pBackupData->FilterItems;
 
	 for(ulI=0;ulI<pBackupData->ulFilterItemCounts;ulI++,pItem++)
	 {
	 	 memset(&mfe,0,sizeof(MONITOR_FILE_ENTRY));
		 memcpy(&mfe.hdr.guidSetId,&pBackupData->guidSetId,sizeof(GUID));
		 strncpy(mfe.wcsMonitorFile,pItem->wszFilterName,strlen(pItem->wszFilterName));
		 printk("RTB: ConfigDelMonitorItem count %d. %s\n",pBackupData->ulFilterItemCounts,pItem->wszFilterName);
 
 /////////////////////////////////////////////////////////////////////////////////////////
		 pMonitorItems = &FileReplData.Config.MonitorFiles;
		 if(pItem->ulFileType == FILTER_TYPE_FILE)
		 {
			 mfe.bDirectory = false;
			 //pMonitorItems = &FileReplData.Config.MonitorFiles;
		 }
		 else if(pItem->ulFileType == FILTER_TYPE_DIR)
		 {
			 mfe.bDirectory = true;
			 //pMonitorItems = &FileReplData.Config.MonitorDirs;
		 }
		 else
		 {
			 printk("AddMonitorItems error.unknown type=%ld.",pItem->ulFileType);
			 continue;
		 }
		 printk("RTB: DelMonitorItem %s.\n",mfe.wcsMonitorFile);
 ///////////////////////////////////////////////////////////////////////////////////////
 		 pmfeFound = (PMONITOR_FILE_ENTRY)mi_remove(&pMonitorItems->RBTree,mfe.wcsMonitorFile);
		 if(pmfeFound != NULL)
		 {
		 	 printk("RTB: Found\n");
			 EntryFree(pMonitorItems->ItemLookAsideList,pmfeFound);
		 }
	 }
 
     SaveConfig();
	 return status;
 }




 int  DelMonitorItemsBySetId(MONITOR_FILE_DATA  *pMonitorItems, GUID guidSet)
 {
	 int status = 0;
	 struct rb_node *node = NULL;
	 PMONITOR_ENTRY_HEAD pmeh = NULL;
	for (node = rb_first(&pMonitorItems->RBTree); node; node = rb_next(node))
	{
		pmeh = rb_entry(node, MONITOR_ENTRY_HEAD, rbnode);
		if(memcmp(&pmeh->guidSetId,&guidSet,sizeof(GUID)) == 0)
		{
			rb_erase(node,&pMonitorItems->RBTree);
			if(pMonitorItems->ulCounts > 0)
			 {
				 pMonitorItems->ulCounts--;
			 }
			EntryFree(pMonitorItems->ItemLookAsideList,pmeh);
		}
	}

	return status;
 
 }



 int AddMonitorItem(MONITOR_FILE_DATA *pMonitorData, PMONITOR_FILE_ENTRY pmfe)
{
    int              status = 0;
    PMONITOR_FILE_ENTRY  pmfeFound = NULL;
    PMONITOR_FILE_ENTRY  pmfeNew = NULL;

    if( pmfe == NULL )
    {
        return -1;
    }

	//源文件路径至少也要有c:\x，所以至少要4个字符以上，并且第2字符必须是:
	if(strlen(pmfe->wcsMonitorFile) < 2 || pmfe->wcsMonitorFile[0] != '/')
	{
		printk("AddMonitorItem.STATUS_INVALID_PARAMETER pmfe->wcsMonitorFile = %s", pmfe->wcsMonitorFile);
		return -1;
	}
	printk("RTB: AddmonitorItem %s.\n",pmfe->wcsMonitorFile);
	
		//WaitWriteForMonitor();

			pmfeFound = (PMONITOR_FILE_ENTRY)mi_search(&pMonitorData->RBTree,pmfe->wcsMonitorFile);
		
			if(pmfeFound == NULL)
			{
				pmfeNew = MonitorFileEntryAlloc();
				if(pmfeNew == NULL)
				{
					status = -3;
					//WriteEvent(FileReplData.DriverObject, status, L"MonitorListEntryAlloc");
					return status;
				}


				memcpy(pmfeNew, pmfe, sizeof(MONITOR_FILE_ENTRY));
				printk("RTB: test1.....\n");
				status = mi_insert(&pMonitorData->RBTree,pmfeNew);

				printk("RTB status is %d.\n",status);
				
				if( status != 0)
				{
					MonitorFileEntryFree(pmfeNew);
					status = -2;
					//WriteEvent(FileReplData.DriverObject, status, L"rbsearch");
					printk("AddMonitorItem failed. %s", pmfe->wcsMonitorFile);
					return status;
				}
				else
				{	
					// 添加成功
                    pmfeNew->bMonitor= true;
                    pmfeNew->pSetEntry = NULL;
					pMonitorData->ulCounts++;
                    printk("AddMonitorItem. pmfe->wcsMonitorFile = %s", pmfeNew->wcsMonitorFile);
				}
			}
			else
			{
////////////////////////////////////////////////////////////////////////////////////
				//原有配置项和本次配置项是否完全匹配
				if(strcmp(pmfeFound->wcsMonitorFile, pmfe->wcsMonitorFile) == 0)
				{
					if(memcmp(&pmfeFound->hdr.guidSetId, &pmfe->hdr.guidSetId, sizeof(GUID)) == 0)
					{
						//重复配置,
						if(!pmfeFound->bMonitor)
						{
							//重置过滤
							pmfeFound->bMonitor = true;
							pmfeFound->ullSeqNo = 0;
						}
						//else 不做改变
					}
					else//换一个备份集
					{
						memcpy(&pmfeFound->hdr.guidSetId, &pmfe->hdr.guidSetId, sizeof(GUID));
						pmfeFound->bMonitor = true;
						pmfeFound->ullSeqNo = 0;
						pmfeFound->pSetEntry = GetMonitorSetEntry(pmfe->hdr.guidSetId);
					}
				}
				else
				{
					//配置冲突
					status = -5;
					//WriteEvent(FileReplData.DriverObject, status, pmfe->wcsMonitorFile);
					printk("AddMonitorItem failed: name collision!. %s", pmfe->wcsMonitorFile);
				}
////////////////////////////////////////////////////////////////////////////////////
			}
		



    return status;
}

int AddMonitorFile(PMONITOR_FILE_ENTRY pmfe)
{
    return AddMonitorItem(&FileReplData.Config.MonitorFiles,pmfe);
}





PMONITOR_SET_ENTRY 
GetMonitorSetEntry(
						GUID guidSet
						)
{
	int                status = 0;
	MONITOR_SET_ENTRY  mse;
	MONITOR_SET_ENTRY  *pmseFound = NULL;

	if(FileReplData.Config.MonitorSet.ulCounts > 0)
	{
        mse.hdr.guidSetId = guidSet;

		pmseFound = (MONITOR_SET_ENTRY*)ms_search(&FileReplData.Config.MonitorSet.RBTree,guidSet);
	}

	return pmseFound;
}

//
// 添加监控项，按文件和目录插入不同队列[改为使用相同的队列]
//
int ConfigAddMonitorSet(PFILEREPL_NOTIFICATION pfn,
                             BOOL bDelExistSet)
{
    int    status = 0;
    ULONG       ulI;
    REALTIME_BACKUP_DATA    *pBackupData=NULL;
    FILTER_ITEM *pItem;
    MONITOR_SET_ENTRY   mse;
    MONITOR_FILE_ENTRY  mfe;
	PMONITOR_SET_ENTRY pmse = NULL;
    memset(&mse,0,sizeof(MONITOR_SET_ENTRY));
    memset(&mfe,0,sizeof(MONITOR_FILE_ENTRY));
	printk("ConfigAddMonitorSet bDelExisSet %d.\n");
    if(pfn == NULL)
    {
        return -1;
    }

    pBackupData=&pfn->AddOrDel.BackupData;

    //mse.hdr.guidSetId = pBackupData->guidSetId;
    memcpy(&mse.hdr.guidSetId, &pBackupData->guidSetId,sizeof(GUID));
	mse.ullSetSeqNo = 0;
	
	

    strncat(mse.wcsSetCacheDir,pBackupData->wszBakCacheDir, strlen(pBackupData->wszBakCacheDir));

    status = AddMonitorSet(&mse,bDelExistSet);
    if (0 != (status))
    {
        if(status == -2 && !bDelExistSet)
        {
            printk("ConfigAddMonitorSet.AddMonitorSet.STATUS_OBJECT_NAME_COLLISION");
        }
        else
        {
            //NotifyClientDriverError(status);
            //WriteEvent(FileReplData.DriverObject, status, L"ConfigAddMonitorSet.AddMonitorSet");
            printk("ConfigAddMonitorSet.AddMonitorSet error. %d\n",status);
            return status;
        }
    }
	pmse = ms_search(&FileReplData.Config.MonitorSet.RBTree,mse.hdr.guidSetId);
	if(pmse == NULL)
	{
		printk("RTB: ConfigAddMonitorSet search error \n");
		return -3;
	}
    pItem = pBackupData->FilterItems;
    for(ulI = 0; ulI<pBackupData->ulFilterItemCounts; ulI++,pItem++)
    {
    	printk("RTB: Filter num %d,count is %d. filter name is %s.\n",ulI,pBackupData->ulFilterItemCounts,pItem->wszFilterName);
		memset(&mfe,0,sizeof(mfe));
		mfe.pSetEntry = pmse;
		memcpy(&mfe.hdr.guidSetId,&pBackupData->guidSetId,sizeof(GUID));
        status = strncat(mfe.wcsMonitorFile, pItem->wszFilterName,strlen(pItem->wszFilterName));

        if(pItem->ulFileType == FILTER_TYPE_FILE)
        {
        	mfe.ulLength = strlen(mfe.wcsMonitorFile);
            status = AddMonitorFile(&mfe);
        }
        else if(pItem->ulFileType == FILTER_TYPE_DIR)
        {
/////////////////////////////////////////////////////////////////
            //status = AddMonitorDir(&mfe);

			//设置目录标志,并放到相同的搜索树中
			mfe.bDirectory = true;
			mfe.ulLength = strlen(mfe.wcsMonitorFile);
			status = AddMonitorFile(&mfe);
/////////////////////////////////////////////////////////////////
        }
        else
        {
            printk("AddMonitorItems error.unknown type=%d.",pItem->ulFileType);
            continue;
        }

        if (0 != (status))
        {
///////////////////////////////////////////////////////////////////////////////
			if(status == -5)
			{
				//配置冲突，将本次配置删除，不影响原来的配置工作
				pBackupData->ulFilterItemCounts = ulI - 1;
				ConfigDelMonitorItem(pfn);

                //WriteEvent(FileReplData.DriverObject, status, L"ConfigAddMonitorSet.AddMonitorFile name_collision!!!");
				printk("ConfigAddMonitorSet.AddMonitorFile or AddMonitorDir name_collision:%s",mfe.wcsMonitorFile);
			}
			else//驱动出错
			{
				//NotifyClientDriverError(status);
                //WriteEvent(FileReplData.DriverObject, status, L"ConfigAddMonitorSet.AddMonitorFile or AddMonitorDir error");
                printk("ConfigAddMonitorSet.AddMonitorFile or AddMonitorDir error.ulFileType=%d",pItem->ulFileType);
				return status;
			}
			break;
///////////////////////////////////////////////////////////////////////////////
        }
		else
		{
			pmse->ulFileCount++;
			}
    }
    SaveConfig();
    return status;
}

//
//
//
int ConfigDelMonitorSet(PFILEREPL_NOTIFICATION pfn)
{
    int status = 0;
    MONITOR_SET_ENTRY mse;

    if(pfn == NULL)
    {
        return -1;
    }

	memcpy(&mse.hdr.guidSetId,&pfn->AddOrDel.BackupData.guidSetId,sizeof(GUID));
    status = DelMonitorSet(&mse, true);
    SaveConfig();
    return status;
}

//
// 添加备份集，如果同ID备份集存在，则先删除原备份集所有配置后，再重新添加
//
int AddMonitorSet(MONITOR_SET_ENTRY *pSet,BOOL bDelExist)
{
    int status = 0;
    PMONITOR_SET_ENTRY  pmse=NULL;
    PMONITOR_SET_ENTRY  pmseFound=NULL;
	int ret = 0;

    if(pSet == NULL)
    {
        return -1;
    }

	printk("addmonitorset bdelExist %d.\n",bDelExist);
    //WaitWriteForMonitor();


        //
        // 先查找是否有同GUID的备份集
        //
        pmse = GetMonitorSetEntry(pSet->hdr.guidSetId);
        if(pmse)
        {
            if(bDelExist)
            {
                // 如果有，则删除所有该备份集中的设置项
                DelMonitorSet(pSet,false);
                pmse = NULL;
            }
            else
            {
                status = -2;
				return status;
            }
        }

        // 添加新备份集

        pmse = MonitorSetEntryAlloc();
        if(pmse == NULL)
        {
            status = -3;
            //WriteEvent(FileReplData.DriverObject, status, L"MonitorSetEntryAlloc");
            return status;
        }

        strncat(pmse->wcsSetCacheDir, pSet->wcsSetCacheDir,sizeof(pmse->wcsSetCacheDir));


		memcpy(&pmse->hdr.guidSetId, &pSet->hdr.guidSetId, sizeof(GUID));
		pmse->ullSetSeqNo = pSet->ullSetSeqNo;
        pmse->ulFileCount = 0;

		ret = ms_insert(&FileReplData.Config.MonitorSet.RBTree,pmse);
		if(ret != 0)
		{
			    MonitorSetEntryFree(pmse);
                status = -1;
                //WriteEvent(FileReplData.DriverObject, status, L"rbsearch");
                printk("SetMonitorSet failed. %S", pmse->wcsSetCacheDir);
		}
		else
		{
			                // 备份集添加成功
                FileReplData.Config.MonitorSet.ulCounts++;
		}

    return status;
}

//
// BOOL bNeedSync - 函数中是否用同步锁
//
int DelMonitorSet(MONITOR_SET_ENTRY *pSet,BOOL bNeedSync)
{
    int status = 0;
    int    status1;
	printk("DelMonitorSet\n");

    if(pSet==NULL)
    {
        return -1;
    }

    if(bNeedSync)
    {
        //WaitWriteForMonitor();
    }

    //
    // 根据备份集搜索到所有备份项并删除
    //
    status1 = DelMonitorItemsBySetId(&FileReplData.Config.MonitorFiles,pSet->hdr.guidSetId);
    if(0 != (status1))
    {
        status = status1;
    }

	///////////////////////////////////////////////////////////////////////////////////////
	//不再使用单独的目录过滤项搜索树
    //status1=DelMonitorItemsBySetId(&FileReplData.Config.MonitorDirs,pSet->hdr.guidSetId);
    //if(!NT_SUCCESS(status1))
    //{
    //    status=status1;
    //}
	///////////////////////////////////////////////////////////////////////////////////////

    status1 = DelMonitorItemsBySetId(&FileReplData.Config.MonitorSet,pSet->hdr.guidSetId);
    if(0 != (status1))
    {
        status = status1;
    }

    if(bNeedSync)
    {
        //LeaveReadOrWriteForMonitor();
    }

	printk("DelMonitorSet status %d.\n",status);
    return status;
}




// get monitor file entry by abspath
PMONITOR_FILE_ENTRY GetMonitorFileEntryByabspath(char *abspath)
{
	PMONITOR_FILE_ENTRY pmfe = NULL;
	pmfe = mi_search(&FileReplData.Config.MonitorFiles.RBTree,abspath);
	if(pmfe)
		return pmfe;
	else
		return NULL;
//	if(strncmp(abspath,"/test/",strlen("/test/")) == 0 )
//		return 1;
//	else
//		return 0;
}


int  InitMonitorSet(void)
{
	int status = 0;


		// ExInitializeResourceLite(&FileReplData.Config.MonitorFiles.SyncResource);

	/*	ExInitializeNPagedLookasideList( &FileReplData.Config.MonitorSet.ItemLookAsideList,
			NULL,
			NULL,
			0,
			sizeof(MONITOR_SET_ENTRY),
			ITEM_ILALIST_POOL_TAG,
			0 );*/

		//INIT_LIST_HEAD(&FileReplData.Config.MonitorSet.listHead);
		//InitializeListHead(&FileReplData.Config.MonitorSet.listHead);
		memset(&FileReplData,0,sizeof(FILEREPL_DATA));
		FileReplData.Config.MonitorFiles.ItemLookAsideList = kmem_cache_create("MONITOR_FILES_ENTRY",sizeof(MONITOR_FILE_ENTRY),0,SLAB_HWCACHE_ALIGN,NULL);
		if(FileReplData.Config.MonitorFiles.ItemLookAsideList == NULL)
		{
			printk(PKPRE "create kmem cache error");
			return -1;
		}
		
		FileReplData.Config.MonitorSet.ItemLookAsideList = kmem_cache_create("MONITOR_SET_ENTRY",sizeof(MONITOR_SET_ENTRY),0,SLAB_HWCACHE_ALIGN,NULL);
		if(FileReplData.Config.MonitorSet.ItemLookAsideList == NULL)
		{
			printk(PKPRE "create kmem cache error");
			return -1;
		}



		
		FileReplData.Config.MonitorSet.RBTree = RB_ROOT;
		FileReplData.Config.MonitorFiles.RBTree = RB_ROOT;


	    LoadConfig();
	return status;
}

int 
UninitMonitorSet(void)
{
	int status = 0;
    struct rb_node *node = NULL;
	PMONITOR_ENTRY_HEAD pmeh = NULL;
	// FPLogF("UninitMonitorSet ==>");
	// SaveMonitorFiles();

	printk("UninitMonitorSet\n");
	DumpAllMonitorSet();
	ConfigDelAllMonitorSet();
	DumpAllMonitorSet();
		//ExDeleteNPagedLookasideList(&FileReplData.Config.MonitorSet.ItemLookAsideList);

	kmem_cache_destroy(FileReplData.Config.MonitorFiles.ItemLookAsideList);
	kmem_cache_destroy(FileReplData.Config.MonitorSet.ItemLookAsideList);

	printk("UninitMonitorSet exit\n");

	return status;
}




int GetMonitorFileSeqNoByMonitorFileEntry(PMONITOR_FILE_ENTRY pmfe,
						ULONGLONG* pullSeqNo,  ULONGLONG* pullGlocalSetSeqNo ,ULONGLONG *timesecs,char *iologdir)
{
    int  status = 0;
	PMONITOR_SET_ENTRY pmse = NULL;
    struct timespec ts;
    
    if( pmfe == NULL
		|| pullSeqNo == NULL
		|| pullGlocalSetSeqNo == NULL)
    {
        return -1;
    }

    //WaitWriteForMonitor();


    pmse = pmfe->pSetEntry;
    if (pmse == NULL)
    {
            pmfe->pSetEntry= pmse = GetMonitorSetEntry(pmfe->hdr.guidSetId);
            if (pmse == NULL)
            {
                status = -3;
                return status;
            }
    }
    
    ts = current_kernel_time();
    
    *timesecs = ts.tv_sec;
    *pullSeqNo = pmfe->ullSeqNo;
    pmfe->ullSeqNo++;

    *pullGlocalSetSeqNo = pmse->ullSetSeqNo;
    pmse->ullSetSeqNo++;
    printk("RTB: wcsSetCacheDir %s.\n",pmfe->pSetEntry->wcsSetCacheDir);
    strncpy(iologdir,pmfe->pSetEntry->wcsSetCacheDir,strlen(pmfe->pSetEntry->wcsSetCacheDir));
    printk("RTB: iologdir %s.\n",iologdir);
    return status;
}


/*****************************************************************************
 * Function      : LoadConfig
 * Description   : load config 
 * Input          : None
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20141016
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int LoadConfig()
{
    int ret = 0;
    int i = 0;
	CONFIG_FILE_HEADER cfh;
	ULONG cfh_size  = 0;
    ULONG readoffset = 0;
	struct file *filep = NULL;
    struct rb_node *node = NULL;
	PMONITOR_FILE_ENTRY pmfe = NULL;
    PMONITOR_SET_ENTRY pmse = NULL;

    printk("RTB: LoadConfig.\n");
    cfh_size = sizeof(CONFIG_FILE_HEADER);
	memset(&cfh,0,cfh_size);

    filep = file_open("/etc/rtb.cnf",O_RDONLY | O_CREAT, 777);
	if (IS_ERR(filep))
	{
        printk("RTB: open config file error\n");
        goto out;
	}

    //read config header
    if(file_read(filep, 0, &cfh, sizeof(CONFIG_FILE_HEADER)) != sizeof(CONFIG_FILE_HEADER))
    {
        printk("RTB: read config file header error \n");
        goto out;
    }
    FileReplData.Config.dwLastNormalShutdown = cfh.ulLastShutdownStatus;
    printk("RTB: header size %d. shutdown status %d. set offset %d. set size %d. file offset %d. \
        file size %d\n",cfh.ulHeaderSize,cfh.ulLastShutdownStatus,cfh.ulOffsetSet,cfh.ulSizeSet,\
          cfh.ulOffsetFiles,cfh.ulSizeFiles);

    //read backup set
    for(i=0; i < cfh.ulSizeSet / sizeof(MONITOR_SET_ENTRY); i++)
    {
        pmse = MonitorSetEntryAlloc();
        if(pmse == NULL)
            goto out;

        
        if(file_read(filep, cfh.ulOffsetSet + i * sizeof(MONITOR_SET_ENTRY), pmse, \
            sizeof(MONITOR_SET_ENTRY)) != sizeof(MONITOR_SET_ENTRY))
        {
            printk("RTB: read backup set error \n");
            goto out;
        }
        printk("RTB: loadconfig monitor set guid %d.\n",pmse->hdr.guidSetId.Data1);
        ms_insert(&FileReplData.Config.MonitorSet.RBTree, pmse);
    }

    //read backup item
    for(i=0; i < cfh.ulSizeFiles / sizeof(MONITOR_FILE_ENTRY); i++)
    {
        pmfe = MonitorFileEntryAlloc();
        if(pmfe == NULL)
            goto out;

        
        if(file_read(filep, cfh.ulOffsetFiles+ i * sizeof(MONITOR_FILE_ENTRY), pmfe, \
            sizeof(MONITOR_FILE_ENTRY)) != sizeof(MONITOR_FILE_ENTRY))
        {
            printk("RTB: read backup file error \n");
            goto out;
        }

        pmse = ms_search(&FileReplData.Config.MonitorSet.RBTree,pmfe->hdr.guidSetId);
        if(pmse != NULL)
        {
            pmfe->pSetEntry = pmse;
            printk("RTB: loadconfig monitor item %s.\n",pmfe->wcsMonitorFile);
            mi_insert(&FileReplData.Config.MonitorFiles.RBTree, pmfe);
        }
        else
        {
            MonitorFileEntryFree(pmfe);
        }
    }
    
out:
    if(filep)
        file_close(filep);
    return ret;
	
    
}

/*****************************************************************************
 * Function      : SaveConfig
 * Description   : save config
 * Input          : None
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20141016
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
int SaveConfig()
{
    int ret = 0;
	CONFIG_FILE_HEADER cfh;
	ULONG cfh_size  = 0;
    ULONG writeoffset = 0;
	struct file *filep = NULL;
    struct rb_node *node = NULL;
	PMONITOR_FILE_ENTRY pmfe = NULL;
    PMONITOR_SET_ENTRY pmse = NULL;
    

    
	printk("RTB: SaveConfig.\n");
	cfh_size = sizeof(CONFIG_FILE_HEADER);
	memset(&cfh,0,cfh_size);
	cfh.ulLastShutdownStatus = FileReplData.Config.dwLastNormalShutdown;
    cfh.ulHeaderSize = cfh_size;

    filep = file_open("/etc/rtb.cnf",O_WRONLY | O_CREAT, 777);
	if (IS_ERR(filep))
	{
        printk("RTB: open config file error\n");
        goto out;
	}
        

    //write header
    if (file_write(filep, 0, (char *)&cfh, cfh_size) != cfh_size)
    {
        printk("RTB: write config file error\n");
        goto out;
    }
    
    writeoffset += cfh_size;
		
    cfh.ulOffsetSet = writeoffset;
    //write monitor set
    for (node = rb_first(&FileReplData.Config.MonitorSet.RBTree); node; node = rb_next(node))
    {
        pmse = rb_entry(node, MONITOR_SET_ENTRY, hdr.rbnode);
        printk("RTB: monitor set guid %d.\n",pmse->hdr.guidSetId.Data1);
        if(file_write(filep, writeoffset, (char *)pmse, sizeof(MONITOR_SET_ENTRY)) != sizeof(MONITOR_SET_ENTRY))
        {
            printk("RTB: write monitor set error\n");
            goto out;
        }
        writeoffset += sizeof(MONITOR_SET_ENTRY);
        cfh.ulSizeSet += sizeof(MONITOR_SET_ENTRY);
    }

    cfh.ulOffsetFiles = writeoffset;
    //write monitor item
    for (node = rb_first(&FileReplData.Config.MonitorFiles.RBTree); node; node = rb_next(node))
    {
        pmfe = rb_entry(node, MONITOR_FILE_ENTRY, hdr.rbnode);
        printk("RTB: monitor file %s\n",pmfe->wcsMonitorFile);
        if(file_write(filep, writeoffset, (char *)pmfe, sizeof(MONITOR_FILE_ENTRY)) != sizeof(MONITOR_FILE_ENTRY))
        {
            printk("RTB: write monitor item error\n");
            goto out;
        }
        writeoffset += sizeof(MONITOR_FILE_ENTRY);
        cfh.ulSizeFiles += sizeof(MONITOR_FILE_ENTRY);
    }


    //rewrite header
    if (file_write(filep, 0, (char *)&cfh, cfh_size) != cfh_size)
    {
        printk("RTB: rewrite config file error\n");
        goto out;
    }

out:

    if(!IS_ERR(filep))
        file_close(filep);

    return ret;
	
	

	
}

