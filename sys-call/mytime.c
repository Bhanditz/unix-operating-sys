#include <linux/linkage.h>
#include <linux/export.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/printk.h>
#include <linux/slab.h>
asmlinkage int sys_mytime(struct timespec *current_time, int len)
{

	struct timespec cTime = current_kernel_time();

	if (copy_to_user(current_time, &cTime, len)) {
		printk(KERN_ALERT "Failure..!!\n");
		return -EFAULT;
	}
	printk(KERN_ALERT "Current time : %ld nanoseconds.\n", current_time->tv_nsec);	
	return 0;

}EXPORT_SYMBOL(sys_mytime);
