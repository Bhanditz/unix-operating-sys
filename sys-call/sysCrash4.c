
#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
asmlinkage int sys_Crash4(void)
{
	printk(KERN_ALERT "4. Trying system crash: Using c routine library\n");
	printf("Cannot use c routine library\n");
	return 0;
}EXPORT_SYMBOL(sys_Crash4);
