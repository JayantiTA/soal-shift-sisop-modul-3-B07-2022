// source:
// https://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h> // getprotobyname
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char protoname[] = "tcp";
  char *file_path = "hartakarun.zip";
  char buffer[BUFSIZ];
  int client_sockfd;
  int enable = 1;
  int filefd;
  int server_sockfd;
  socklen_t client_len;
  ssize_t read_return;
  struct protoent *protoent;
  struct sockaddr_in client_address, server_address;
  unsigned short server_port = 7702u;

  // Create a socket and listen to it.
  protoent = getprotobyname(protoname);
  if (protoent == NULL) {
    perror("getprotobyname");
    exit(EXIT_FAILURE);
  }
  server_sockfd = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
  if (server_sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(EXIT_FAILURE);
  }
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(server_port);
  if (bind(server_sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  if (listen(server_sockfd, 5) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "listening on port %d\n", server_port);

  while (1) {
    client_len = sizeof(client_address);
    puts("waiting for client");
    client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &client_len);
    filefd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (filefd == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }
    do {
      read_return = read(client_sockfd, buffer, BUFSIZ);
      if (read_return == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      if (write(filefd, buffer, read_return) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
      }
    } while (read_return > 0);
    close(filefd);
    close(client_sockfd);
  }
  exit(EXIT_SUCCESS);
}
