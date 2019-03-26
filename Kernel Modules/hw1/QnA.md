# HW1 Questions and Answers

## Sysinfo

### What other information can uname tell about a system?

Kernel name, network node hostname, kernel release, kernel version, machine hardware name, processor type, hardware platform and the name of the operating system

### How can you instruct gcc to produce statically linked binaries?

After compilation with -c - so it won't link automatically - you can pass the -static statement while linking the object files together.
See generic_static_compile.sh script at the root of this github for an example.

### Which tools are able to display the dynamic dependencies for binaries on Linux?

file, objdump -x, readelf -d or simply ldd

### What is the practical problem with dynamic linking when you want to install your program on a different system, e.g. a Virtual Machine?

The executable may not be runnable because the libraries, the program is linked to, are different versions, or might not be present at all.

## Kernel Configuration

### How can you get more information about an item in the menuconfig?

Simply highlight the item with your cursor and either navigate to help and press **\<ENTER>** or press **\<H>**

### What is the relation between the menuconfig and the .config file in the kernel source directory?

menuconfig can generate a valid .config file using the options you selected during menuconfig.

### Which CONFIG_* options belong to the menu entries that are provided above?

```
64-Bit kernel = CONFIG_64bit, CONFIG_X86_64

General setup:<
 -> Kernel compression mode (GZIP) = CONFIG_KERNEL_GZIP
 [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support = CONFIG_BLK_DEV_INITRD
 Compiler optimization level (Optimize for size)  ---> = CONFIG_CC_OPTIMIZE_FOR_SIZE
 [*] Configure standard kernel features (expert users) = CONFIG_EXPERT
     ->[*] Enable support for printk = CONFIG_PRINTK
 [*] Embedded system = CONFIG_EMBEDDED

 Executable file formats / Emulations:
 [*] Kernel support for elf binaries = CONFIG_BINFMT_ELF
 [*] Kernel support for scripts starting with #! = CONFIG_BINFMT_SCRIPT

 Device Drivers:
 -> Generic Driver options
     [*] Maintain a devtmpfs filesystem to mount at /dev = CONFIG_DEVTMPFS

 -> Character devices:
     [*] Enable TTY = CONFIG_TTY
     [*]   Virtual terminal = CONFIG_VT
     [*]     Enable character translations in console = CONFIG_CONSOLE_TRANSLATIONS
     [*]     Support for console on virtual terminal = CONFIG_VT_CONSOLE
     [*]     Support for binding and unbinding console drivers = CONFIG_VT_HW_CONSOLE_BINDING
     [*]   Unix98 PTY support = CONFIG_UNIX98_PTYS
     -> Serial Drivers
         ->[*] 8250/16550 and compatible serial support = CONFIG_SERIAL_8250
         ->[*] Console on 8250/16550 and compatible serial port = CONFIG_SERIAL_8250_CONSOLE

 File systems -> Pseudo Filesystems:
 [*] /proc file system support = CONFIG_PROC_FS
 [*] Sysctl support (/proc/sys) = CONFIG_PROC_SYSCTL
 [*] sysfs file system support = CONFIG_SYSFS
```

###What are the different times displayed by time?

The first time displayed is the elapsed real time, the second the elapsed user cpu time and the third the system cpu time.

###Where and which are the output binary files produced by the compilation?

All the binaries produced by the compilation are in the linux-4.11/arch/x86/boot folder. 

setup.bin, vmlinux.bin but vmlinux is flagged as data when executing file. 

###Which is the binary that represents bootable kernel image?

The bootable kernel is the binary at /arch/x86/boot/bzImage.

###Which TTY-device do you need to pass to the kernel for console input/output?

In our config we specified the support for the serial terminal for 8250/16550 with "console=ttyS0" we can pass our kernel the argument to use 
the serial terminal number 0.

###Is the system in a usable state, e.g. can you use a shell to execute commands on it?
If not, what is missing?
No the system is not usable because we have not specified a root filesystem.


###What could the -L parameter be useful for?

The -L flag can be useful if you dont want links to files which dont exist in our new created userland. It copys the file a link points to in the archive and not the link.

###How can you list the contents of a CPIO archive?

cpio -itv < initrd.cpio

###What is the path of program that the kernel can execute after unpacking it?

All executable binaries for normal users are in the /bin folder and binaries for the system are in the /sbin folder.

###What needs to be passed to the kernel within append in order to tell it what binary to execute?

The -init flag to set the initial command to be executed by the kernel. If this is not a interactive command you cant do anything with the system.

###What is the default executable path of the kernel in case nothing is passed to change it?

Per default the kernel will try to execute /sbin/init then /bin/init and last /bin/sh.

###What is the complete qemu command line to run your sysinfo application as the init process?

/bin/sysinfo


###How do multi-call binaries work?

They are binaries with different functionality. You have to create symbolic links with the corresponding name of the utility you want to use from the multi call binary.

###What applets are needed to allow us to interact with the system?

A shell is needed, in this case we enabled the ash shell.

###How are symlinks to these binaries interpreted?

If you create a symlink to busybox (ln -s /bin/busybox ls) the system appends /bin/busybox before the command you link.


###Does your busybox file have dynamic link dependencies against libraries installed on the build host?

No it does not because is saw the option the first time I configured busybox.

###How can you verify this on the command line?

You can check this with the file command. 

###What busybox options have you chosen for your new initrd, and why?

cp - to copy files
mkdir - to make directorys
mv - to move files
pwd - to set the password for a user
whoami - to see under which user you are working
rmdir - to delete directorys
sleep - to let a process sleep
touch - to create files
vi - for a simple text editor and because nano was not an option
find - to search for files
grep - to search in text
ash - to have a interactive shell
top - to monitor processes
ls - to list files in directorys
mount - to mount the filesystems
unmount - to unmount filesystems
cp - to copy files
ln - to link files, was not necessary because busybox has the install flag

###Which additional directories are required to make the system work at runtime?

/sys to know which devices are present on the system

/dev to have a directory for all devices

/proc for all the processe

/tmp to have a directory for temporary files

###Which directories need to be created to be compliant with the Linux FHS 3.0?

the directorys 
boot etc lib media mnt opt run sbin srv tmp usr var sys proc

###What are all the mounts/filesystems that are active after the system has booted?

/dev is for special or device files

/proc is for all the processes and information about them

/tmp for all temporary files

/boot for static files needed at boot time. In this case just for FHS 3.0 compliance

/var for logfiles

/sys for information about devices

/opt for third party software in this case 

/lib for librarys

/etc for configuration files

/media to mount external media like cd0 (FHS 3.0 compliance)

###How does the filetree under / look after the system is booted? 

drwxr-xr-x   19 0        0              0 Oct 31 17:26 .
drwxr-xr-x   19 0        0              0 Oct 31 17:26 ..
-rw-r--r--    1 0        0           1.0K Oct 31 16:54 .init.swp
drwxr-xr-x    2 0        0              0 Oct 31 17:26 bin
drwxr-xr-x    2 0        0              0 Oct 31 17:26 boot
drwxr-xr-x    2 0        0              0 Oct 31 17:26 dev
drwxr-xr-x    2 0        0              0 Oct 31 17:26 etc
-rwxr-xr-x    1 0        0            600 Oct 31 16:54 init
drwxr-xr-x    2 0        0              0 Oct 31 17:26 lib
drwxr-xr-x    2 0        0              0 Oct 31 17:26 media
drwxr-xr-x    2 0        0              0 Oct 31 17:26 mnt
drwxr-xr-x    2 0        0              0 Oct 31 17:26 opt
dr-xr-xr-x   24 0        0              0 Oct 31 17:26 proc
drwx------    2 0        0              0 Oct 31 15:49 root
drwxr-xr-x    2 0        0              0 Oct 31 17:26 run
drwxr-xr-x    2 0        0              0 Oct 31 17:26 sbin
drwxr-xr-x    2 0        0              0 Oct 31 17:26 srv
dr-xr-xr-x   10 0        0              0 Oct 31 17:26 sys
drwxr-xr-x    2 0        0              0 Oct 31 17:26 tmp
drwxr-xr-x    2 0        0              0 Oct 31 17:26 usr
drwxr-xr-x    2 0        0              0 Oct 31 17:26 var

