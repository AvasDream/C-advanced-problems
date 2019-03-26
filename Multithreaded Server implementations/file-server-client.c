#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <aio.h>

#define PORT 9000
#define BUFFERSIZE 4096
#define IP "127.0.0.1"
// Backlog defines the amount of queued Requests
#define BACKLOG 10
#define DEBUG 1

typedef struct server_type {
  struct sockaddr_in addr;
  int descriptor;
} server_type;


int createClient(server_type *server) {
  server->descriptor = socket( AF_INET, SOCK_STREAM, 0 );
  if (server->descriptor < 0) {
    printf("Error while creating socket.\n");
    return -1;
  }
  //IPv4
  server->addr.sin_family = AF_INET;
  // Localhost
  server->addr.sin_addr.s_addr = inet_addr(IP);
  // Port 80
  server->addr.sin_port = htons(PORT);
  return 0;
}
void cleanExit(server_type *server) {
  free(server);
  //exit(1);
}

int main(int argc, char const *argv[]) {
  // Check arguments
  if (argc < 2) {
    printf("Usage:\n");
    printf("./file-server-client <path>\n");
    printf("Note make file creates 'test' file in current dir.\n");
    exit(-1);
  }
  // Create Client Instance
  server_type *client = (server_type *) malloc(sizeof(server_type));
  int rc = createClient(client);
  if (rc != 0) {
    printf("Error while creating client\n");
    cleanExit(client);
  }
  char *path = argv[1];
  char buffer[BUFFERSIZE];
  // Create remote address
  struct sockaddr_in remote_addr;
  memset(&remote_addr, 0, sizeof(remote_addr));
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons(PORT);
  remote_addr.sin_addr.s_addr = inet_addr(IP);
  /* Connect to the server */
  if (connect(client->descriptor, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
  {
    printf("Error while connecting to client\n");
          exit(EXIT_FAILURE);
    }

  if (send(client->descriptor, path, strlen(path) + 1, NULL) < 0) {
    printf("Error while sending path: %s\n", path);
  } else  {
    recv(client->descriptor, buffer, BUFFERSIZE, 0);
    printf("Received File: %s\n", buffer);
  }
  cleanExit(client);
  return 0;
}
