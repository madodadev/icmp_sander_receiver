#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

unsigned short calculate_checksum(void *b, int len);

#define BUFEER_SIZE 1500

int main(void) {
    int fd_sock, recv_bytes, send_bytes;
    struct sockaddr_in recv_addr;
    socklen_t recv_addr_len = sizeof(recv_addr);
    struct icmphdr * icmp_hdr;
    int icmp_hdr_size = sizeof(icmp_hdr);
    struct iphdr * ip_hdr;

    char *data;
    
    char buffer[BUFEER_SIZE];

    if((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        perror("Creating Row Socket");
        exit(EXIT_FAILURE);
    }

    printf("Listen for ICMP...\n");

    while (1)
    {
        recv_bytes = recvfrom(fd_sock, buffer, BUFEER_SIZE, 0, 
        (struct sockaddr *)&recv_addr, &recv_addr_len);
        
        if(recv_bytes < 0) {
            perror("Recev Packet");
            exit(EXIT_FAILURE);
        }
        printf("Packet Len %ld\n", recv_bytes);
        //Extrat IP Header
        ip_hdr = (struct iphdr *)buffer;
        printf("Recv IP: %s\n", inet_ntoa(recv_addr.sin_addr));
        int ip_hdr_len = ip_hdr->ihl * 4;

        //Extract ICMP
        icmp_hdr = (struct icmphdr *)(buffer + ip_hdr_len);
        printf("Size Of ICMP Header %ld\n", sizeof(icmp_hdr));
        printf("ICMP Code %d \n", icmp_hdr->code);
        printf("ICMP ID %d\n", icmp_hdr->un.echo.id);
        printf("ICMP Sequence %d\n", icmp_hdr->un.echo.sequence);

        data = (char *) icmp_hdr + sizeof(icmp_hdr);

        printf("Data: %s\n", data);

        printf("\n\n");
    }
    

}


unsigned short calculate_checksum(void *b, int len) {    
    unsigned short *buf = b; 
    unsigned int sum = 0;
    unsigned short result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}