#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
asmlinkage int sys_Crash1(void)
{
	int num  = 1;	
	printk(KERN_ALERT "1. Trying sys crash: divide by zero\n");
	return num/0;
}EXPORT_SYMBOL(sys_Crash1);
