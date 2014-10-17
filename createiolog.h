/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : createiolog.h
 * Author        : lb
 * Date          : 2014-09-24
 * Description   : write io log info to files.
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-09-24
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/

/*****************************************************************************
 * Function      : getseqno
 * Description   : get seq number
 * Input          : None
 * Output        : None
 * Return        : 
 * Others        : 
 * Record
 * 1.Date        : 20140924
 *   Author      : lb
 *   Modification: Created function

*****************************************************************************/
#ifndef _CREATEIOLOG_H_
#define _CREATEIOLOG_H_

#include "module.h"
#include "fileop.h"



ulonglong getseqno(void);


int createiologformkdir(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath,int mode);

int createiologforcreatefile(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *abspath,int mode);

int createiologforrmdir(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs, char *iologdir,char *abspath);

int createiologforrmfile(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *abspath);

int createiologforrename(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *oldabspath, char *newabspath);

int createiologforcreatesymlink(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *symlinkabspath, char *dstabspath);

int createiologforcreatelink(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *linkabspath, char *dstabspath);

int createiologforwrite(ulonglong ullSeqNo, ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir,char *abspath,char *writebuff, size_t count, loff_t *pos);

int createiologforerror(ulonglong ullSeqNo,ulonglong ullGlobalSeqNo, ulonglong timesecs,char *iologdir, char *abspath);
#endif

