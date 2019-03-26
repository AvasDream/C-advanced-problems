#!/bin/bash
function start {
  cd $base
  ./hw6.sh ssh_cmd "easy_server &"
  ./hw6.sh ssh_cmd "complex_server &"
} 


function ssh_cmd {
  cd $base
  # Fingerprint of the vm is changing!

  ssh_cmd=`ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p 22222 -i $base/artifacts/ssh_key "$2"`
  #ssh_cmd=`./dropbearmulti dbclient root@localhost -p 22222 -i dropbear_key -y -y "$2"`

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
  make -j 5
  mv $base/artifacts/linux-4.11/arch/x86/boot/bzImage $base/artifacts/bzImage
  cd $base

  echo -e "build busybox"
  cd $base/artifacts
  wget -t 5 -c -N http://busybox.net/downloads/busybox-1.26.2.tar.bz2 > /dev/null
  tar xvjf $base/artifacts/busybox-1.26.2.tar.bz2 --keep-newer-files
  cd $base/artifacts/busybox-1.26.2/
  cp $base/busybox/config .config
  make clean
  make -j 5 > /dev/null
  mv busybox ..

  echo -e "build sysinfo"
  cd $base/sysinfo/src && make && make clean
  mv $base/sysinfo/src/sysinfo $base/artifacts/sysinfo

  echo -e "build dropbear"
  cd $base/artifacts
  wget -t 5 -c -N https://matt.ucc.asn.au/dropbear/releases/dropbear-2016.74.tar.bz2 > /dev/null
  tar xvjf $base/artifacts/dropbear-2016.74.tar.bz2
  cd $base/artifacts/dropbear-2016.74/
  ./configure --disable-shadow --disable-lastlog --disable-syslog --disable-wtmp --disable-wtmpx --disable-utmpx --disable-zlib > /dev/null
  make clean
  make -j 5 PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1 STATIC=1 strip > /dev/null
  mv dropbearmulti $base/artifacts/dropbearmulti
  cd $base/complexserver
  [ ! -e lib ] && mkdir lib
  cd lib 
  git submodule add -f https://github.com/oatpp/oatpp
  git submodule update --init
  cd ..
  clang++ -std=c++11 -static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive $(find "lib/oatpp/" -type f -name *.cpp) $(find "src/" -type f -name *.cpp) -I "lib" -D OATPP_USE_TARGET -D OATPP_TARGET_APP -D OATPP_DISABLE_ENV_OBJECT_COUNTERS -O2 -Wall -o run_app
  mv run_app $base/artifacts/complex_server
  cd $base/easyserver
  make clean && make all && make clean
  mv easy_server $base/artifacts/easy_server

  echo -e "Copying binaries to initrd"
  mkdir -p $base/initrd/bin
  cd $base/initrd/bin/
  rm ./sysinfo
  ln -s  $base/sysinfo ./sysinfo
  rm ./busybox
  ln -s  $base/artifacts/busybox ./busybox
  rm ./dropbearmulti
  ln -s $base/artifacts/dropbearmulti ./dropbearmulti
  rm ./complex_server
  ln -s $base/artifacts/complex_server ./complex_server
  rm ./easy_server
  ln -s $base/artifacts/easy_server ./easy_server
  echo -e "Zipping initrd"
  rm $base/artifacts/initrd.cpio
  mkdir $base/initrd/lib
  cd $base/artifacts/dropbear-2016.74
  cp $(gcc -print-file-name=ld-linux-x86-64.so.2) $base/initrd/lib/
  cp $(gcc -print-file-name=libc.so.6) $base/initrd/lib/
  cp $(gcc -print-file-name=libnss_files.so.2) $base/initrd/lib/
  cp $(gcc -print-file-name=libm.so.6) $base/initrd/lib/
  cp $(gcc -print-file-name=libpthread.so.0) $base/initrd/lib/
  cp $(gcc -print-file-name=libgcc_s.so.1) $base/initrd/lib/


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

function qemu {
    cd $base/initrd
    find . | cpio -L -v -o -H newc > $base/artifacts/initrd.cpio
    qemu-system-x86_64 -m 1024 -nographic -kernel $base/artifacts/bzImage -append "console=ttyS0 init=./init" -initrd $base/artifacts/initrd.cpio -netdev user,id=mynet0,hostfwd=tcp::22222-:22,hostfwd=tcp::8000-:8000,hostfwd=tcp::8001-:8001 -device virtio-net,netdev=mynet0
    return
} 

base=$(pwd)

case "$1" in
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
    ssh_cmd=`ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p 22222 -i $base/artifacts/ssh_key "$2"`
    echo "$ssh_cmd"
	exit
    ;;
  "start")
    start
  exit
    ;;
  "build")
    build_artifacts
  exit
    ;;
  *)
    [ ! -e artifacts ] && build_artifacts
    mate-terminal --working-directory=$base --command "./hw6.sh qemu" &
	exit
    ;;
esac