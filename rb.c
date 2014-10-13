/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : rb.c
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
#include "rb.h"
#include "fruk.h"




MONITOR_SET_ENTRY * ms_search(struct rb_root *root, GUID guid)
{
  	struct rb_node *node = root->rb_node;
	int result;
	MONITOR_SET_ENTRY *data = NULL;
  	while (node) {
  		data = container_of(node, MONITOR_SET_ENTRY, hdr.rbnode);
		result = memcmp(&guid,&data->hdr.guidSetId,sizeof(GUID));
		if (result < 0)
  			node = node->rb_left;
		else if (result > 0)
  			node = node->rb_right;
		else
  			return data;
	}
	return NULL;
}


int ms_insert(struct rb_root *root, MONITOR_SET_ENTRY *data)
{
	int result;
  	struct rb_node **new = &(root->rb_node), *parent = NULL;
	MONITOR_SET_ENTRY *this = NULL;

  	/* Figure out where to put new node */
  	while (*new) {
  		this = container_of(*new, MONITOR_SET_ENTRY, hdr.rbnode);
		//result = memcmp(&(data->hdr.guidSetId.Data1),&(this->hdr.guidSetId.Data1),sizeof(GUID));
		result = memcmp(&(data->hdr.guidSetId),&(this->hdr.guidSetId),sizeof(GUID));
		parent = *new;
  		if (result < 0)
  			new = &((*new)->rb_left);
  		else if (result > 0)
  			new = &((*new)->rb_right);
  		else
  			return -1;
  	}

  	/* Add new node and rebalance tree. */
  	rb_link_node(&data->hdr.rbnode, parent, new);
  	rb_insert_color(&data->hdr.rbnode, root);
	printk("insert success\n");
	return 0;
}

PMONITOR_SET_ENTRY ms_remove(struct rb_root *root, GUID guid)
{
	PMONITOR_SET_ENTRY pmse = ms_search(root,guid);
	if(pmse)
	{
		rb_erase(&pmse->hdr.rbnode,root);

	}

	return pmse;
}

MONITOR_FILE_ENTRY * mi_search(struct rb_root *root, char * pathname)
{
  	struct rb_node *node = root->rb_node;
	int result;
	MONITOR_FILE_ENTRY *data = NULL;
  	while (node) {
  		data = container_of(node, MONITOR_FILE_ENTRY, hdr.rbnode);
		//result = memcmp(guid,data->hdr.guidSetId,sizeof(GUID));
		result = strncmp(pathname,data->wcsMonitorFile,strlen(data->wcsMonitorFile));
		if (result < 0)
  			node = node->rb_left;
		else if (result > 0)
  			node = node->rb_right;
		else
  			return data;
	}
	return NULL;
}

PMONITOR_FILE_ENTRY mi_remove(struct rb_root *root, char *pathname)
{
	PMONITOR_FILE_ENTRY pmfe = NULL;
	pmfe = mi_search(root,pathname);
	if(pmfe)
	{
		rb_erase(&pmfe->hdr.rbnode,root);
	}

	return pmfe;
		
}


int mi_insert(struct rb_root *root, MONITOR_FILE_ENTRY *data)
{
	int result;
  	struct rb_node **new = &(root->rb_node), *parent = NULL;
	MONITOR_FILE_ENTRY *this = NULL;
  	/* Figure out where to put new node */
  	while (*new) {
  		this  = container_of(*new, MONITOR_FILE_ENTRY, hdr.rbnode);
		result = strncmp(data->wcsMonitorFile,this->wcsMonitorFile,strlen(this->wcsMonitorFile));
		parent = *new;
  		if (result < 0)
  			new = &((*new)->rb_left);
  		else if (result > 0)
  			new = &((*new)->rb_right);
  		else
  			return -1;
  	}

  	/* Add new node and rebalance tree. */
  	rb_link_node(&data->hdr.rbnode, parent, new);
  	rb_insert_color(&data->hdr.rbnode, root);

	return 0;
}





