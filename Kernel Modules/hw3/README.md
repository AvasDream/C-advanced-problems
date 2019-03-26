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
