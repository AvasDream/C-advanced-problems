
## Things to remeber

* kthread test
EIO -> Input output error
ERESTART -> Interrupted system call should be restarted


## Disable Kernel messages to stdout

(How to stop kernel messages from flooding my console)[https://superuser.com/questions/351387/how-to-stop-kernel-messages-from-flooding-my-console]

* Activated sysctl in busybox and pray to the default configuration gods

Does not work, no motivation to investigate further.

## How to find header files for functions you need

First use google then:

(Bootlin)[https://elixir.bootlin.com/linux/v4.11/ident/kill_pid]

## mykthread

* Start Kernel Thread that runs a loop over printing a message to the kernel log and then goes to sleep for 2 seconds.

Like a thread in userspace except the thread is executing _only_ in kernel space.

When the thread is executed is determinate by the scheduler. But the
kernel thread itself has to go to sleep to free the cpu because it is
running in kernel level with high privileges. If a thread does not got to
sleep while waiting for _something_ it will steal execution time from
other processes.

To wake a sleeping kernel thread we can wake him up directly or give him
a signal. A signal is only possible if the thread is interruptible

`kill_pid` send kill signal to kernel thread with pid.

`current->pid` get pid of current kernel thread.

`allow_signal` activate the option to receive signals. Per default this is disabled.

Deletion of a module is always a critical section. It has to be guaranteed that the kernel thread has finished.


Fuck everything i wrote till here. There is a framework that gives us a
solution to all these problems. _kthread-daemon_

Kthreadd does ensure that our thread has:

* No ressources in user space.
* No open files
* blocks all signals
* can be executed on every cpu core in the system
* has a _normal_ priority for scheduling

the kthreadd is executed directly after the init process and has because of that the PID 2.


```
struct task_struct * kthread_create (	int (* 	threadfn(void *data),
 										void *  	data,
										const char  	namefmt[],...);
```

threadfn -> the function to run until signal_pending(current).

data ->  data ptr for threadfn.

namefmt ->      printf-style name for the thread.

`int kthread_should_stop (void);`

When someone calls kthread_stop on your kthread, it will be woken and
this will return true. You should then return, and your return value
will be passed through to kthread_stop.

There is a kthread_create and a kthread_run function. The difference is
the kthread_run function creates a thread and then wakes it up directly
while with kthread_create the scheduler gives to some time X later the
signal for execution.


HZ is one second because of reasons. (See Jiffies, timer wheel etc)

wait_event_interruptible_timeout(wq, condition, timeout) -> sleep until a
condition gets true
or a timeout elapses

```
void complete_and_exit(struct completion *x);

Die Funktion ruft die Funktion complete mit »x« als Parameter auf und
beendet sich danach selbst. Die Funktion wird von Kernel-Threads benutzt,
um anderen Instanzen das Ende des Kernel-Threads mitzuteilen.
```


## mysemaphore

```
int __sched mutex_lock_interruptible (	struct mutex * lock);

# acquire the mutex, interruptible

int __sched mutex_lock_killable (	struct mutex * lock);

# acquire the mutex only interupptible through a signal that leads to
the end of execution of the thread.

int __sched mutex_try_lock (	struct mutex * lock);

# Does the same as lock but if mutex is already acquired function call
returns immediately.
```

