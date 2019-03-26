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
MODULE_DESCRIPTION("This driver increments a global counter per kernel thread 10 times");

static dev_t device_number;
static struct cdev *driver_object;
static struct class *driver_class;
static struct device *device_object;

static struct task_struct *thread_id1, *thread_id2;
// init completion 'barrier'
DECLARE_COMPLETION( cmpltn );
DEFINE_MUTEX( list_mutex );
static int global_counter;

//define all possible operations for drivers
static struct file_operations fops = {
	.owner = THIS_MODULE,
};

static int thread_code( void *data )
{
	unsigned long timeout;
	int i;
	wait_queue_head_t wq;

	init_waitqueue_head(&wq);
	allow_signal( SIGTERM );
	for( i=0; i<2; i++ ) {
		// Timeout of three seconds
		timeout=HZ * 3;
		/*
		mutex_lock_interruptible checks if mutex is acquired
		if yes: return "-EINTR"
		else: acquire mutex
		*/
		if( mutex_lock_interruptible( &list_mutex )==-EINTR ) {
			printk(KERN_INFO "Interrupted from signal\n");;
			break;
		}
		// Critical Section
		global_counter++;
		printk(KERN_INFO "mysemaphore: Thread: %d Counter: %d", current->pid, global_counter);
		// End
		mutex_unlock( &list_mutex );
		timeout=wait_event_interruptible_timeout(wq,(timeout==0),timeout);
		printk(KERN_INFO "mysemaphore: thread %d woke up\n", current->pid);
		if( timeout==-ERESTARTSYS ) {
			printk(KERN_INFO "Interrupted from signal\n");;
			break;
		}
	}
	// Signal Completion
	complete_and_exit( &cmpltn, 0 );
	return 0;
}

static int __init ModInit(void)
{
		// Register range of char device numbers
		// alloc_chrdev_region(dev_t* dev, unsigned baseminor,
		// unsigned count, const *char name)
		if ( alloc_chrdev_region(&device_number,0,1,"mysemaphore")<0 ) {
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
		driver_class = class_create( THIS_MODULE, "mysemaphore");
		device_object = device_create(driver_class,NULL,device_number,"%s","mysemaphore");

		printk(KERN_INFO "mysemaphore registered\n");
		thread_id1=kthread_create(thread_code, NULL, "mysemaphore_thread1");
		if( thread_id1 ) {
			thread_id2=kthread_create(thread_code, NULL, "mysemaphore_thread2");
			if( thread_id2 ) {
				wake_up_process( thread_id1 );
				wake_up_process( thread_id2 );
				return 0;
			}
			printk(KERN_WARNING "Creation of Threads failed\n");
		}
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
		kill_pid( task_pid(thread_id1), SIGTERM, 1 );
		kill_pid( task_pid(thread_id2), SIGTERM, 1 );
		// Wait for completion signal
		wait_for_completion( &cmpltn );
		wait_for_completion( &cmpltn );
		printk(KERN_INFO "mysemaphore: \n\n global_counter = %d \n\n", global_counter);
		// delete sysfs entry
		device_destroy(driver_class, device_number);
		class_destroy(driver_class);
		// unregister driver
		cdev_del( driver_object);
		unregister_chrdev_region( device_number, 1);
}

module_init(ModInit);
module_exit(ModExit);
