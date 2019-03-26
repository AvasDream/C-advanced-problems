## What is the difference between the commands gcc and aarch64-linux-gnu-gcc?

gcc compiles code for a x86_64 architecture and aarch64-linux-gnu-gcc
compiles for the aarch64 architecture.

## What is the difference between the commands ld and aarch64-linux-gnu-ld?

ld compiles code for a x86_64 architecture and aarch64-linux-gnu-ld
compiles for the aarch64 architecture. The different supported targets can be seen under the supported targets/emulations part of the help output.


## Which configure option(s) reflect(s) the architecture change?

Linux/arm64 4.11.0 Kernel Configuration
CONFIG_ARM64=y

The changes in the memory mapper bits due to the larger address.
Huge translationlookaside buffers are now supported.
CONFIG_CMDLINE gives the possibility of cmd options at build time.
Start of the physical address were the kernel is loaded is changed, also the aligned boundry is 2mb of size.
And a lot more changed in the config file. The diff file has 936 lines that would make approximately ~400 config changes.

## Which ARM platform has been activated by default?

ARMv8


## What is the name of the serial port that you need to pass to console=... in order to get console output on the PL011 device?

`console=ttyAMA0`


## What is the meaning of the host option for this configure script?

The host option specifys the architecture of the server on which dropbear will run after compiling.
So dropbear is able to crosscompile by itself.

## Please explain your choice for the machine and CPU types.

The machine type is suggested and after that  i wrote a bash script to test all possible cpus with this machine and only three worked. I choose the newest one but i dont think there is a difference.
