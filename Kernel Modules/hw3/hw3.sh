#!/bin/bash

base=$(pwd)
echo "$base"
case "$1" in
  "clean")
    ##Run clean
    rm -rf $base/artifacts
    rm -rf $base/initrd/root
    rm -rf $base/initrd/lib
    rm -rf $base/initrd/bin
    ;;
  "qemu")
    echo -e "\e[32m Kernel with Busybox, Sysinfo and Networking"
    echo -e "\e[0m"
    cd $base/initrd
    find . | cpio -L -v -o -H newc > $base/artifacts/initrd.cpio
    cd $base
    qemu-system-aarch64 -m 1024 -nographic -machine virt -cpu cortex-a57 -machine type=virt -smp 1 -kernel artifacts/Image.gz -append "console=ttyAMA0 init=/init" -initrd artifacts/initrd.cpio -netdev user,id=mynet0,hostfwd=tcp::22222-:22 -device virtio-net,netdev=mynet0
    ;;
  "ssh_cmd")
    cd $base
    # Fingerprint of the vm is changing!
    ssh_cmd=`ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p 22222 -i $base/artifacts/ssh_key "$2"`
    cd $base/artifacts
    #ssh_cmd=`./dropbearmulti dbclient root@localhost -p 22222 -i dropbear_key -y -y "$2"`
    echo "$ssh_cmd"
    ;;
  *)
    #: '
    echo -e "\e[34m Build from scratch"
    mkdir $base/artifacts
    cd $base/artifacts
    wget -t 5 -c -N https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.11.tar.xz
    tar -xJ --checkpoint=1000 --checkpoint-action="echo=%c" --keep-newer-files -f linux-4.11.tar.xz --totals
    cp $base/kernel/config $base/artifacts/linux-4.11/.config
    cd $base/artifacts/linux-4.11/
    make clean
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
    mv $base/artifacts/linux-4.11/arch/arm64/boot/Image.gz $base/artifacts/Image.gz
    sleep 3
    cd $base

    echo -e "build sysinfo"
    cd $base/sysinfo/src && make CC=aarch64-linux-gnu-gcc && make clean
    mv $base/sysinfo/src/sysinfo $base/artifacts/sysinfo

    echo -e "build busybox"
    cd $base/artifacts
    wget -t 5 -c -N http://busybox.net/downloads/busybox-1.26.2.tar.bz2
    tar xvjf $base/artifacts/busybox-1.26.2.tar.bz2
    cd $base/artifacts/busybox-1.26.2/
    cp $base/busybox/config .config
    make clean
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- 
    mv busybox ..

    echo -e "build dropbear"
    cd $base/artifacts
    wget -t 5 -c -N https://matt.ucc.asn.au/dropbear/releases/dropbear-2016.74.tar.bz2
    tar xvjf $base/artifacts/dropbear-2016.74.tar.bz2
    cd $base/artifacts/dropbear-2016.74/
    ./configure --disable-shadow --disable-lastlog --disable-syslog --disable-wtmp --disable-wtmpx --disable-utmpx --disable-zlib --host=aarch64-linux-gnu
    make clean
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 STATIC=1 strip
    mv dropbearmulti $base/artifacts/dropbearmulti

    echo -e "Linking binaries to initrd"
    mkdir -p $base/initrd/bin
    cd $base/initrd/bin/
    mv $base/artifacts/sysinfo sysinfo
    rm ./busybox
    ln -s  $base/artifacts/busybox ./busybox
    rm ./dropbearmulti
    ln -s $base/artifacts/dropbearmulti ./dropbearmulti
    echo -e "Zipping initrd"
    rm $base/artifacts/initrd.cpio
    mkdir $base/initrd/lib
    cd $base/artifacts/dropbear-2016.74
    cp $(aarch64-linux-gnu-gcc -print-file-name=ld-linux-aarch64.so.1) $base/initrd/lib/
    cp $(aarch64-linux-gnu-gcc -print-file-name=libc.so.6) $base/initrd/lib/
    cp $(aarch64-linux-gnu-gcc -print-file-name=libnss_files.so.2) $base/initrd/lib/

    cd $base/initrd
    mkdir -p root/.ssh
    rm -rf $base/artifacts/ssh_key
    ssh-keygen -t rsa -f $base/artifacts/ssh_key -N ''
    rm -rf $base/initrd/root/.ssh/authorized_keys
    mv $base/artifacts/ssh_key.pub $base/initrd/root/.ssh/authorized_keys
    cd $base/initrd
    find . | cpio -L -v -o -H newc > $base/artifacts/initrd.cpio
    cd $base
    echo -e "\n\nBuild Finished\n\n\e[0m"
    ;;
esac
