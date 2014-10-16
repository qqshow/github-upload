#include "module.h"
#include "fruk.h"
#include "usrlink.h"

#define NTSTATUS int

 
int InitMonitorSet(void);

 
int UninitMonitorSet(void);

 
PMONITOR_SET_ENTRY MonitorSetEntryAlloc(void);

    
void MonitorSetEntryFree(
						  PMONITOR_SET_ENTRY pmse
						  );

 
PMONITOR_SET_ENTRY GetMonitorSetEntry(
						GUID guidSet
						);

int checkneedtolog(char *abspath);
 
int AddMonitorSet(MONITOR_SET_ENTRY *pSet,BOOL bDelExist);


int DelMonitorSet(MONITOR_SET_ENTRY *pSet,BOOL bNeedSync);

 
int ConfigAddMonitorSet(PFILEREPL_NOTIFICATION pfn,
                    BOOL bDelExistSet);

 
int ConfigDelMonitorSet(PFILEREPL_NOTIFICATION pfn);

int ConfigDelMonitorItem(PFILEREPL_NOTIFICATION pfn);

int ConfigDelAllMonitorSet();

int SaveConfig();

int LoadConfig();