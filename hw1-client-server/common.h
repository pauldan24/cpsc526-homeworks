#ifndef COMMON_H
#define COMMON_H

/**
 * Get the sockaddr, ipv4 or ipv6
 */
void *get_in_addr(struct sockaddr *addr);

void sigchild_handler(int s);

#endif /* COMMON_H */
