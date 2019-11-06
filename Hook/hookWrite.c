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
#include <linux/fs.h>
#include <linux/fdtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("1712206");

unsigned long **system_call_table_addr;

/*custom syscall*/
asmlinkage ssize_t (*original_write)(unsigned int fd, const void *buf, size_t count);

/*Hook function - Do desired action here*/
asmlinkage ssize_t new_write(unsigned int fd, const void *buf, size_t count){
	char *tmp;
	char *pathname;
	struct file *file;
	struct path *path;

	spin_lock(&current->files->file_lock);
	file = fcheck_files(current->files, fd);
	if (!file) {
		spin_unlock(&current->files->file_lock);
		return -ENOENT;
	}

	path = &file->f_path;
	path_get(path);
	spin_unlock(&current->files->file_lock);

	tmp = (char *)__get_free_page(GFP_KERNEL);

	if (!tmp) {
		path_put(path);
		return -ENOMEM;
	}

	pathname = d_path(path, tmp, PAGE_SIZE);
	path_put(path);

	if (IS_ERR(pathname)) {
		free_page((unsigned long)tmp);
		return PTR_ERR(pathname);
	}

	int bytes= original_write(fd,buf,count);

	if (strstr(pathname,"a.txt")==0) {
		printk(KERN_INFO "Calling process: %s\n", current->comm);
		printk(KERN_INFO "Write to: %s\n", pathname);
		printk(KERN_INFO "Bytes written: %d\n", bytes);
	}
	free_page((unsigned long)tmp);

	return bytes;
}

/*Make page writeable*/
int make_rw(unsigned long address){
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	if(pte->pte &~_PAGE_RW){
		pte->pte |=_PAGE_RW;
	}
	return 0;
}

/* Make the page write protected */
int make_ro(unsigned long address){
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	pte->pte = pte->pte &~_PAGE_RW;
	return 0;
}

static int __init entry_point(void){
	/*MY system_call_table_addr address*/
	system_call_table_addr=(void *) kallsyms_lookup_name("sys_call_table");

	/* Replace custom syscall with the correct system call name (open) to hook*/
	original_write = (void*)system_call_table_addr[__NR_write];

	/*Disable page protection*/
	make_rw((unsigned long)system_call_table_addr);

	/*Change syscall to our syscall function*/
	system_call_table_addr[__NR_write] = (unsigned long*)new_write;

	return 0;
}

static void __exit exit_point(void){
	/*Restore original system call */
	system_call_table_addr[__NR_write] = (unsigned long*)original_write;

	/*Renable page protection*/
	make_ro((unsigned long)system_call_table_addr);
	return;
}

module_init(entry_point);
module_exit(exit_point);
