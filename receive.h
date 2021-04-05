/*
Michał Wójtowicz 308248
*/
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <vector>

#ifndef receive
#define receive

void print_as_bytes (unsigned char* buff, ssize_t length);

int check_data(unsigned char* buff, int ttl, u_int16_t pid);

int receive_package(int &sockfd, char addess_ip[20], int ttl, u_int16_t pid, std::vector<std::string> &v);

#endif