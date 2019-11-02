#include <asm/unistd.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/pgtable_types.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zin");


#define START_MEM	PAGE_OFFSET
#define END_MEM		ULLONG_MAX

unsigned long long *system_call_table_addr;

/*custom syscall*/
asmlinkage int (*original_open) (const char *filename, int flags, int mode);
asmlinkage ssize_t (*original_write) (int fd, const char *buf, size_t count);

/*Hook function - Do desired action here*/
asmlinkage int open_hook(const char *filename, int flags, int mode){
	printk(KERN_INFO "OPEN HOOK FUNCTION IS IN\n");
	printk(KERN_INFO "filename: %s", filename);
	printk(KERN_INFO "filename: %s", *filename);
	printk(KERN_INFO "flags: %i", flags);
	printk(KERN_INFO "mode: %i", mode);
	printk(KERN_INFO "OPENED FILE: %d\n", filename);

	return (*original_open)(filename, flags, mode);

}

asmlinkage ssize_t write_hook(int fd, const char *buf, size_t count){
	printk(KERN_INFO "WRITE HOOK FUNCTION IS IN\n");
	printk(KERN_INFO "fd: %d", fd);
	printk(KERN_INFO "buf: %s", buf);
	printk(KERN_INFO "size: %d", count);

	return (*original_write)(fd, buf, count);
}

static int __init entry_point(void){
	printk(KERN_INFO "HOOK LOADED Successfully...");
	/*MY sys_call_table address*/
	system_call_table_addr=(void *) kallsyms_lookup_name("sys_call_table");

	/* Replace custom syscall with the correct system call name (open) to hook*/
	write_cr0 (read_cr0 () & (~ 0x10000));
	original_open = system_call_table_addr[__NR_open];
	original_write = system_call_table_addr[__NR_write];
	/*Change syscall to our syscall function*/
	system_call_table_addr[__NR_open] = open_hook;
//	system_call_table_addr[__NR_write]= write_hook;
	return 0;
}

static void __exit exit_point(void){
	printk(KERN_INFO "HOOK UNLOADED Successfully...");
	/*Restore original system call */
	system_call_table_addr[__NR_open] = original_open;
//	system_call_table_addr[__NR_write] = original_write;
	write_cr0 (read_cr0 () | 0x10000);
	return;
}

module_init(entry_point);
module_exit(exit_point);
