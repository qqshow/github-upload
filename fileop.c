#include "Module.h"
#include <asm/segment.h>
#include <linux/buffer_head.h>
#include <linux/unistd.h>
#include "fileop.h"



struct file* file_open(const char* path, int flags, int rights)
{
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
    	err = PTR_ERR(filp);
    }
    return filp;
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &(offset));
	//ret = file->f_op->read(file, data, size, &(offset));
	
    set_fs(oldfs);
    return ret;
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());
	printk("file_write  f_flags %d\n",file->f_flags);
	
    ret = vfs_write(file, data, size, &(offset));
    
    set_fs(oldfs);
    return ret;
}

int file_sync(struct file* file)
{
    //file_fsync(file, file->f_dentry, 0);
    return 0;
}

void file_close(struct file* file)
{
    filp_close(file, NULL);
}
