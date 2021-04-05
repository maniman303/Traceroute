/*
Michał Wójtowicz 308248
*/
#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <cassert>

#ifndef send
#define send

u_int16_t compute_icmp_checksum (const void *buff, int length);

int send_package(int &sockfd, struct sockaddr_in &recipient, int ttl, u_int16_t pid);

#endif