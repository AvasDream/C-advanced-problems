#!/bin/bash

base=$(pwd)
echo "$base"
case "$1" in
"qemu_sysinfo")
    echo -e "\e[35m Run Kernel with sysinfo application"
    [ -e $base/initrd.cpio ] && rm $base/initrd.cpio
    cd $base/initrd-sysinfo
    find . | cpio -L -v -o -H newc > $base/initrd.cpio
    cd $base
    echo -e "\e[0m"
    qemu-system-x86_64 -m 64 -nographic -kernel $base/artifacts/linux-4.11/arch/x86/boot/bzImage -append "console=ttyS0 init=./bin/sysinfo" -initrd $base/initrd.cpio
    ;;
"qemu_busybox")
    echo -e "\e[32m Kernel with Busybox and sysinfo application"
    [ -e $base/initrd.cpio ] && rm $base/initrd.cpio
    cd $base/initrd-busybox
    find . | cpio -L -v -o -H newc > $base/initrd.cpio
    cd $base
    echo -e "\e[0m"
    qemu-system-x86_64 -m 64 -nographic -kernel $base/artifacts/linux-4.11/arch/x86/boot/bzImage -append "console=ttyS0 init=./init" -initrd $base/initrd.cpio
   ;;
  "clean")
      ##Run clean
      rm $base/initrd.cpio
      rm -rf $base/artifacts
      ;;
*)
    echo -e "\e[34m Build from scratch"
    mkdir $base/artifacts
    cd $base/artifacts
    wget -t 5 -c -N https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.11.tar.xz
    tar -xJ --checkpoint=1000 --checkpoint-action="echo=%c" --keep-newer-files -f linux-4.11.tar.xz --totals
    cp $base/kernel/config $base/artifacts/linux-4.11/.config
    cd $base/artifacts/linux-4.11/
    make -j 5
    cd $base
    # build sysinfo
    cd $base/sysinfo/src && make && make clean
    mv $base/sysinfo/src/sysinfo $base/artifacts/sysinfo
    # build busybox
    cd $base/artifacts 
    wget http://busybox.net/downloads/busybox-1.26.2.tar.bz2  && tar xvjf $base/artifacts/busybox-1.26.2.tar.bz2 && cd $base/artifacts/busybox-1.26.2/
    cp $base/busybox/config ./.config && make -j 5
    cd $base/initrd-sysinfo/bin
    [ -e ./sysinfo ] && rm ./sysinfo
    ln -s  ../../artifacts/sysinfo ./sysinfo
    cd ./../../initrd-busybox/bin/
    [ -e ./sysinfo ] && rm ./sysinfo
    ln -s  ./../../artifacts/sysinfo ./sysinfo
    [ -e ./busybox ] && rm ./busybox
    ln -s  ./../../artifacts/busybox-1.26.2/busybox ./busybox
    echo -e "\n\nBuild Finished\n\n\e[0m"
    ;;
esac
