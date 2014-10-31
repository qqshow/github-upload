/********************************************************************************

 **** Copyright (C), 2014, unary Co., Ltd.                ****

 ********************************************************************************
 * File Name     : iowritethread.c
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
#include "iowritethread.h"
#include "module.h"
#include "createiolog.h"


static int tc = 0;
static struct completion  comp;  

void initcompletion(void)
{
    return init_completion(&comp);
}


void completecompletion(void)
{
    return complete(&comp);
}



int thread_iowrite(void *data)  
{  

    PIOWRITE_CONTEXT piowc = NULL;
    printk(KERN_INFO "RTB: thread_iowrite..........\n");
    do {
            wait_for_completion_timeout(&comp,1000);
            //complete(&comp);

            //msleep_interruptible(1000);
            while(!list_empty(&FileReplData.Config.iowritequeue))
            {           
                spin_lock( &FileReplData.Config.iowritequeuelock);
                piowc = list_first_entry(&FileReplData.Config.iowritequeue, IOWRITE_CONTEXT, entry);
                list_del(&piowc->entry);
                spin_unlock( &FileReplData.Config.iowritequeuelock);
                if(piowc != NULL && piowc != &FileReplData.Config.iowritequeue)
                {
                    createiolog(piowc->iologpath, piowc->logfile, piowc->logfile->hdr.ulLogSize);
         
                }

                if(piowc)
                {
                    if(piowc->iologpath)
                        kfree(piowc->iologpath);
                    if(piowc->logfile)
                        kfree(piowc->logfile);

                    kfree(piowc);
                }
            }
                  

    }while(!kthread_should_stop());  

    return tc;  

} 