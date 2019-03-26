1. Enable NIC in quemu
2. Configure Busybox to use Network utilities
3. Compile and Configure Dropbear
4. Configure Kernel for multiuser 
5. Generate SSH Client Key
6. Create Ramdisk
7. Run Kernel
8. Write Init file


###Commands

`wget -t 5 -c -N https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.11.tar.xz && tar -xJ --checkpoint=1000 --checkpoint-action="echo=%c" --keep-newer-files -f linux-4.11.tar.xz --totals`

`wget http://busybox.net/downloads/busybox-1.26.2.tar.bz2  && tar xvjf busybox-1.26.2.tar.bz2`



`qemu-system-x86_64 -m 64 -nographic -kernel $base/artifacts/linux-4.11/arch/x86/boot/bzImage -append "console=ttyS0 init=/bin/sysinfo" -initrd initrd.cpio`


`cpio -itv < initrd.cpio`


`ln -s ../../artifacts/sysinfo ./initrd-sysinfo/bin/sysinfo`


`find . | cpio -L -v -o -H newc > ../initrd.cpio`

