#!/bin/bash

base=$(pwd)
echo "$base"
case "$1" in
  "clean")
    ##Run clean
    rm -rf $base/artifacts
    rm -rf $base/initrd/root
    rm -rf $base/initrd/lib
    rm -rf $base/initrd/root
    ;;
  "qemu")
    echo -e "\e[32m Kernel with Busybox, Sysinfo and Networking"
    echo -e "\e[0m"
    cd $base/initrd
    find . | cpio -L -v -o -H newc > $base/artifacts/initrd.cpio
    qemu-system-x86_64 -m 64 -nographic -kernel $base/artifacts/bzImage -append "console=ttyS0 init=./init" -initrd $base/artifacts/initrd.cpio -netdev user,id=mynet0,hostfwd=tcp::22222-:22 -device virtio-net,netdev=mynet0
    ;;
  "ssh_cmd")
    cd $base
    # Fingerprint of the vm is changing!
    ssh-keygen -R localhost
    #ssh_cmd=`ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p 22222 -i $base/artifacts/ssh_key "$2"`
    cd $base/artifacts
    ssh_cmd=`./dropbearmulti dbclient root@localhost -p 22222 -i dropbear_key -y -y "$2"`
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
    make -j 5
    mv $base/artifacts/linux-4.11/arch/x86/boot/bzImage $base/artifacts/bzImage
    cd $base
    echo -e "build sysinfo"
    cd $base/../hw1/sysinfo/src && make && make clean
    mv $base/../hw1/sysinfo/src/sysinfo $base/artifacts/sysinfo
    echo -e "build busybox"
    cd $base/artifacts
    wget -t 5 -c -N http://busybox.net/downloads/busybox-1.26.2.tar.bz2  && tar xvjf $base/artifacts/busybox-1.26.2.tar.bz2 && cd $base/artifacts/busybox-1.26.2/
    cp $base/busybox/config ./.config && make clean && make -j 5
    echo -e "build dropbear"
    cd $base/artifacts
    wget -t 5 -c -N https://matt.ucc.asn.au/dropbear/releases/dropbear-2016.74.tar.bz2 && tar xvjf $base/artifacts/dropbear-2016.74.tar.bz2 && cd $base/artifacts/dropbear-2016.74/
    ./configure --disable-shadow --disable-lastlog --disable-syslog --disable-wtmp --disable-wtmpx --disable-utmpx  && \
    make clean && make -j 5 PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 STATIC=1 strip && mv dropbearmulti $base/artifacts/dropbearmulti
    echo -e "Linking binaries to initrd"
    cd $base/initrd/bin/
    rm ./sysinfo
    ln -s  $base/artifacts/sysinfo ./sysinfo
    rm ./busybox
    ln -s  $base/artifacts/busybox-1.26.2/busybox ./busybox
    rm ./dropbearmulti
    ln -s $base/artifacts/dropbearmulti ./dropbearmulti
    echo -e "Zipping initrd"
    rm $base/artifacts/initrd.cpio
    #Oder einfach manuell kopieren
    mkdir $base/initrd/lib
    #'
    cd $base/artifacts/dropbear-2016.74
    cp $(gcc -print-file-name=ld-linux-x86-64.so.2) $base/initrd/lib/
    cp $(gcc -print-file-name=libc.so.6) $base/initrd/lib/
    cp $(gcc -print-file-name=libnss_files.so.2) $base/initrd/lib/

    cd $base/initrd
    mkdir -p $base/initrd/root/.ssh
    [ -e $base/artifacts/dropbear_key ] && rm -rf $base/artifacts/dropbear_key
    bin/dropbearmulti dropbearkey -t rsa -f $base/artifacts/dropbear_key && chmod 666 $base/artifacts/dropbear_key
    [ -e $base/initrd/root/.ssh/authorized_keys ] && rm -rf $base/initrd/root/.ssh/authorized_keys
    bin/dropbearmulti dropbearkey -f $base/artifacts/dropbear_key -y > $base/initrd/root/.ssh/authorized_keys
    [ -e $base/artifacts/ssh_key ] && rm $base/artifacts/ssh_key
    bin/dropbearmulti dropbearconvert dropbear openssh $base/artifacts/dropbear_key $base/artifacts/ssh_key
    cd $base/initrd
    find . | cpio -L -v -o -H newc > $base/artifacts/initrd.cpio
    cd $base
    echo -e "\n\nBuild Finished\n\n\e[0m"
    ;;
esac
