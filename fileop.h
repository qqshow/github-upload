#ifndef _FILEOP_H_
#define _FILEOP_H_

struct file* file_open(const char* path, int flags, int rights);

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);

int file_sync(struct file* file);

void file_close(struct file* file);


#endif /* _FILEOP_H_ */
