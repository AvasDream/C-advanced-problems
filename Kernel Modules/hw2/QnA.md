## What does the 2>&1 do in the given command? Why is it needed?

It does a redirect from stderr to stdout. Because with the output from stderr we get a table with the device types and not just a list.


## Which Kernel Options did you activate?

- Multi User Support.For Dropbear
- TCP/IP Networking
- Bus options (PCI, PCMCIA, EISA, MCA, ISA)
- - PCI Support
- Device Drivers
- - Network device support
- - - Network device support


## How is the DHCP lease applied to the interface by uhdcpc?

All of the paramaters are set in enviromental variables, The script should configure the interface, and set any other relavent parameters (default gateway, dns server, etc)

[READ MORE](https://git.busybox.net/busybox/tree/networking/udhcp/README.udhcpc?h=1_1_stable&id=179f41778880d0a233435f5c9ed04e17316f479a)

## What does the example simple.script do on a new lease?

It binds the leased ip address to the interface, removes all existing routes to it and sets the dhcp-server address

## What does the example simple.script do on a new lease?

Set the ip to 0.0.0.0 then to the new value and reset the interface eth0.

## Which applets does the dropbearmulti binary support and what is their purpose?

dropbear - ssh server
dbclient - ssh client
dropbearkey - like ssh-keygen
dropbearconvert - to convert Openssh keys to one line dropbear format


## What is shadow and how can we login to the system if it is not used?

In linux the password hashes for the users are stored in the shadow file. But if we use a SSH key, we dont need one. We could also set the password hashes directly in the passwd file.

e.g. this
vince:$6$bxwJfzor$MUhUWO0MUgdkWfPPEydqgZpm.YtPMI/gaM4lVqhP21LFNWmSJ821kvJnIyoODYtBh.SF9aR7ciQBRCcw5bgjX0:0:0:root:/root:/bin/bash
would create the user root with the password test


## What are the UID gid numbers?

User id and group id with this values you can grant permission to one user or a whole user group.
All the users are stored in /etc/passwd and all the groups in /etc/groups


## How can we display the public key portion of the keyfile?

We can use grep.

## Which file within a users direcroty.... ?

The ~/.ssh/authorized_keys file.


## What is the difference... dlopen()?

dlopen implements an interface to directly link dynamic libraries in your code. Normally this is
done via some compiler operation at compile time and not via a function call on runtime.
Because of that file is not seeing any dynamic libraries on the dropbear binary.


## Hostfwd= ?

This is simple portforwarding from our virtual interface on the Guest VM to the physical (In our case also virtual)
interface on the Host VM.

## What is the full command?


ssh_cmd=`ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p 22222
22222 -i $base/artifacts/ssh_key "$2"`

or

ssh_cmd=`./dropbearmulti dbclient root@localhost -p 22222 -i dropbear_key -y -y "$2"`


## PTY ?

Pty stands for pseudoterminal and is a pair of virtaul char devices (master/slave).
The devpts fs contains all the slaves of the pty devices which
contains all the slaves.
