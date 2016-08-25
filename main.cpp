#include "DNSHeader.h"
#include "DNSPacketParser.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

char* makeDNSQuery(const char *domain, size_t *buflen){
    DNSHeader header;
    header.transactionID = htons(0x8008);
    header.flags = htons(0x0180);
    header.questions = htons(1);
    header.answerRRs = htons(0);
    header.authorityRRs = htons(0);
    header.additionalRRs = htons(0);
    const unsigned short queryType = htons(1);
    const unsigned short queryClass = htons(1);

    char *buffer = (char *)malloc(1024);
    char *current = buffer;
    memset(buffer, 0, 1024);

    memcpy(current, &header, sizeof(struct DNSHeader));
    current += sizeof(struct DNSHeader);

    const char *domain_ptr = domain;
    const char *dot_ptr = NULL;
    unsigned char len;

    while((dot_ptr = strchr(domain_ptr, '.')) != NULL){
        len = dot_ptr - domain_ptr;
        memcpy(current, &len, sizeof(unsigned char));
        current += sizeof(unsigned char);
        memcpy(current, domain_ptr, len);
        current += len;
        domain_ptr = dot_ptr + sizeof(char);
    }
    len = strlen(domain_ptr);
    memcpy(current, &len, sizeof(unsigned char));
    current += sizeof(unsigned char);
    memcpy(current, domain_ptr, len);
    current += len;
    memset(current, 0, sizeof(unsigned char));
    current += sizeof(unsigned char);

    memcpy(current, &queryType, sizeof(unsigned short));
    current += sizeof(unsigned short);
    memcpy(current, &queryClass, sizeof(unsigned short));
    current += sizeof(unsigned short);
    *buflen = current - buffer;
    return buffer;
}

int main(int argc, char *argv[]){
    if (argc < 2){
        cout << "usage: mydns name"<<endl;
        exit(1);
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53);
    addr.sin_addr.s_addr = inet_addr("114.114.114.114");

    size_t buflen;
    char *buffer = makeDNSQuery(argv[1], &buflen);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sendto(sockfd, (void *)buffer, buflen, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    free(buffer);

    char recvbuf[1024];
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len;
    ssize_t recvlen = recvfrom(sockfd, (void *)recvbuf, 1024, 0, (struct sockaddr *)&remote_addr, &remote_addr_len);
    close(sockfd);
    DNSPacketParser *parser = new DNSPacketParser(recvbuf, recvlen);
    parser->parse();
    delete parser;
}
