
## Which functions are available in the kernel for allocating/freeing memory?

For actual physical memory:
 * `kmalloc(size, options)` which can be used to allocate memory with different options as to where and how exactly you want your memory allocated.
 * `kfree(memory)` is able to free memory in kernel space. 

For virtual memory:
 * `vmalloc(size)` to allocate a virtually contiguous memory space
 * `vfree(memory)` to free this memory

For more functions and different memory types see [this](https://www.oreilly.com/library/view/linux-device-drivers/0596005903/ch08.html)

## Which functions did you choose and why?

I used kmalloc because I didn't need to allocate a great amount of memory and needed to use it on all CPUs.

## Which is the more accurate time source: jiffies or CPU cycles?

With the way current processors can regulate their clock speeds, Jiffies are a more accurate and reliable time source.
If it's an cpu that cannot throttle or turbo up it's clock speed, I'd say cpu cycles.

## How can you identify the kernel thread's PID using the ps utility?

I can grep for the nameFmt String i passed in the create_kthread function.
When i want to get the pid in the thread itself i can use `current->pid`.

## Can your kernel thread receive and handle signals that were sent from userspace using kill?

Yes i cann kill it from the user space but only becaus i allowed signals in the thread function.

## What is the name of the workqueue in the process list?

Since I use a standard event workqueue from the kernel itself I cannot identify my worker. I guess it's one of the standard kworker.

## How do the time measurements of the workqueue compare to the one from the timer?

They tend to be identical.

## What problems can occur on module unload?

If the cleanup of the workqueue fails (or is poorly written) the worker could try to re-add a cancelled job which may or may not exist.
If the re-adding is successfull you end up in an infinite loop of the worker. If it fails because the job did not exist anymore the kernel tries to add a random piece of memory into it's workqueue that may or may not be executable, which may or may not result in a kernel exception.

## How would the design of mutual exclusion look like using a spinclock?

The design would be the same because i am using the semaphore as mutex (init with 1). The difference is that with a spinlock we are waisting cpu power while waiting for an event. E.g:

`while (try_lock());`


## Which implementation would you prefer, and why?

I would prefer spinlocks because functions in the interrupt context have to wait active. Otherwise they will be put to sleep and maybe never
awake again because of the interrupt level.

##  Can close and unload unconditionally clean up the resources?

No one important condition is that no threads are running. We ensure this with the global cmpltn (completion) and the (completion
API)[https://www.kernel.org/doc/Documentation/scheduler/completion.txt].
The mechanism behind this API is basically a barrier.
