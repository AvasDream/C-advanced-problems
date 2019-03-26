#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "sysinfo.h"

int main()
{
    int create_socket, new_socket;
    socklen_t addrlen;
    int bufsize = 2048;
    char *rx_buffer = malloc(bufsize);
    char *tx_buffer = malloc(bufsize);
    size_t tx_size = 0;
    struct sockaddr_in address;

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0)
    {
        printf("The socket was created\n");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8001);

    if (bind(create_socket, (struct sockaddr *)&address, sizeof(address)) == 0)
    {
        printf("Binding Socket\n");
    }

    while (1)
    {
        if (listen(create_socket, 10) < 0)
        {
            perror("server: listen");
            exit(1);
        }

        if ((new_socket = accept(create_socket, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("server: accept");
            exit(1);
        }

        if (new_socket > 0)
        {
            printf("The Client is connected...\n");
        }

        recv(new_socket, rx_buffer, bufsize, 0);
        printf("%s\n", rx_buffer);
        print_info(tx_buffer, &tx_size);
        write(new_socket, tx_buffer, tx_size);
        close(new_socket);
    }
    close(create_socket);
    return 0;
}