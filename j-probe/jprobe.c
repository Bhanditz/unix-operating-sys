#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/major.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/miscdevice.h>
#include <linux/moduleparam.h>

#define INIT 0
#define MAX_VAL 500 // BUffer max size
#define QUERY_GET_VALUE 1
#define DEVICE_NAME "jprobe" // Kernel module name

static int seek = INIT; // Initial Value = 0
static int PROCESS_ID = INIT; // Initial value = 0
static int MAX_BUFFER = MAX_VAL; // Max value = 500

typedef struct {
	struct timespec cTime;
	long virtual_addr;

}ProcessInfo;

// Array of Struct Process Info
ProcessInfo* processInfo;

// pass PROCESS_ID as module parameter 
module_param(PROCESS_ID, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 

// ioctl Defination
static long my_ioctl(struct file*, unsigned int, unsigned long);

/* struct "file_operation" from  "/linux/fs.h" - allowed file operations */ 
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = my_ioctl
};

static struct miscdevice misc_device = {
	
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME, // "jprobe"
	.fops = &fops // pointer to struct file operation
};

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	int retVal = -1;
	switch(cmd)
	{
		case QUERY_GET_VALUE: 	// Copy buffer to user variable
		retVal = copy_to_user((ProcessInfo*)arg, processInfo, MAX_BUFFER * sizeof(ProcessInfo));
		if (retVal == 0){
			printk(KERN_INFO "my_ioct: Copied successful.\n");
		}
		else {
			printk(KERN_INFO "my_ioct: Failed to copy.\n");
			retVal = -1;
		}
		break;
	}
	return retVal;
}

/* Proxy routine having the same arguments as actual handle_mm_fault() routine */
static int j_handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,
                     unsigned long address, unsigned int flags) {

	if(current->pid == PROCESS_ID) {
		// Initialize to 0
		if(seek == MAX_BUFFER) { seek = INIT; } 
		processInfo[seek].cTime = current_kernel_time(); // Assign current time
		processInfo[seek].virtual_addr = address; // Fault address
		seek++;	
		printk(KERN_INFO "j_handle_mm_fault: %ld.%ld \t %lx\n", 
			processInfo[seek].cTime.tv_sec, processInfo[seek].cTime.tv_nsec, processInfo[seek].virtual_addr);
		}		
	jprobe_return();
	return 0;
}
 
static struct jprobe my_jprobe = {
	.entry	= j_handle_mm_fault,
	.kp = {
		.symbol_name    = "handle_mm_fault",
	},
};
 
static int __init jprobe_init(void) {
	int ret, rno;
	
	// Register device with kernel
	rno = misc_register(&misc_device);
	if (rno < 0) {
		printk (KERN_INFO "__init jprobe_init: Registeration failed.\n");
		return rno;
	}

	// Register jprobe with kernel
	ret = register_jprobe(&my_jprobe);
	if (ret < 0) {
		misc_deregister(&misc_device); 	// Unregister the device
		printk(KERN_INFO "__init jprobe_init: register_jprobe failed, returned %d\n", ret);
		printk(KERN_INFO "__init jprobe_init: Device unregistered.\n");
		return ret;
	}

	// Allocate memory for the process info buffer when initialized
	processInfo = (ProcessInfo*)kzalloc(MAX_BUFFER * sizeof(ProcessInfo), GFP_KERNEL);
	if(processInfo == NULL) {
		unregister_jprobe(&my_jprobe); 	// Unregister jprobe
		misc_deregister(&misc_device); 	// Unregister the device
		printk (KERN_INFO "__init jprobe_init: Failed to allocate memory.\n");
		printk(KERN_INFO "__init jprobe_init: jprobe at %p unregistered\n", my_jprobe.kp.addr);
		printk(KERN_INFO "__init jprobe_init: Device unregistered.\n");
		return -1;	
	}
	printk (KERN_INFO "__init jprobe_init: Registeration success.\n");
	printk(KERN_INFO "__init jprobe_init: Planted jprobe at %p, handler addr %p\n",
		my_jprobe.kp.addr, my_jprobe.entry);
	return 0;	
}
 
static void __exit jprobe_exit(void) {

	kfree(processInfo); // Free device buffer
	unregister_jprobe(&my_jprobe); 	// Unregister jprobe
	misc_deregister(&misc_device); 	// Unregister the device
	printk(KERN_INFO "__exit jprobe_exit: jprobe at %p unregistered\n", my_jprobe.kp.addr);
	printk(KERN_INFO "__exit jprobe_exit: Device unregistered.\n");
}
module_init(jprobe_init)
module_exit(jprobe_exit)
MODULE_LICENSE("GPL");
