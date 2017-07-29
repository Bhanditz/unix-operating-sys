#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
asmlinkage int sys_Crash2(void)
{
	printk(KERN_ALERT "2. Trying sys crash: Dereferencing NULL pointer\n");
	int* ptr = NULL;
	int* i;
	i = ptr;
	*ptr = 10;	
	return 0;
}EXPORT_SYMBOL(sys_Crash2);
