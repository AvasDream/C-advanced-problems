## Do not put a directory to much in the .cpio file

`find . | cpio -L -v -o -H newc > ../initrd.cpio`


##ToDo

- hw1 script
- symbolic links to /artifacts
- clean hw1 folder
- merge


Structure:
hw1
   ├── busybox
   │   └── config
   ├── hw1.sh
   ├── initrd-busybox
   │   ├── bin
   │   │   ├── busybox -> ../../artifacts/busybox
   │   │   └── sysinfo -> ../../artifacts/sysinfo
   │   └── init
   ├── initrd-sysinfo
   │   └── bin
   │       └── sysinfo -> ../../artifacts/sysinfo
   ├── kernel
   │   └── config
   ├── QnA.md
   ├── README.md
   └── sysinfo
       └── src
           ├── Makefile
           └── sysinfo.c
