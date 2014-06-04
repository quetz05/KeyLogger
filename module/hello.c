#include <linux/module.h>	
#include <linux/kernel.h>

#include <asm/unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <asm/fcntl.h>
#include <asm/errno.h>
#include <linux/types.h>
#include <linux/dirent.h> 
#include <sys/mman.h> 
#include <linux/string.h> 
#include <linux/fs.h> 
#include <linux/malloc.h> 	

MODULE_AUTHOR("Marcin");
MODULE_DESCRIPTION("Modul, ktory wcale nie ukrywa keylogera");

extern void* sys_call_table[]; 

int init_module(void)
{
	printk(KERN_INFO "inicjalizujemy hide modul\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "cleanupujemy hide modul\n");
}
