#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
asmlinkage int sys_Crash5(void)
{
	printk(KERN_ALERT "5. Trying system crash: Other \n");
	sys_Crash5();
	return 0;
}EXPORT_SYMBOL(sys_Crash5);
