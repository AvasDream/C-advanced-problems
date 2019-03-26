## ToDo
* Reset branch befor final PR, binaries in repo!
* zero module & test
* integrate module compilation in script
* integrate module transfer in script
* integrate module loading in script
* integrate module tests hw4.sh

## Homework 4

Kernel printk level:

```
KERN_EMERG 0 Das System ist nicht mehr zu gebrauchen.
KERN_ALERT 1 Sofortige Maßnahmen sind erforderlich.
KERN_CRIT 2 Der Systemzustand ist kritisch.
KERN_ERR 3 Fehlerzustände sind aufgetreten.
KERN_WARNING 4 Warnung.
KERN_NOTICE 5 Wichtige Nachricht, kein Fehler.
KERN_INFO 6 Zur Information.
KERN_DEBUG 7 Debug-Information.

printk(“<7>user will %d bytes kopieren.\n“, count );

mknod MyDeviceName c Major Minor
```

## kernel changes

~~`PRINTK_TIME [=y]` enable timestamps on printk~~

* Forced module loading
* module unloading
* forced module unloading
* module versioning
* compress modules
* trim unused chars from modules
* kobject debugging

`MODULES [=y]` Enable modules & unloading & versioning
`PREEMPT [=y]` Make all kernel code preemtable.

After this you get prompted while compiling,
leave everything default _EXCEPT_:

`CONFIG_SERIAL_AMBA_PL011=y`

`CONFIG_SERIAL_AMBA_PL011_CONSOLE=y`

Note: this was the reason qemu did nothing because i gave qemu no terminal.

For modules unecessary things i changed:

```
X86_PLATFORM_DEVICES=n
CONFIG_SYSFS_SYSCALL=y
CONFIG_TIMERFD=y
CONFIG_SIGNALFD=y
CONFIG_EVENTFD=Y
CONFIG_FUTEX=Y
CONFIG_BASE_FULL=Y
CONFIG_BUG=y
CONFIG_KALLSYMS=y
CONFIG_BLOCK=y
CONFIG_PCI_HOST_GENERIC=Y
```

## Busybox changes

* modprobe
* lsmod
* rmmod
* insmod
* depmod
* modinfo
* dmesg
* md5sum // to check integrity of modules
* mknod to create char/block/fifo, we need this later imho

## Compile Kernel MODULES

Makefile needs hard tabs, can be configured in atom

`cd $base/modules/hello_kworld && make && cp hello_kworld.ko ../../artifacts/hello_kworld.ko`

`./hw4.sh ssh_cmd "cat > /hello_kworld.ko" < artifacts/hello_kworld.ko`

`insmod hello_world.ko`
## myzero

`__put_user(x,ptr)`

Write a simple value in user space WITHOUT checking if addressspace is correct

`put_user(x,ptr)`

Write a simple value in user space AND check addressspace

## Commands
`uname -r #`
`mkdir -p #nested dirs`
## Notes

* Strange Error while compiling kernel regarding usb
=======
### We dont need to compile gcc again for the aarch64 architecture, everything is given on the system.
### I think for pragmatic reasons we should just link everything statically.
### If i got it right all the configurations regarding networking etc should stay the same.
## Get system triplet of Host

`gcc -dumpmachine`

machine-vendor-operatingsystem

Host: x86_64-linux-gnu

Target: aarch64-linux-gnu

## Kernel

`make menuconfig ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu`
`diff .config ../../kernel/config > ../../config.diff`


```
SERIAL_AMBA_PL011 [=y]  
CONFIG_PCI_HOST_GENERIC [=y]
```

Wrong command:

`make -j 5 ARCH=arm64 CROSS_COMPILE=aarch64-unknown-linux-gnu CC=aarch64-linux-gnu-gcc LD=aarch64-linux-gnu-ld`

Should look like this:

`make -j 5 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-`


`qemu-system-aarch64 -machine virt -cpu help`

`qemu-system-aarch64 -M help`

Check supported cpus:

```
for line in $(cat artifacts/cpus.txt)
do qemu-system-aarch64 -m 1024 -nographic -machine virt -cpu $line -machine type=virt  -kernel artifacts/linux-4.11/arch/arm64/boot/Image.gz
done
```
supported cpus:

```  
cortex-a15  - Address Error
cortex-a53 - Seems to work
cortex-a57 - Seems to work
```

## Sysinfo

[manpage](http://pubs.opengroup.org/onlinepubs/007908775/xsh/sysutsname.h.html)


## dropbear

`./configure --disable-shadow --disable-lastlog --disable-syslog --disable-wtmp --disable-wtmpx --disable-utmpx  --host=aarch64-unknown-linux-gnu`

`make ARCH=arm64 CROSS_COMPILE=aarch64-unknown-linux-gnu- -j 5 PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 STATIC=1 strip`

Working commands:

`./configure --disable-shadow --disable-lastlog --disable-syslog --disable-wtmp --disable-wtmpx --disable-utmpx --disable-zlib --host=aarch64-linux-gnu`

`make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j 5 PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 STATIC=1 strip`

Note when cross compiling dropbear cant find zlib, bc of that i excluded zlib from the build


## Run qemu

-smp sets number of cpus

`	qemu-system-aarch64 -m 1024 -nographic -machine virt -cpu cortex-a57 -machine type=virt  -smp 1 -kernel artifacts/Image.gz -append "console=ttyAMA0 init=/init" -initrd artifacts/initrd.cpio -netdev user,id=mynet0,hostfwd=tcp::22224-:22 -device virtio-net,netdev=mynet0
