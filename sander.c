#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

unsigned short calculate_checksum(void *b, int len);

#define ICMP_ECHO 8

struct icmp_header {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short id;
    unsigned short sequence;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <destination_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int fd_sock, send_bytes;
    struct sockaddr_in dest_addr;
    struct icmp_header icmp_hdr;
    int icmp_hdr_size = sizeof(icmp_hdr);

    char data[1400] = "A luta continua";
    
    int packet_size = sizeof(icmp_hdr) + sizeof(data);
    char packet[packet_size];

    if((fd_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
    {
        perror("Creating Row Socket");
        exit(EXIT_FAILURE);
    }

    //SET destation Address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    //SET ICMP PACKET
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.id = 111;
    icmp_hdr.sequence = 1;

    //Make Packet
    memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
    memcpy(packet + sizeof(icmp_hdr), data, sizeof(data));

    //Calculate checksum
    icmp_hdr.checksum = calculate_checksum(packet, packet_size);
    memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));

    send_bytes = sendto(fd_sock, packet, packet_size, 0, 
    (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    if(send_bytes < 0) {
        perror("Sending Packet");
        exit(EXIT_FAILURE);
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