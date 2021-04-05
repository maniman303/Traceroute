/*
Michał Wójtowicz 308248
*/
#include "send.h"

//Funckja z wykładu, wyliczająca sumę kontrolną
u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t *ptr = (const u_int16_t *) buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

//Funkcja wysyłająca pakiet.
//Oznacza pakiet PID programu, oraz aktualnym ttl-em.
//Zwraca -1 gdy wystąpił błąd, 1 gdy wysłano pakiet.
int send_package(int &sockfd, struct sockaddr_in &recipient, int ttl, u_int16_t pid)
{
    struct icmp header;
    header.icmp_type = ICMP_ECHO; 
    header.icmp_code = 0; 
    header.icmp_hun.ih_idseq.icd_id = pid; //numer procesu, do określenia, że to od nas
    header.icmp_hun.ih_idseq.icd_seq = ttl; //oznaczenie, z której tury pochodzi pakiet
    header.icmp_cksum = 0; 
    header.icmp_cksum = compute_icmp_checksum ( 
        (u_int16_t*)&header, sizeof(header));
    ssize_t bytes_sent = sendto ( 
        sockfd, 
        &header,
        sizeof(header), 
        0,
        (struct sockaddr*)&recipient,
        sizeof(recipient));
    if (bytes_sent <= 0)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        return -1;
    }
    return 1;
}