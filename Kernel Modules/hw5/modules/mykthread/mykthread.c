#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kthread.h>
//For kill etc
#include <linux/types.h>
#include <linux/sched/signal.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vincent Grimmeisen");
MODULE_DESCRIPTION("This driver creates a kernel thread.");

static dev_t device_number;
static struct cdev *driver_object;
static struct class *driver_class;
static struct device *device_object;

// global struct for created thread
static struct task_struct *thread_id;
//Waitqueue
static wait_queue_head_t wq;
// Flag for completion
static DECLARE_COMPLETION( on_exit );


//define all possible operations for drivers
static struct file_operations fops = {
	.owner = THIS_MODULE,
};

static int print_messages(void *data) {
	unsigned long timeout;
	int i;
	allow_signal( SIGTERM );
	for (i = 0; i < 3 && (kthread_should_stop() == 0); i++) {
		timeout = HZ * 2; // HZ = 1 second
		timeout = wait_event_interruptible_timeout( wq, (timeout == 0), timeout);
		printk("Hello from Thread: %d \n", current->pid);
	}
	complete_and_exit(&on_exit, 0);
}

static int __init ModInit(void)
{
		// Register range of char device numbers
		// alloc_chrdev_region(dev_t* dev, unsigned baseminor,
		// unsigned count, const *char name)
		if ( alloc_chrdev_region(&device_number,0,1,"mykthread")<0 ) {
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
		driver_class = class_create( THIS_MODULE, "mykthread");
		device_object = device_create(driver_class,NULL,device_number,"%s","mykthread");

		printk(KERN_INFO "Mykthread registered\n");
// Code above is unecessary for this module but still best practice.

		init_waitqueue_head(&wq);
		thread_id=kthread_create(print_messages, NULL, "mykernelthread");
		if (thread_id == 0) {
			return -EIO;
		}
		wake_up_process( thread_id );
		return 0;
		free_character_device:
			// void kobject_put (	struct kobject *  	kobj);
			// decrement refcount for kernelObject and if 0
			// call kobject_cleanup
			kobject_put(&driver_object->kobj);
		free_device_number:
			unregister_chrdev_region( device_number, 1);
        return 0;
}

static void __exit ModExit(void)
{
		//Firste ensure that the thread is completed.
		kill_pid( task_pid(thread_id), SIGTERM, 1 );
		wait_for_completion( &on_exit );
		// delete sysfs entry
		device_destroy(driver_class, device_number);
		class_destroy(driver_class);
		// unregister driver
		cdev_del( driver_object);
		unregister_chrdev_region( device_number, 1);
}

module_init(ModInit);
module_exit(ModExit);
