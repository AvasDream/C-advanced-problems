#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vincent Grimmeisen");
MODULE_DESCRIPTION("This driver creates a threadsafe bufferdevice to temporarily store data.");


static dev_t device_number;
static struct cdev *driver_object;
struct class *driver_class;
static struct device *device_object;
static wait_queue_head_t wq_read, wq_write;

#define MAX_BUFFER_SIZE 32
static char *kernelmem;

static atomic_t bytes_that_can_be_written=ATOMIC_INIT(MAX_BUFFER_SIZE);
static atomic_t bytes_available=ATOMIC_INIT(0);
#define READ_POSSIBLE  (atomic_read(&bytes_available)!=0)
#define WRITE_POSSIBLE (atomic_read(&bytes_that_can_be_written)!=0)
#define DEBUG 0
static spinlock_t	rlock;
static spinlock_t	wlock;

static int driver_open( struct inode *devicefile, struct file *instance )
{
	return 0;
}

static int driver_close( struct inode *devicefile, struct file *instance )
{
	return 0;
}

ssize_t driver_read( struct file *instance, char __user *buffer,
	size_t max_bytes_to_read, loff_t *offset)
{
	size_t to_copy, not_copied;

	if(DEBUG) printk(KERN_INFO "mybuffer-driver_read() BUFFER: %s", buffer);

	if (!READ_POSSIBLE && (instance->f_flags&O_NONBLOCK) ) {
		printk(KERN_INFO "mybuffer-driver_read(): Not ready to READ data");
		return  -EAGAIN;
	}

	if (wait_event_interruptible( wq_read, READ_POSSIBLE ) ) {
		printk(KERN_INFO "mybuffer-driver_read(): Interupted by signal while sleeping");
		return -ERESTARTSYS;
	}
	//Critical Section
	spin_lock(&rlock);
	if(DEBUG) printk(KERN_INFO "mybuffer-driver_read() BEFORE: BYTES_AVAILABLE: %d", bytes_available);
	to_copy = min((size_t)atomic_read(&bytes_available),max_bytes_to_read);
	not_copied = copy_to_user( buffer, kernelmem, to_copy );
	atomic_sub( to_copy-not_copied, &bytes_available );
	atomic_add(to_copy-not_copied, &bytes_that_can_be_written);
	if(DEBUG) printk(KERN_INFO "mybuffer-driver_read() AFTER: BYTES_AVAILABLE: %d", bytes_available);
	*offset += to_copy-not_copied;
	spin_unlock(&rlock);
	//Critical Section End
	wake_up_interruptible(&wq_write);
	return to_copy-not_copied;
}

ssize_t driver_write( struct file *instance, const char __user *buffer,
	size_t max_bytes_to_write, loff_t *offset)
{
	size_t to_copy, not_copied;

	if(DEBUG) printk(KERN_INFO "mybuffer-driver_write() BUFFER: %s", buffer);


	if (!WRITE_POSSIBLE && (instance->f_flags&O_NONBLOCK) ) {
 		printk(KERN_INFO "mybuffer-driver_write(): Not ready to WRITE data");
		return -EAGAIN;
	}
	if (wait_event_interruptible(wq_write,WRITE_POSSIBLE) ) {
		printk(KERN_INFO "mybuffer-driver_wite(): Interupted by signal while sleeping");
		return -ERESTARTSYS;
	}
	//Critical Section
	spin_lock(&wlock);
	if(DEBUG) printk(KERN_INFO "mybuffer-driver_write() BEFORE: BYTES_THAT_CAN_BE_WRITTEN: %d", bytes_that_can_be_written);
	to_copy = min( (size_t) atomic_read(&bytes_that_can_be_written),
		max_bytes_to_write );
	not_copied = copy_from_user( kernelmem, buffer, to_copy );

	atomic_sub( to_copy-not_copied, &bytes_that_can_be_written );
	atomic_add(to_copy-not_copied, &bytes_available);
	if(DEBUG) printk(KERN_INFO "mybuffer-driver_write() AFTER: BYTES_THAT_CAN_BE_WRITTEN: %d", bytes_that_can_be_written);
	*offset += to_copy-not_copied;
	spin_unlock(&wlock);
	//Critical Section End
	// Signal for reader
	wake_up_interruptible(&wq_read);
	return to_copy-not_copied;
}



static struct file_operations fops = {
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write
};


static int __init template_init( void )
{
	init_waitqueue_head(&wq_read);
	init_waitqueue_head(&wq_write);

	spin_lock_init( &rlock);
	spin_lock_init( &wlock);

	if((kernelmem = (char*)kcalloc(MAX_BUFFER_SIZE, sizeof(char), GFP_KERNEL))<0) {
		printk(KERN_ALERT "Failed to allocate storage\n");
		goto free_buffer;
	}

	if (alloc_chrdev_region(&device_number,0,1,"mybuffer_sync")<0)
		return -EIO;
	driver_object = cdev_alloc();
	if (driver_object==NULL)
		goto free_device_number;
	driver_object->owner = THIS_MODULE;
	driver_object->ops = &fops;
	if (cdev_add(driver_object,device_number,1))
		goto free_cdev;
	driver_class = class_create( THIS_MODULE, "mybuffer_sync" );
	if (IS_ERR(driver_class)) {
		pr_err("template: no udev support\n");
		goto free_cdev;
	}

	device_object = device_create( driver_class, NULL, device_number, NULL, "%s", "mybuffer_sync" );
	return 0;
free_cdev:
	kobject_put( &driver_object->kobj );
free_device_number:
	unregister_chrdev_region( device_number, 1 );
free_buffer:
	kfree(kernelmem);
	return -EIO;
}

static void __exit template_exit( void )
{
	// delete sysfs entry
	device_destroy( driver_class, device_number );
	class_destroy( driver_class );
	// unregister driver
	cdev_del( driver_object );
	unregister_chrdev_region( device_number, 1 );
	return;
}

module_init( template_init );
module_exit( template_exit );
