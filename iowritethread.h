/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : iowritethread.h
 * Author        : lb
 * Date          : 2014-10-30
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-10-30
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/
#ifndef _IOWRITETHREAD_H_
#define _IOWRITETHREAD_H_

#include <linux/kthread.h>

int thread_iowrite(void *data);

void initcompletion(void);

void completecompletion(void);


#endif
