#include <linux/module.h>
#include <linux/timex.h>

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

static struct timer_list mytimer;

static times_t jiff = {
    .prev = 0L,
    .curr = 0L,
    .max = 0L,
    .min = 0xFFFFFFFFFFFFFFFF,
};
static times_t cycles = {
    .prev = 0L,
    .curr = 0L,
    .max = 0L,
    .min = 0xFFFFFFFFFFFFFFFF,
};
static times_t nanosecs = {
    .prev = 0L,
    .curr = 0L,
    .max = 0L,
    .min = 0xFFFFFFFFFFFFFFFF,
};

static void timer_func(unsigned long arg)
{
    unsigned long cyc = get_cycles();
    unsigned long j = jiffies;
    cycles.curr = cyc - cycles.prev;
    jiff.curr = j - jiff.prev;
    nanosecs.curr = jiffies_to_nsecs(jiff.curr);
    printk(KERN_DEBUG "%lu JIFFIES", j);
    printk(KERN_DEBUG "%lu CYCLES", cyc);
    if (cycles.curr > cycles.max)
    {
        cycles.max = cycles.curr;
    }
    if (cycles.curr < cycles.min)
    {
        cycles.min = cycles.curr;
    }
    if (jiff.curr > jiff.max)
    {
        jiff.max = jiff.curr;
        nanosecs.max = nanosecs.curr;
    }
    if (jiff.curr < jiff.min)
    {
        jiff.min = jiff.curr;
        nanosecs.min = nanosecs.curr;
    }
    printk(KERN_INFO "Timer Current delay: %lu Jiffies, %lu Cycles, %lu ns\n", jiff.curr, cycles.curr, nanosecs.curr);
    printk(KERN_INFO "Timer Minimum delay: %lu Jiffies, %lu Cycles, %lu ns\n", jiff.min, cycles.min, nanosecs.min);
    printk(KERN_INFO "Timer Maximum delay: %lu Jiffies, %lu Cycles, %lu ns\n", jiff.max, cycles.max, nanosecs.max);
    cycles.prev = cyc;
    jiff.prev = j;
    mytimer.expires = j + (2 * HZ);
    add_timer(&mytimer);
}

static int __init timer_init(void)
{
    init_timer(&mytimer);
    unsigned long j = jiffies;
    unsigned long cyc = get_cycles();
    mytimer.function = timer_func;
    mytimer.data = 0;
    mytimer.expires = j + (2 * HZ);
    jiff.prev = j;
    cycles.prev = cyc;
    add_timer(&mytimer);
    printk(KERN_NOTICE "Activated timer.");
    return 0;
}

static void __exit timer_exit(void)
{
    if (del_timer_sync(&mytimer))
    {
        printk(KERN_NOTICE "Closed active timer.");
    }
}

module_init(timer_init);
module_exit(timer_exit);

