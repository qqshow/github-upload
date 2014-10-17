#include "module.h"
#include "fruk.h"
#include "usrlink.h"

#define NTSTATUS int


int checkneedtolog(char *path);
int InitMonitorSet(void);

 
int UninitMonitorSet(void);

 
PMONITOR_SET_ENTRY MonitorSetEntryAlloc(void);

    
void MonitorSetEntryFree(
						  PMONITOR_SET_ENTRY pmse
						  );

 
PMONITOR_SET_ENTRY GetMonitorSetEntry(
						GUID guidSet
						);

PMONITOR_FILE_ENTRY GetMonitorFileEntryByabspath(char *abspath);
 
int AddMonitorSet(MONITOR_SET_ENTRY *pSet,BOOL bDelExist);


int DelMonitorSet(MONITOR_SET_ENTRY *pSet,BOOL bNeedSync);

 
int ConfigAddMonitorSet(PFILEREPL_NOTIFICATION pfn,
                    BOOL bDelExistSet);

 
int ConfigDelMonitorSet(PFILEREPL_NOTIFICATION pfn);

int ConfigDelMonitorItem(PFILEREPL_NOTIFICATION pfn);

int ConfigDelAllMonitorSet();

int GetMonitorFileSeqNoByMonitorFileEntry(PMONITOR_FILE_ENTRY pmfe,
						ULONGLONG* pullSeqNo,  ULONGLONG* pullSetSeqNo ,ULONGLONG *timesecs,char *iologdir);

int SaveConfig();

int LoadConfig();

