#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <asm/errno.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/miscdevice.h>
#include <linux/semaphore.h> 

#define DEVICE_NAME "mypipe" // Kernel module name
#define ERROR -1 
#define SUCCESS 0
#define INIT_VAL 0
#define MAX_VAL 100

static int head = INIT_VAL;
static int tail = INIT_VAL; 
static int device_count = INIT_VAL;
static int MAX_BUFFER = MAX_VAL;
static char ** char_buffer; 

static struct miscdevice misc_device;
static struct file_operations fops;
int __init my_module_init(void);
void __exit my_module_exit(void);
static int device_open(struct inode *, struct file *);
static int device_close(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

module_param(MAX_BUFFER, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // pass MAX_BUFFER as module parameter 

static DEFINE_SEMAPHORE(mutex); // Declare semaphore mutex
static DEFINE_SEMAPHORE(empty); // Declare semaphore empty
static DEFINE_SEMAPHORE(full); // Declare semaphore full


static struct miscdevice misc_device = {
	
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &fops // pointer to struct file operation
};

/* struct "file_operation" from  "/linux/fs.h" - allowed file operations */ 
static struct file_operations fops = {

	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.read = device_read,
	.write = device_write
};

/* Device is initialized when module is inserted in kernel */
int __init my_module_init(void) {

	// Register device with kernel
	int rno = misc_register(&misc_device);
	if (rno < 0) {
		printk (KERN_INFO "My_module_init: Registeration failed.\n");
		return rno;
	}
	
	// Allocate memory for the device buffer when module is inserted
	char_buffer = (char**)kmalloc(MAX_BUFFER, GFP_KERNEL);
	if(char_buffer == NULL) {
		printk (KERN_INFO "My_module_init: Failed to allocate memory.\n");
		return -ENOMEM;	
	}
	sema_init(&mutex, 1); // initialize 1 to mutex
	sema_init(&empty, MAX_BUFFER); // initialize max buffer size to emoty
	sema_init(&full, INIT_VAL); // initialize 0 to full

	printk (KERN_INFO "My_module_init: Registeration success.\n");
	return rno;
}

/* Unregister the device when rmmod for the device is called */
void __exit my_module_exit(void) {
	
	kfree(char_buffer); // Free device buffer
	printk("My_module_exit: Device unregistered.\n");
	// Unregister the device
	misc_deregister(&misc_device);
}

/* Process opens device file example: open("/dev/mypipe", O_RDWR) */
static int device_open(struct inode *inode, struct file *filep) {
	
	device_count++;	// Identify number of times device is opened
	printk(KERN_INFO "Device_open: %d.\n", device_count);
	return SUCCESS;
}

/* Process closes device example: close(fd); */
static int device_close(struct inode *inodep, struct file *filep) {

	device_count--;	
	printk(KERN_INFO "Device_close: Device closed.\n");
	return SUCCESS;
}

/* Process attemps to read from the buffer */
static ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   		
	int retVal = 0;
	// Acquire lock: Decrement full and blocks, if (full == 0) until another process calls up(write operation)
	if(down_interruptible(&full)) {
		printk(KERN_INFO "Device_read: BUFFER EMPTY\n");
		return ERROR;	
	}
	// Acuire lock: Decrement mutex by 1 (only single process can read/ write at a time)
	if(down_interruptible(&mutex)) {
		printk(KERN_INFO "Device_read: Failed to acuire lock\n");
		up(&empty);
		return ERROR;
	}
	// Set head to 0 if head reaches end of the buffer
	if(head == MAX_BUFFER) {
		head = 0;
	}
	// Read from the buffer head and copy to user variable
	retVal = copy_to_user(buffer, char_buffer[head], len);
	if (retVal == 0){
		printk(KERN_INFO "Device_read: %s\n", char_buffer[head]);
		kfree(char_buffer[head]); // Free buffer slot
		head++;
	}
	else {
		printk(KERN_INFO "Device_read: Failed to read %d character(s)\n", retVal);
		retVal = -EFAULT;
	}
	// Increment semaphore (release lock) 
	up(&mutex);
	up(&empty);
	return retVal;	
}

/* Process attemps to write to the buffer */
static ssize_t device_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

	int retVal = 0;	
	// Acquire lock: Decrement empty and blocks, if (empty == max buffer size) until another process calls up(read operation)	
	if(down_interruptible(&empty)) {
		printk(KERN_INFO "Device_read: BUFFER FULL\n");
		return ERROR;	
	}
	// Acuire lock: Decrement mutex by 1 (only single process can read/ write at a time)
	if(down_interruptible(&mutex)) {
		printk(KERN_INFO "Device_write: Failed to acuire lock\n");
		up(&full);
		return ERROR;
	}
	// Set tail to 0 if tail reaches end of the buffer
	if(tail == MAX_BUFFER) {
		tail = 0;
	}
	// Allocate memory and assign 0
	char_buffer[tail] = (char*)kzalloc(len, GFP_KERNEL);
	if(char_buffer[tail] == NULL) {
		printk (KERN_INFO "Device_write: Failed to allocate memory");
		return -ENOMEM;	
	}
	// Read to buffer head and copy from user variable
	retVal = copy_from_user (char_buffer[tail] , buffer, len);
	if (retVal == 0){
		printk(KERN_INFO "Device_write: %s\n", buffer);
		tail++;
	}
	else {
		kfree(char_buffer[tail]); // Free slot if copy from user failed
		printk(KERN_INFO "Device_write: Failed to write %d character(s)\n", retVal);
		retVal = -EFAULT;
	}
	// Increment semaphore (release lock) 	
	up(&mutex);
	up(&full);
	return retVal;
}
module_init(my_module_init);
module_exit(my_module_exit);
MODULE_LICENSE("GPL");
