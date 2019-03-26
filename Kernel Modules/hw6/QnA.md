## What do the both mentioned commandline options do?

-S is the flag to not start the CPU automatically. and -gdb:Port opens a gdb Server at the specified Port for debugging.

## What do the Commands in gdb do?


*set architecture aarch64

Set architecture

*target remote tcp:127.0.0.1:1234

Set remote TCP connection

*symbol-file sourcecode/linux-4.11/vmlinux

Read symbol table

*b start_kernel

Set breakpoint

*c

continue

## What is a breakpoint?

A point in the execution flow where the execution halts and the state of the programm can be analyzed.

## Which registers are displayed by qemu? which by gdb? (Shorten the answers by summing things together. e.G.

Not sure how the answer is meant. But gdb is the debugging tool and shows us the registers of the by qemu virtualized CPU.

## when info is entered in qemu you can use tab-Completion to find more information objects. which other Information object are there?


balloon               block                 block-jobs            
blockstats            capture               chardev               
cpus                  cpustats              dump                  
history               hotpluggable-cpus     ioapic                
iothreads             irq                   jit                   
kvm                   lapic                 mem                   
memdev                memory-devices        mice                  
migrate               migrate_cache_size    migrate_capabilities  
migrate_parameters    mtree                 name                  
network               numa                  opcount               
pci                   pic                   profile               
qdm                   qom-tree              qtree                 
registers             rocker                rocker-of-dpa-flows   
rocker-of-dpa-groups  rocker-ports          roms                  
snapshots             spice                 status                
tlb                   tpm                   trace-events          
usb                   usbhost               usernet               
uuid                  version               vnc
