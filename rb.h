/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : rb.h
 * Author        : lb
 * Date          : 2014-10-10
 * Description   : .C file function description
 * Version       : 1.0
 * Function List :
 * 
 * Record        :
 * 1.Date        : 2014-10-10
 *   Author      : lb
 *   Modification: Created file

*************************************************************************************************************/
#ifndef _RB_H_12345
#define _RB_H_12345

#include "module.h"
#include "fruk.h"
#include <linux/rbtree.h>



MONITOR_SET_ENTRY * ms_search(struct rb_root *root, GUID guid);
int ms_insert(struct rb_root *root, MONITOR_SET_ENTRY *data);
PMONITOR_SET_ENTRY ms_remove(struct rb_root *root, GUID guid);
MONITOR_FILE_ENTRY * mi_search(struct rb_root *root, char * pathname);
PMONITOR_FILE_ENTRY mi_remove(struct rb_root *root, char *pathname);
int mi_insert(struct rb_root *root, MONITOR_FILE_ENTRY *data);

#endif

