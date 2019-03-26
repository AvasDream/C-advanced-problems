#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

// for file_operations struct

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven Inácio");
MODULE_DESCRIPTION("This driver creates a bufferdevice to temporarily store data.");

typedef struct mybuffer {
	char *buffer;
	size_t length;
} mybuffer_t;

static dev_t device_number;
static struct cdev *driver_object;
static struct class *driver_class;
static struct device *device_object;
/* static int write_count=0; */

static mybuffer_t *buffer;

DECLARE_WAIT_QUEUE_HEAD(wq_read);

static int driver_open(struct inode *dev_file, struct file *instance);
static int driver_close(struct inode *dev_file, struct file *instance);
static ssize_t driver_read(struct file *instance, char *user, size_t count, loff_t *offset);
static ssize_t driver_write(struct file *instance, const char *user, size_t count, loff_t *offs);

#define MAX_BUFFER_SIZE 512
#define BUFFER_FILLED (buffer->length!=0)

//define all possible operations for drivers
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};

static int __init ModInit(void)
{
		if((buffer = (mybuffer_t*)kmalloc(sizeof(buffer), GFP_KERNEL))<0) {
			printk(KERN_ALERT "Failed to allocate buffer\n");
			goto free_struct;
		} 
		if((buffer->buffer = (char*)kcalloc(MAX_BUFFER_SIZE, sizeof(char), GFP_KERNEL))<0) {
			printk(KERN_ALERT "Failed to allocate storage\n");
			goto free_buffer;
		}
		buffer->length = 0;
		// Register range of char device numbers
		// alloc_chrdev_region(dev_t* dev, unsigned baseminor,
		// unsigned count, const *char name)
		if ( alloc_chrdev_region(&device_number,0,1,"mybuffer")<0) {
			// EIO is a macro for input/output error
			printk(KERN_ALERT "Failed to register device number\n");
			return -EIO;
		}
		// Memory allocation for driver object
		driver_object = cdev_alloc();
		if (driver_object == NULL) {
			printk(KERN_ALERT "Failed to allocate driver object\n");
			goto free_device_number;
		}
		//Define owner of object
		driver_object->owner = THIS_MODULE;
		//Define Operations permitted for Object
		// In this case none, see $fops
		driver_object->ops = &fops;


		//cdev_add(struct dev* dev, dev_t number, unsigned count)
		// Regsiter char device in kernel
		if (cdev_add(driver_object,device_number,1)) {
			printk(KERN_ALERT "Failed to register Device in Kernel\n");
			goto free_character_device;
		}
		//Create Entry in sysfs so udev creates char device
		driver_class = class_create( THIS_MODULE, "mybuffer");
		device_object = device_create(driver_class,NULL,device_number,"%s","mybuffer");

		printk(KERN_ALERT "MyBuffer registered\n");
		return 0;
		free_character_device:
			// void kobject_put (	struct kobject *  	kobj);
			// decrement refcount for kernelObject and if 0
			// call kobject_cleanup
			kobject_put(&driver_object->kobj);
		free_device_number:
			unregister_chrdev_region( device_number, 1);
		free_buffer:
			kfree(buffer->buffer);
		free_struct:
			kfree(buffer);
        return 0;
}

static void __exit ModExit(void)
{
		// free buffer
		kfree(buffer->buffer);
		kfree(buffer);
		// delete sysfs entry
		device_destroy(driver_class, device_number);
		class_destroy(driver_class);
		// unregister driver
		cdev_del( driver_object);
		unregister_chrdev_region( device_number, 1);
}

module_init(ModInit);
module_exit(ModExit);


static int driver_open(struct inode *dev_file, struct file *instance){
/* 	if (instance->f_flags&O_WRONLY && write_count > 0) {
		return -EBUSY;
	}
	else if (instance->f_flags&O_WRONLY) {
		write_count++;
		return 0;
	} 
	else if (instance->f_flags&O_RDONLY) {
		return 0;
	}
	else if (instance->f_flags&O_RDWR) {
		if (write_count > 0){
			return -EBUSY;
		} 
		write_count++;
		return 0;
	} */
	return 0;
}

static int driver_close(struct inode *dev_file, struct file *instance) {
/* 	if (instance->f_flags&O_WRONLY) {
		write_count--;
	} */
	return 0;
}

static ssize_t driver_read(struct file *instance, char *user, size_t count, loff_t *offset) {
	int to_copy;
	if(wait_event_interruptible(wq_read, BUFFER_FILLED)){
		return -ERESTARTSYS;
	} 
	to_copy = min((size_t)(buffer->length-(*offset)), count);
	if(copy_to_user(user, buffer->buffer, to_copy)) {
		return -EFAULT;
	} 
	(*offset) += to_copy;
	return to_copy;
}

static ssize_t driver_write(struct file *instance, const char *user, size_t count, loff_t *offs) {
	int to_copy, not_copied;
	to_copy = min(count, (size_t)MAX_BUFFER_SIZE);
	not_copied = copy_from_user(buffer->buffer, user, to_copy);
	buffer->length = to_copy-not_copied; // size of actually written bytes
	wake_up_interruptible(&wq_read);
	return to_copy-not_copied;
}