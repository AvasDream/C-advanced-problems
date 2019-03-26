#include <linux/init.h> //macros
#include <linux/module.h>
#include <linux/cdev.h> // register dev
#include <linux/device.h> //device object
#include <linux/fs.h> //filesystem
#include <asm/uaccess.h> // User access memory functions
//#include <asm/signal.h> tried to implement signal handling
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vincent Grimmeisen");
MODULE_DESCRIPTION("Module myzero.\n");

static dev_t device_number;
static struct cdev *driver_object;
static struct class *driver_class;
static struct device *device_object;


static int driver_open( struct inode *driver_file, struct file *instance )
{
	return 0;
}

static int driver_close( struct inode *driver_file, struct file *instance )
{
	return 0;
}
// Random number of zeros can be read.
static ssize_t read_zero(struct file *file, char *user,
		 	size_t count, loff_t *offset) {
	//declaration in for loop only allowed in C99 & C11 (C variants)
	size_t bytes = 0;
	for (; bytes < count; bytes++) {
		//write zero at address (user + bytes)
		//signal(SIGINT, driver_close);
		put_user('\0',user + bytes);
	}
	// set new offset
	*offset += bytes;
	// return written bytes
 	return bytes;
}

// No need to change something writes should get lost
// writes simply succeed
static ssize_t write_zero(struct file *file, const char __user *buf,
			  size_t count, loff_t *ppos) {
	return count;
}


//define all possible operations for drivers
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read= read_zero,
	.write= write_zero,
	.open= driver_open,
	.release= driver_close,
};



static int __init ModInit(void)
{
		// Register range of char device numbers
		// alloc_chrdev_region(dev_t* dev, unsigned baseminor,
		// unsigned count, const *char name)
		if ( alloc_chrdev_region(&device_number,0,1,"myzero")<0) {
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
		driver_class = class_create( THIS_MODULE, "myzero");
		if (IS_ERR( driver_class )) {
		pr_err( "myzero: class_create failed in init\n");
		goto free_class;
		}
		device_object = device_create(driver_class,NULL,device_number,"%s","myzero");
		if (IS_ERR( device_object)) {
		pr_err( "myzero: device_create failed in init\n");
		goto free_device_number;
		}
		return 0;
		free_character_device:
			// void kobject_put (	struct kobject *  	kobj);
			// decrement refcount for kernelObject and if 0
			// call kobject_cleanup
			kobject_put(&driver_object->kobj);
		free_device_number:
			unregister_chrdev_region( device_number, 1);
		free_class:
			class_destroy( driver_class );
		return -EIO;
}

static void __exit ModExit(void)
{
		// delete sysfs entry
		device_destroy(driver_class, device_number);
		class_destroy(driver_class);
		// unregister driver
		cdev_del( driver_object);
		unregister_chrdev_region( device_number, 1);
        printk(KERN_ALERT "Goodbye from myzero\n");
}

module_init(ModInit);
module_exit(ModExit);
