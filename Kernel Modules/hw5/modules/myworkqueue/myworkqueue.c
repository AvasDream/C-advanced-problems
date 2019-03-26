/*#include <linux/module.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven Inácio");
MODULE_DESCRIPTION("This driver creates a timer that checks the actual waiting times.");

typedef struct times_t_private
{
    unsigned long prev;
    unsigned long curr;
    unsigned long max;
    unsigned long min;
} times_t;

static times_t jiff = {
    .prev = 0UL,
    .curr = 0UL,
    .max = 0UL,
    .min = -1UL};
static times_t cycles = {
    .prev = 0UL,
    .curr = 0UL,
    .max = 0UL,
    .min = -1UL
};

 struct delayed_work mywork_timer;

/* static void work_func(struct work_struct *work); */

/* DECLARE_DELAYED_WORK(mywork_timer, work_func); */
/*
static void work_func(struct work_struct *work)
{
    printk(KERN_INFO,"work_func");
    unsigned long current_ns, max_ns, min_ns;
    unsigned long cyc = get_cycles();
    unsigned long j = jiffies;
    cycles->curr = cyc - cycles->prev;
    jiff->curr = (long)j - (long)jiff->prev;
    if (cycles->curr > cycles->max)
    {
        cycles->max = cycles->curr;
    }
    if (cycles->curr < cycles->min)
    {
        cycles->min = cycles->curr;
    }
    if (jiff->curr > jiff->max)
    {
        jiff->max = jiff->curr;
    }
    if (jiff->curr < jiff->min)
    {
        jiff->min = jiff->curr;
    }
    current_ns = jiff->curr * (long)1e9 / HZ; // jiffy to nanoseconds */
    /*max_ns = jiff->max * (long)1e9 / HZ;
    min_ns = jiff->min * (long)1e9 / HZ;
    printk(KERN_INFO "Current delay: %ld Jiffies, %ld Cycles, %ld ns\n", jiff->curr, cycles->curr, current_ns);
    printk(KERN_INFO "Minimum delay: %ld Jiffies, %ld Cycles, %ld ns\n", jiff->min, cycles->min, min_ns);
    printk(KERN_INFO "Maximum delay: %ld Jiffies, %ld Cycles, %ld ns\n", jiff->max, cycles->max, max_ns);
    cycles->prev = cyc;
    jiff->prev = j;
    schedule_delayed_work(&mywork_timer, (2*HZ));
}

static int __init myqueue_init(void)
{
    printk(KERN_NOTICE "Try Myqueue.");
    printk(KERN_NOTICE "Timer@%p, fn@%p", mywork_timer, work_func);
    INIT_DELAYED_WORK(mywork_timer, work_func);
    int retval = schedule_delayed_work(&mywork_timer, (2*HZ));
    printk(KERN_NOTICE "Schedule executed with %d.", retval);
    if(retval) 
    {
        printk(KERN_NOTICE "FAILURE Schedule workqueue.");
        printk(KERN_NOTICE "FAILURE Schedule workqueue.2");
        return 0;//return -EFAULT;
    } 
    
    printk(KERN_NOTICE "Activated timed workqueue.");
    return 0;
}

static void __exit myqueue_exit(void)
{
    flush_delayed_work(&mywork_timer);
}

module_init(myqueue_init);
module_exit(myqueue_exit);*/


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven Inácio");
MODULE_DESCRIPTION("This driver creates a timer that checks the actual waiting times.");

typedef struct times_t_private
{
    unsigned long prev;
    unsigned long curr;
    unsigned long max;
    unsigned long min;
} times_t;

static times_t * jiff;

static times_t * cycles;

static times_t * nanosecs;

//static struct workqueue_struct *queue;

static void work_func(struct work_struct *w);

DECLARE_DELAYED_WORK(work, work_func);

static void work_func(struct work_struct *w)
{
    cycles->curr = get_cycles() - cycles->prev;
    jiff->curr = jiffies - jiff->prev;
    nanosecs->curr = jiffies_to_nsecs(jiff->curr);
    if (cycles->curr > cycles->max)
    {
        cycles->max = cycles->curr;
    }
    if (cycles->curr < cycles->min)
    {
        cycles->min = cycles->curr;
    }
    if (jiff->curr > jiff->max)
    {
        jiff->max = jiff->curr;
        nanosecs->max = nanosecs->curr;
    }
    if (jiff->curr < jiff->min)
    {
        jiff->min = jiff->curr;
        nanosecs->min = nanosecs->curr;
    }
    printk(KERN_INFO "Workqueue Current delay: %lu Jiffies, %lu Cycles, %lu ns\n", jiff->curr, cycles->curr, nanosecs->curr);
    printk(KERN_INFO "Workqueue Minimum delay: %lu Jiffies, %lu Cycles, %lu ns\n", jiff->min, cycles->min, nanosecs->min);
    printk(KERN_INFO "Workqueue Maximum delay: %lu Jiffies, %lu Cycles, %lu ns\n", jiff->max, cycles->max, nanosecs->max);
    cycles->prev = get_cycles();
    jiff->prev = jiffies;
	schedule_delayed_work(&work, 2*HZ);
}

int init_module(void)
{
	//queue = create_singlethread_workqueue("myworkqueue");
    jiff = (times_t*)kmalloc(sizeof(times_t), GFP_USER);
    cycles = (times_t*)kmalloc(sizeof(times_t), GFP_USER);
    nanosecs = (times_t*)kmalloc(sizeof(times_t), GFP_USER);
    jiff->curr = 0;
    jiff->max = 0;
    jiff->min = 0xFFFFFFFFFFFFFFFF;
    cycles->curr = 0;
    cycles->max = 0;
    cycles->min = 0xFFFFFFFFFFFFFFFF;
    jiff->prev = jiffies;
    cycles->prev = get_cycles();
    int retval = schedule_delayed_work(&work, (2*HZ));
    if(!retval)
    {
        printk(KERN_NOTICE "FAILURE Schedule workqueue.");
        return -EFAULT;
    }
    printk(KERN_NOTICE "Activated timed workqueue.");
	return 0;
}

void cleanup_module(void)
{
	/* TODO why is this needed? Why flush_workqueue doesn't work? (re-insmod panics)
	 * http://stackoverflow.com/questions/37216038/whats-the-difference-between-flush-delayed-work-and-cancel-delayed-work-sync */
	/*flush_workqueue(queue);*/
	//cancel_work_sync(&work);
	//destroy_workqueue(queue);
    cancel_delayed_work(&work);
}