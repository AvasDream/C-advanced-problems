#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>


int main(int argc, char const *argv[]) {
  // Number of queued connections
  int BACKLOG = 64;
  int PORT = 9000;
  struct sockaddr_in server;
  struct sockaddr_in client;
  int client_descriptor;
  int server_descriptor;
  socklen_t client_length =sizeof(client_descriptor);
  char *buffer[256];
  /*
    Normally only a single protocol
    exists to support a particular socket type within a given protocol family, in which case protocol can  be
    specified as 0.
  */
  server_descriptor = socket( AF_INET, SOCK_STREAM, 0 );
  if (server_descriptor < 0) {
    printf("Error while creating socket.\n");
    return -1;
  }
  //IPv4
  server.sin_family = AF_INET;
  // Localhost
  server.sin_addr.s_addr = inet_addr("0.0.0.0");
  // Port 80
  server.sin_port = htons(PORT);
  //Bind Address specified above. Assign address to socket.
  int rc = bind(server_descriptor, (struct sockaddr *) &server, sizeof(server));
  if (rc < 0) {
    printf("Error while assigning address.\nCode %d\n", rc);
    return -1;
  }
  // Put Socket in passive mode. Per default it is in active mode ( send data)
  rc = listen(server_descriptor, BACKLOG);
  if (rc < 0) {
    printf("Error while listening.\n");
    return -1;
  }
  client_descriptor = accept(server_descriptor, (struct sockaddr *) &client, &client_length);

  while (1) {
    // Accept Clients
    if (client_descriptor < 0) {
      printf("Error while accepting Client\n");
      return -1;
    }
    //Overwrite Buffer
    bzero(buffer,256);
    rc = read(client_descriptor, buffer, sizeof(buffer));
    if (rc < 0) {
      printf("Erro while reading from client\n");
      return -1;
    }
    printf("Received data: %s\n",buffer);
    rc = write(client_descriptor,"hello",6);
    if (rc < 0) {
      printf("Erro while sending to client\n");
      return -1;
    }
  }
  close(client_descriptor);
  close(server_descriptor);
  return 0;
}
