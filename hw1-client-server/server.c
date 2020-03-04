#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PENDING_CONNECTIONS 10

void sigchild_handler(int s);

/**
 * Get sockaddr, ipv4 or ipv6
 */
void *get_in_addr(struct sockaddr *addr);

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: ./server [port number]\n");
    return 1;
  }

  char *port = argv[1];

  // Listen to incoming connections on socket_fd
  int socket_fd;

  // Used for new connections
  int client_socket_fd;

  struct addrinfo connection_info;
  struct addrinfo *addr_results;

  struct sigaction signal_action;

  memset(&connection_info, 0, sizeof connection_info);
  connection_info.ai_family = AF_UNSPEC;     // use ipv4 or ipv6
  connection_info.ai_socktype = SOCK_STREAM; // tcp
  connection_info.ai_flags = AI_PASSIVE;     // fill in host IP address

  // Start looking for potential clients that trying to connect to port
  int addrinfo_status =
      getaddrinfo(NULL, port, &connection_info, &addr_results);
  if (addrinfo_status != 0) {
    fprintf(stderr, "Failed to get address info: %s\n",
            gai_strerror(addrinfo_status));
    return 1;
  }

  int yes = 1;
  struct addrinfo *p;
  for (p = addr_results; p != NULL; p = p->ai_next) {
    if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
        -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
        -1) {
      perror("setsockopt");
      return 1;
    }

    if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(socket_fd);
      perror("server: bind");
      continue;
    }

    break;
  }

  // free the memory
  freeaddrinfo(addr_results);

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    return 1;
  }

  if (listen(socket_fd, MAX_PENDING_CONNECTIONS) == -1) {
    perror("listen");
    return 1;
  }

  signal_action.sa_handler = sigchild_handler;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = SA_RESTART;

  printf("server: listening to incoming connections on localhost:%s ...\n",
         port);

  // Connector's address information
  struct sockaddr_storage incoming_addr;
  char incoming_addr_str[INET6_ADDRSTRLEN];
  while (true) {
    socklen_t sin_size = sizeof incoming_addr;
    client_socket_fd =
        accept(socket_fd, (struct sockaddr *)&incoming_addr, &sin_size);

    if (client_socket_fd == -1) {
      perror("accept");
      continue;
    }

    void *addr = get_in_addr((struct sockaddr *)&incoming_addr);
    // convert ipv4/ipv6 from binary to printable string
    inet_ntop(incoming_addr.ss_family, addr, incoming_addr_str,
              sizeof incoming_addr_str);
    printf("server: conneced from %s\n", incoming_addr_str);

    if (!fork()) {
      close(socket_fd);

      if (send(client_socket_fd, "Hello, world", 12, 0) == -1) {
        perror("send");
      }

      close(client_socket_fd);
      exit(0);
    }
  }

  return 0;
}

void *get_in_addr(struct sockaddr *addr) {
  if (addr->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)addr)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)addr)->sin6_addr);
}

void sigchild_handler(int s) {
  int saved_err = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_err;
}
