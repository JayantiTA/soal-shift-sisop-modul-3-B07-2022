// source:
// https://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
  char *file_name = "hartakarun.zip";
  char buffer[BUFSIZ];
  int client_fd;
  int enable_reuseaddr = 1;
  int filestream;
  int server_fd;
  socklen_t client_len;
  ssize_t read_status;
  struct protoent *proto;
  struct sockaddr_in client_address, server_address;
  unsigned short port = 7702u;

  // Create a socket and listen to it.
  proto = getprotobyname("tcp");
  if (proto == NULL) {
    perror("getprotobyname");
    exit(EXIT_FAILURE);
  }
  server_fd = socket(AF_INET, SOCK_STREAM, proto->p_proto);
  if (server_fd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable_reuseaddr, sizeof(enable_reuseaddr)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(EXIT_FAILURE);
  }
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(port);
  if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 5) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "listening on port %d\n", port);

  while (1) {
    client_len = sizeof(client_address);
    printf("waiting for client\n");
    client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_len);
    filestream = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (filestream == -1) {
      perror("open");
      exit(EXIT_FAILURE);
    }
    do {
      read_status = read(client_fd, buffer, BUFSIZ);
      if (read_status == -1) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      if (write(filestream, buffer, read_status) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
      }
    } while (read_status > 0);
    close(filestream);
    close(client_fd);
  }
  exit(EXIT_SUCCESS);
}
