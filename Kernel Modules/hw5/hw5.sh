#!/bin/bash

function qemu {
    echo -e "\e[32m Kernel with Busybox, Sysinfo and Networking"
    echo -e "\e[0m"
    cd $base/initrd
    find . | cpio -L -v -o -H newc > $base/artifacts/initrd.cpio
    cd $base

    qemu-system-aarch64 -m 1024 -nographic -machine virt -cpu cortex-a57 -machine type=virt -smp 1 -kernel artifacts/Image.gz -append "console=ttyAMA0 init=/init" -initrd artifacts/initrd.cpio -netdev user,id=mynet0,hostfwd=tcp::22222-:22 -device virtio-net,netdev=mynet0

	return
}

function modules_build {
	cd $base/modules
	for folder in $(ls)
	do
		echo "build $folder"
		cd $folder
		make 2> /dev/null
		make test 2> /dev/null
		cp *.ko $base/artifacts/
		cp *.test $base/artifacts/
		make clean 2> /dev/null
		cd ..
	done
	return
}

function modules_copy {
	cd $base/artifacts
	for module in $(ls *.test *.ko| cut -d" " -f9)
	do
		cd ..
		echo "Copy $module to VM"
		/bin/bash $base/hw5.sh ssh_cmd "cat > /lib/modules/4.11.0/$module" < $base/artifacts/$module 2> /dev/null

		/bin/bash $base/hw5.sh ssh_cmd "chmod +x /lib/modules/4.11.0/$module"

		echo -e "\e[31mManually check file integrity\e[0m"
		echo "**************************************"
		/bin/bash $base/hw5.sh ssh_cmd "md5sum /lib/modules/4.11.0/$module"
		md5sum $base/artifacts/$module
		echo "**************************************"
		cd $base/artifacts
	done

	return
}
function modules_load {
	cd $base/artifacts
	for module in $(ls *.ko| cut -d" " -f9 | cut -d"." -f1)
	do
		cd ..

		echo "Load $module"
		/bin/bash $base/hw5.sh ssh_cmd "modprobe $module"
		/bin/bash $base/hw5.sh ssh_cmd "chmod 777 /dev/$module"

		cd $base/artifacts
	done
	return
}
function modules_unload {
	cd $base/artifacts
	for module in $(ls *.ko| cut -d" " -f9 | cut -d"." -f1)
	do
		cd ..
		echo "Unload $module to VM"
		/bin/bash $base/hw5.sh ssh_cmd "rmmod $module"
		cd $base/artifacts
	done
	return
}
function modules_test {
	cd $base/artifacts

	for module in $(ls *.ko| cut -d" " -f9 | cut -d"." -f1)
	do
		cd ..
		echo "Test $module to VM"
		/bin/bash $base/hw5.sh ssh_cmd "/bin/bash -c /lib/modules/4.11.0/$module.ko.test"
		cd $base/artifacts

	done
	return
}

function ssh_cmd {
  cd $base
  # Fingerprint of the vm is changing!

  ssh_cmd=`ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p 22223 -i $base/artifacts/ssh_key "$2"`
  cd $base/artifacts
  #ssh_cmd=`./dropbearmulti dbclient root@localhost -p 22223 -i dropbear_key -y -y "$2"`

  echo "$ssh_cmd"
}

function build_artifacts {

  echo -e "\e[34m Build from scratch"
  mkdir $base/artifacts
  cd $base/artifacts
  wget -t 5 -c -N https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.11.tar.xz
  tar -xJ --checkpoint=1000 --checkpoint-action="echo=%c" --keep-newer-files -f linux-4.11.tar.xz --totals
  cp $base/kernel/config $base/artifacts/linux-4.11/.config
  cd $base/artifacts/linux-4.11/
  make clean
  make -j 5 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
  mv $base/artifacts/linux-4.11/arch/arm64/boot/Image.gz $base/artifacts/Image.gz
  cd $base

  echo -e "build busybox"
  cd $base/artifacts
  wget -t 5 -c -N http://busybox.net/downloads/busybox-1.26.2.tar.bz2 > /dev/null
  tar xvjf $base/artifacts/busybox-1.26.2.tar.bz2 > /dev/null
  cd $base/artifacts/busybox-1.26.2/
  cp $base/busybox/config .config
  make clean
  make -j 5 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- > /dev/null
  mv busybox ..

  echo -e "build sysinfo"
  cd $base/sysinfo/src && make CC=aarch64-linux-gnu-gcc && make clean
  mv $base/sysinfo/src/sysinfo $base/artifacts/sysinfo

  echo -e "build dropbear"
  cd $base/artifacts
  wget -t 5 -c -N https://matt.ucc.asn.au/dropbear/releases/dropbear-2016.74.tar.bz2 > /dev/null
  tar xvjf $base/artifacts/dropbear-2016.74.tar.bz2
  cd $base/artifacts/dropbear-2016.74/
  ./configure --disable-shadow --disable-lastlog --disable-syslog --disable-wtmp --disable-wtmpx --disable-utmpx --disable-zlib --host=aarch64-linux-gnu > /dev/null
  make clean
  make -j 5 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 STATIC=1 strip > /dev/null
  mv dropbearmulti $base/artifacts/dropbearmulti

  echo -e "Linking binaries to initrd"
  mkdir -p $base/initrd/bin
  cd $base/initrd/bin/
  [ -e sysinfo ] && rm ./sysinfo
  ln -s  $base/artifacts/sysinfo ./sysinfo
  [ -e busybox ] && rm ./busybox
  ln -s  $base/artifacts/busybox ./busybox
  [ -e dropbearmulti ] && rm ./dropbearmulti
  ln -s $base/artifacts/dropbearmulti ./dropbearmulti
  echo -e "Zipping initrd"
  [ -e $base/artifacts/initrd.cpio ] && rm $base/artifacts/initrd.cpio
  mkdir $base/initrd/lib
  cd $base/artifacts/dropbear-2016.74
  cp $(aarch64-linux-gnu-gcc -print-file-name=ld-linux-aarch64.so.1) $base/initrd/lib/
  cp $(aarch64-linux-gnu-gcc -print-file-name=libc.so.6) $base/initrd/lib/
  cp $(aarch64-linux-gnu-gcc -print-file-name=libnss_files.so.2) $base/initrd/lib/


  cd $base/initrd
  mkdir -p root/.ssh
  [ -e $base/artifacts/ssh_key ] && rm -rf $base/artifacts/ssh_key
  ssh-keygen -t rsa -f $base/artifacts/ssh_key -N ''
  [ -e $base/artifacts/initrd.cpio ] && rm -rf $base/initrd/root/.ssh/authorized_keys
  mv $base/artifacts/ssh_key.pub $base/initrd/root/.ssh/authorized_keys
  cd $base/initrd
  find . | cpio -L -v -o -H newc > $base/artifacts/initrd.cpio
  cd $base
  echo -e "\n\nBuild Finished\n\n\e[0m"
  return
}

base=$(pwd)

case "$1" in
  "modules_build")
  	modules_build
	exit
    ;;
  "modules_copy")
	modules_copy
	exit
    ;;
  "modules_load")
	modules_load
	exit
    ;;
  "modules_test")
	modules_test
	exit
    ;;
  "modules_unload")
	modules_unload
	exit
    ;;
  "modules")
	modules_build
	modules_copy
	modules_load
	modules_test
	modules_unload

	exit
    ;;
  "clean")
    ##Run clean
    rm -rf $base/artifacts
    rm -rf $base/initrd/root
    rm -rf $base/initrd/lib
    rm -rf $base/initrd/bin
	exit
    ;;
  "qemu")
    qemu
	exit
    ;;
  "ssh_cmd")
    cd $base
    # Fingerprint of the vm is changing!

    ssh_cmd=`ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p 22222 -i $base/artifacts/ssh_key "$2"`
    cd $base/artifacts
    #ssh_cmd=`./dropbearmulti dbclient root@localhost -p 22223 -i dropbear_key -y -y "$2"`

    echo "$ssh_cmd"
	exit
    ;;
    *)
    build_artifacts
	exit
    ;;
esac
