#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_DATA_SIZE 100

/**
 * Get sockaddr
 *
 * ipv4 or ipv6
 */
void *get_in_addr(struct sockaddr *addr);

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: ./client [host] [port]\n");
    return 1;
  }

  char *host = argv[1];
  char *port = argv[2];

  int socket_fd;

  char buf[MAX_DATA_SIZE];

  struct addrinfo connection_info;
  memset(&connection_info, 0, sizeof connection_info);
  connection_info.ai_family = AF_UNSPEC;
  connection_info.ai_socktype = SOCK_STREAM;

  struct addrinfo *addr_results;
  int addrinfo_status =
      getaddrinfo(host, port, &connection_info, &addr_results);
  if (addrinfo_status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addrinfo_status));
    return 1;
  }

  struct addrinfo *p;
  for (p = addr_results; p != NULL; p = p->ai_next) {
    if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
        -1) {
      perror("client: socket");
      continue;
    }

    if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(socket_fd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 1;
  }

  char server_addr[INET6_ADDRSTRLEN];
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            server_addr, sizeof server_addr);
  printf("client: connecting to %s\n", server_addr);

  freeaddrinfo(addr_results);

  int numbytes = recv(socket_fd, buf, MAX_DATA_SIZE - 1, 0);
  if (numbytes == -1) {
    perror("recv");
    exit(1);
  }

  buf[numbytes] = '\0';
  printf("client: received '%s'\n", buf);

  close(socket_fd);
  return 0;
}

void *get_in_addr(struct sockaddr *addr) {
  if (addr->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)addr)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)addr)->sin6_addr);
}
