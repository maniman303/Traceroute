/*
Michał Wójtowicz 308248
*/
#include "receive.h"

//Funckja z wykładu do wypisywania w bajtach odebranych danych
void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);
}

//Na podstawie funkcji z wykładu.
//Sprawdza poprawność i zgodność odebranego pakietu.
//Patrzy czy pakiet jest wysłany przez tą konkretną instancję programu.
//Czy jest wysłany z konkretnej tury.
int check_data(unsigned char* buff, int ttl, u_int16_t pid)
{
    
    u_int8_t type = *buff;
    int s = ((type == 11) ? 28 : 0);
    u_int16_t id = *(buff+5+s) * 256 + *(buff+4+s);
    u_int16_t seq = *(buff+7+s) * 256 + *(buff+6+s);
    if ((type == 0 || type == 11) && id == pid && seq == ttl) return 1;
    return -1;
}

//Funkcja odbierająca pakiet.
//Zwraca -1 gdy błąd, 0 gdy pakiet nieistotny, 1 gdy time exceeded, 2 gdy echo reply czyli dostaliśmy odpowiedź z adresu końcowego.
int receive_package(int &sockfd, char addess_ip[20], int ttl, u_int16_t pid, std::vector<std::string> &v)
{
    struct sockaddr_in 	sender;
	socklen_t 			sender_len = sizeof(sender);
	u_int8_t 			buffer[IP_MAXPACKET];
	
    ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
	if (packet_len < 0)
    {
		fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); //prawdopodobnie w gnieździe nie ma pakietu
		return -1;
	}

	struct ip* ip_header = (struct ip*) buffer;
	ssize_t ip_header_len = 4 * ip_header->ip_hl;

    if (check_data(buffer + ip_header_len, ttl, pid) != 1) return 0;

    char sender_ip_str[20];
	inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
    bool print = true;
    for (long unsigned int i = 1; i <= v.size(); i++)
        if (v[i-1] == std::string(sender_ip_str)) print = false;
    if (print)
    {
        printf ("%s ", sender_ip_str);
        v.push_back(std::string(sender_ip_str));
    }
    if (std::string(addess_ip) == std::string(sender_ip_str)) return 2;
    return 1;
}