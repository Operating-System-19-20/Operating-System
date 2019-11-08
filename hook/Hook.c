#include <linux/init.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>

MODULE_AUTHOR("1712162 - 1712329");
MODULE_DESCRIPTION("Hook system call open and write");

unsigned long **syscall_table;

asmlinkage int (*syscall_open)(const char __user *, int, mode_t);
asmlinkage int (*syscall_write)(unsigned int, const char __user *, size_t);

asmlinkage int custom_open(const char __user *pathname, int flags, mode_t mode)
{
	printk(KERN_INFO "HOOK ON OPEN SYSCALl!!! \n");
	printk(KERN_INFO "Process's name: %s\n", current->comm);
	printk(KERN_INFO "Filename: %s\n", pathname);

	return (*syscall_open)(pathname, flags, mode);
}

asmlinkage int custom_write(unsigned int fd, const char __user *buf, size_t count)
{
	int countBytes;

	printk(KERN_INFO "HOOK ON WRITE SYSCALL!!! \n");
	printk(KERN_INFO "Process's name: %s\n", current->comm);

	countBytes = (*syscall_write)(fd, buf, count);
	printk(KERN_INFO "Number of bytes: %d\n, ", countBytes);

	return countBytes;
}

static void get_syscall_table(void)
{
	for(unsigned long int i = PAGE_OFFSET ; i < ULLONG_MAX; i += sizeof(void *)) {
		unsigned long **temp = (unsigned long **)i;
		if (temp[__NR_close] ==
		    (unsigned long *)sys_close) {
			syscall_table = temp;
			return;
		}
	}
	syscall_table = NULL;
}

static int entry_point(void)
{
	printk(KERN_INFO "Module init\n");

	get_syscall_table();
	if (syscall_table == NULL) {
		printk(KERN_ERR "Can not found syscall table\n");
		return -1;
	}
	syscall_open = (void *)syscall_table[__NR_open];
	syscall_write = (void *)syscall_table[__NR_write];

	write_cr0(read_cr0() & ~0x10000);

	syscall_table[__NR_open] = (unsigned long *)custom_open;
	syscall_table[__NR_write] = (unsigned long *)custom_write;

	write_cr0(read_cr0() | 0x10000);
	return 0;
}

static void exit_point(void)
{
	printk(KERN_INFO "Module exit\n");

	if (syscall_table != NULL) {
		write_cr0(read_cr0() & ~0x10000);

		syscall_table[__NR_open] = (unsigned long *)syscall_open;
		syscall_table[__NR_write] = (unsigned long *)syscall_write;

		write_cr0(read_cr0() | 0x10000);
	}
}

module_init(entry_point);
module_exit(exit_point);