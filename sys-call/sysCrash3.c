#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
asmlinkage int sys_Crash3(void)
{
	printk(KERN_ALERT "3. Trying sys crash: Returning negative value\n");
	return -1;	
}EXPORT_SYMBOL(sys_Crash3);
