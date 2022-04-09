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

int main(int argc, char **argv)
{
  char *file_name;
  char *hostname = "127.0.0.1";
  char buffer[BUFSIZ];
  in_addr_t in_address;
  int filestream;
  int sock;
  ssize_t read_status;
  struct hostent *host;
  struct protoent *proto;
  struct sockaddr_in socket_address;
  unsigned short port = 7702;

  if (argc > 1) {
    if (strcmp(argv[1], "send") == 0) {
      if (argc > 2) {
        file_name = argv[2];
      } else {
        exit(EXIT_FAILURE);
      }
    } else {
      exit(EXIT_FAILURE);
    }
  }

  filestream = open(file_name, O_RDONLY);
  if (filestream == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  // Get socket.
  proto = getprotobyname("tcp");
  if (proto == NULL) {
    perror("getprotobyname");
    exit(EXIT_FAILURE);
  }
  sock = socket(AF_INET, SOCK_STREAM, proto->p_proto);
  if (sock == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  // Prepare socket_address.
  host = gethostbyname(hostname);
  if (host == NULL) {
    fprintf(stderr, "error: gethostbyname(\"%s\")\n", hostname);
    exit(EXIT_FAILURE);
  }
  in_address = inet_addr(inet_ntoa(*(struct in_addr*)*(host->h_addr_list)));
  if (in_address == (in_addr_t)-1) {
    fprintf(stderr, "error: inet_addr(\"%s\")\n", *(host->h_addr_list));
    exit(EXIT_FAILURE);
  }
  socket_address.sin_addr.s_addr = in_address;
  socket_address.sin_family = AF_INET;
  socket_address.sin_port = htons(port);
  // Do the actual connection.
  if (connect(sock, (struct sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  while (1) {
    read_status = read(filestream, buffer, BUFSIZ);
    if (read_status == 0) break;
    if (read_status == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    if (write(sock, buffer, read_status) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }
  close(filestream);
  exit(EXIT_SUCCESS);
}
