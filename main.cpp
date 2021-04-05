/*
Michał Wójtowicz 308248
*/
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <vector>
#include <regex>
#include "send.h"
#include "receive.h"

//Funkcja zliczająca poprawne odpowiedzi i zwracająca wyliczony, uśredniony czas.
float inc_replies(int &replies, struct timeval &tv)
{
    float time_used = 0;
    replies++;
    if (replies == 3)
    {
        time_used =(1000000 - tv.tv_usec) / 3000.0;
        tv.tv_usec = 0;
    }
    return time_used;
}

//Najważniejsza funkcja programu, wysyła bez czekania na odpowiedzi 3 pakiety o ustalonym TTL.
//Następnie bez aktywnego czekania odbiera 3 pakiety (lub kończy się po 1 sek).
//Wypisuje uśredniony czas jeżeli doszły 3 poprawne odpowiedzi.
//Zwraca -1 jeżeli dojdzie do błędu, 1 gdy nie dojdzie odpowiedź z adresu docelowego, 2 gdy dojdzie odpowiedź z adresu docelowego.
int send_and_receive(int &sockfd, char addess_ip[20], struct sockaddr_in &recipient, int ttl, u_int16_t pid)
{
    setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    for (int i = 1; i <= 3; i++)
        if (send_package(sockfd, recipient, ttl, pid) <= -1) return -1;
    std::vector<std::string> v;
    int replies = 0, stat;
    float time_used = 0;
    bool final = false;
    fd_set descriptors; 
    FD_ZERO (&descriptors); 
    FD_SET (sockfd, &descriptors); 
    struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0; 
    while(tv.tv_usec > 0 || tv.tv_sec > 0)
    {
        int ready = select (sockfd+1, &descriptors, NULL, NULL, &tv);
        if (ready < 0) return -1;
        if (ready > 0)
        {
            stat = receive_package(sockfd, addess_ip, ttl, pid, v);
            switch (stat)
            {
                case -1:
                    return -1;
                    break;
                case 2:
                    final = true;
                    time_used = inc_replies(replies, tv);
                    break;
                case 1:
                    time_used = inc_replies(replies, tv);
                    break;
            }
        }
    }
    if (replies == 3) printf("%f ms\n", time_used);
    else if (replies > 0) printf("???\n");
    else printf("*\n");
    if (final) return 2;
    return 1;
}

//Funkcja wykorzystująca wyrażenia regularne do sprawdzania poprawności adresów.
bool check_address(std::string s)
{
    std::string regx = "^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$";
    bool found = regex_match(s, std::regex(regx));
    return found;
}

//Główna funkcja, ustawia socket-a, pobiera adres z argumentów oraz w pętli wysyła i odbiera pakiety.
int main(int argc, char** argv)
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        printf("Yo, run this program as sudo.\n");
        return 1;
    }
    if (argc != 2)
    {
        printf("The arguments do not mach.\n");
        return 1;
    }
    u_int16_t pid = getpid();
    struct sockaddr_in recipient;
    std::string astring(argv[1]);
    if (!check_address(astring))
    {
        printf("The argument is not a proper IPv4 address.\n");
        return 1;
    }
    char* address_ip = &astring[0]; //156.17.4.1 jakiś adres ii uwr, 94.23.242.48 wikipedia
    printf("Hello, I'm %d\n", pid);
    bzero (&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET; 
    inet_pton(AF_INET, address_ip, &recipient.sin_addr); //wczytaj ip z konsoli/argumentów programu i regexem spr czy dobry adres ip
    int res;
    for (u_int16_t i = 1; i <= 30; i++)
    {
        res = send_and_receive(sockfd, address_ip, recipient, i, pid);
        if (res == -1 || res == 2)
        {
            //printf("Jumps: %d\n", i); // przydatne przy porównywaniu do innych traceroutów
            i = 31;
        }
    }
    return 1;
}