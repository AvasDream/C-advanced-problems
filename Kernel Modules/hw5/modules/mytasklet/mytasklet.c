#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven Inácio");
MODULE_DESCRIPTION("This driver creates a tasklet which prints out the group number.");

static void tasklet_function(unsigned long data) {
    printk(KERN_INFO "Tasklet grp20");
}

DECLARE_TASKLET(tldescr, tasklet_function, 0L);

static int __init ModInit(void)
{
    tasklet_schedule(&tldescr);
    return 0;
}

static void __exit ModExit(void)
{
    tasklet_kill(&tldescr);
}

module_init(ModInit);
module_exit(ModExit);