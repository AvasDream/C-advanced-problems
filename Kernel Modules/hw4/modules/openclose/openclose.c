#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h> // for file_operations struct

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven Inácio");
MODULE_DESCRIPTION("This driver creates a char device without any associated functions.");

static int driver_open(struct inode *dev_file, struct file *instance);
static int driver_close(struct inode *dev_file, struct file *instance);

static dev_t device_number;
static struct cdev *driver_object;
static struct class *driver_class;

static int count = 0;

#define MAX_DEVICES 257

//define all possible operations for drivers
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};

static int __init ModInit(void)
{
		int i = 0;
		// Register range of char device numbers
		// alloc_chrdev_region(dev_t* dev, unsigned baseminor,
		// unsigned count, const *char name)
		if ( alloc_chrdev_region(&device_number,0,257,"openclose")<0) {
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
		if (cdev_add(driver_object,device_number,257)) {
			printk(KERN_ALERT "Failed to register Device in Kernel\n");
			goto free_character_device;
		}
		//Create Entry in sysfs so udev creates char device
		driver_class = class_create( THIS_MODULE, "openclose");
		for(i = 0; i < MAX_DEVICES; i++) {
			device_create(driver_class,NULL,MKDEV(MAJOR(device_number), MINOR(device_number)+i),"%s","openclose%d", i);
		}

		printk(KERN_ALERT "Chardev registered\n");
		return 0;
		free_character_device:
			// void kobject_put (	struct kobject *  	kobj);
			// decrement refcount for kernelObject and if 0
			// call kobject_cleanup
			kobject_put(&driver_object->kobj);
		free_device_number:
			unregister_chrdev_region( device_number, MAX_DEVICES);
        return 0;
}

static void __exit ModExit(void)
{
		// delete sysfs entry
		int i;
		for(i = 0; i < MAX_DEVICES; i++) {
			device_destroy(driver_class, MKDEV(MAJOR(device_number), MINOR(device_number)+i));
		} 
		class_destroy(driver_class);
		// unregister driver
		cdev_del( driver_object);
		unregister_chrdev_region( device_number, 1);
}

module_init(ModInit);
module_exit(ModExit);

static int driver_open(struct inode *dev_file, struct file *instance){
	if(count >= MAX_DEVICES) {
		return -EBUSY;
	}
	count++;
	return 0;
}

static int driver_close(struct inode *dev_file, struct file *instance) {
	count--;
	return 0;
}

