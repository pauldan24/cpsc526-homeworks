#include "common.h"


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
