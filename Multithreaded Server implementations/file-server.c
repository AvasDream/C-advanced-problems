#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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


int createServer(server_type *server) {
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
  //Bind Address specified above. Assign address to socket.
  int rc = bind(server->descriptor, (struct sockaddr *) &server->addr, sizeof(server->addr));
  if (rc < 0) {
    printf("Error while assigning address.\nCode %d\n", rc);
    return -1;
  }
  // Put Socket in passive mode. Per default it is in active mode ( send data)
  rc = listen(server->descriptor, BACKLOG);
  if (rc < 0) {
    printf("Error while listening.\n");
    return -1;
  }
  // Address in use error elimination
  int optval = 1;
  setsockopt(server->descriptor, SOL_SOCKET, SO_REUSEADDR,
	     (const void *)&optval , sizeof(int));
  return 0;
}
void cleanExit(server_type *server) {
  free(server);
  //exit(1);
}
char *readFromFile(const char *filename)
{

    printf("filename: %s", filename );
    long int size = 0;
    FILE *file = fopen(filename, "r");
    if(!file) {
        fputs("File error.\n", stderr);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    char *result = (char *) malloc(size);
    if(!result) {
        fputs("Memory error.\n", stderr);
        return NULL;
    }
    if(fread(result, 1, size, file) != size) {
        fputs("Read error.\n", stderr);
        return NULL;
    }
    fclose(file);
    return result;
}
int writeToClient(int descr,const char *message) {
  if(DEBUG) printf("message: %s size:%d\n", message, strlen(message)+1);
  int rc = write(descr, message, strlen(message)+1);
  if (rc < 0) {
      printf("Error writting to client\n");
      return -1;
  }
  return 0;
}
int main(int argc, char const *argv[]) {
  // Create Server Instance
  server_type *server = (server_type *) malloc(sizeof(server_type));
  int rc = createServer(server);
  if (rc != 0) {
    printf("Error while creating Server\n");
    cleanExit(server);
  }

  // Prepare main loop
  int notdone = 1;
  fd_set readfds;
  int activeConnections = 0;
  char inputBuffer[BUFFERSIZE];
  char OutputString;
  // Client data
  struct sockaddr_in clientaddr;
  int clientFiledescriptor;
  int clientLength = sizeof(clientaddr);
  // Main loop
  while (notdone) {
    FD_ZERO(&readfds);          /* initialize the fd set */
    FD_SET(server->descriptor, &readfds); /* add socket fd */
    FD_SET(0, &readfds);        /* add stdin fd (0) */
    int rc = select(server->descriptor+1, &readfds, 0, 0, 0);
    if (rc < 0) {
      printf("Error while creating Server\n");
      close(server->descriptor);
      cleanExit(server);
    }
    //Terminate Server on Host User Input
    if (FD_ISSET(0, &readfds)) {
      notdone = 0;
    }
    // Client Request
    if (FD_ISSET(server->descriptor, &readfds)) {
      //accept request
      clientFiledescriptor= accept(server->descriptor, (struct sockaddr *) &clientaddr, &clientLength);
      if (clientFiledescriptor< 0) {
          printf("Error while Accepting Client\n");
          close(clientFiledescriptor);
          cleanExit(server);
          activeConnections++;
      }
      //read from request
      bzero(inputBuffer, BUFFERSIZE);
      rc = read(clientFiledescriptor, inputBuffer, BUFFERSIZE);
      if (rc < 0) {
          printf("Error reading from client\n");
          close(clientFiledescriptor);
          cleanExit(server);
      }
      if(DEBUG) printf("Received: %s\nlength %d\n", inputBuffer, strlen(inputBuffer));
      char *path = inputBuffer;
      if(DEBUG) printf("Path: %s\n", path);
      if(DEBUG) printf("path length: %d\n", strlen(path));
      char *tmp;

      char *pos = inputBuffer;
      if ((pos=strchr(pos, '\n')) != NULL) {
        *pos = '\0';
      }

      tmp = readFromFile(path);
      if(DEBUG) printf("File output: %s\n", tmp);
      if (tmp == NULL) {
        rc = writeToClient(clientFiledescriptor, "Error while reading file\n");
        close(clientFiledescriptor);
        cleanExit(server);
      } else {
        printf("FileBuffer output:%s\n", inputBuffer);
        writeToClient(clientFiledescriptor, tmp);
      }
      close(clientFiledescriptor);
    }
  }
  cleanExit(server);
  return 0;
}
