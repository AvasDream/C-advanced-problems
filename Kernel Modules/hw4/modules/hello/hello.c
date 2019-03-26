#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h> // for file_operations struct
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vincent Grimmeisen");
MODULE_DESCRIPTION("Module to return a string when driver is opened\n");

static dev_t device_number;
static struct cdev *driver_object;
static struct class *driver_class;
static struct device *device_object;

static char hello[]="Hello from Group 20\n";

static int driver_open( struct inode *driver_file, struct file *instance )
{
	printk(KERN_NOTICE "Driver open\n");
	return 0;
}

static int driver_close( struct inode *driver_file, struct file *instance )
{
	printk(KERN_NOTICE "Driver close called\n");
	return 0;
}


static ssize_t read_hello( struct file *instanz, char __user *user,
		size_t count, loff_t *offset )
{
	unsigned long not_copied, to_copy;

	to_copy = strlen(hello)+1;

	if(*offset >= to_copy) {
		printk(KERN_NOTICE "Nothing more to print\n");
		return 0;
	}

	if(to_copy > count) {
		to_copy = count;
	}

	not_copied=copy_to_user(user,hello,to_copy);
	*offset += to_copy-not_copied;
	return to_copy-not_copied;
}

//define all possible operations for drivers
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read= read_hello,
	.open= driver_open,
	.release= driver_close,
};



static int __init ModInit(void)
{
		// Register range of char device numbers
		// alloc_chrdev_region(dev_t* dev, unsigned baseminor,
		// unsigned count, const *char name)
		if ( alloc_chrdev_region(&device_number,0,1,"hello")<0) {
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
		driver_class = class_create( THIS_MODULE, "hello");
		if (IS_ERR( driver_class )) {
		pr_err( "hello: class_create failed in init\n");
		goto free_class;
		}
		device_object = device_create(driver_class,NULL,device_number,"%s","hello");
		if (IS_ERR( device_object)) {
		pr_err( "hello: device_create failed in init\n");
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
        printk(KERN_ALERT "Goodbye from hello\n");
}

module_init(ModInit);
module_exit(ModExit);
