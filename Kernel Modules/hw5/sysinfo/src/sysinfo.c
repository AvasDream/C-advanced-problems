#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(int argc, char *argv[])
{
    struct utsname uName;
    struct sysinfo info;
    char name[255];
    if (sysinfo(&info) != 0)
    {
        perror("Could not get system info.");
        exit(1);
    }
    if (uname(&uName) != 0)
    {
        perror("Could not get uname info.");
        exit(1);
    }
    if(gethostname(name, 255) != 0){
        perror("Could not get host name.");
        exit(2);
    }
    printf("---- gethostname Information ----\n\n");
    printf("Hostname: %s\n", name);
    printf("---- sysinfo Information ----\n");
    printf("Uptime: %ld second(s)\n", info.uptime);
    printf("Process count: %d\n", info.procs);
    printf("Total RAM: %lu Byte(s)\n", info.totalram);
    printf("Free RAM: %lu Byte(s)\n", info.freeram);
    printf("Page size: %u Byte(s)\n", info.mem_unit);
    printf("---- utsname Information ----\n");
    printf("system name: %s\n", uName.sysname);
    printf("node name: %s\n", uName.nodename);
    printf("release: %s\n", uName.release);
    printf("version: %s\n", uName.version);
    printf("machine: %s\n", uName.machine);
    exit(0);
}
