#include "DNSPacketSender.hpp"
#include "DNSHeader.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

DNSPacketSender::DNSPacketSender(struct sockaddr_in &addr)
{
    this->addr = addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

DNSPacketSender::DNSPacketSender(const char *host, unsigned short port)
{
    makeSockAddrIn(host, port);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

DNSPacketSender::~DNSPacketSender()
{
    close(sockfd);
}

void DNSPacketSender::makeSockAddrIn(const char *host, unsigned short port)
{
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(port);
    addr.sin_addr.s_addr = inet_addr(host);
}

char *DNSPacketSender::sendDNSQuery(const char *name, int &recvlen)
{
    int buflen;
    char *buffer = makeDNSPacket(name, buflen);

    sendto(sockfd, (void *)buffer, buflen, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    free(buffer);

    char recvbuf[1024];
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len;
    recvlen = recvfrom(sockfd, (void *)recvbuf, 1024, 0, (struct sockaddr *)&remote_addr, &remote_addr_len);
    
    char *recvdata = (char *)malloc(recvlen);
    memcpy(recvdata, recvbuf, recvlen);
    return recvdata;
}

char *DNSPacketSender::makeDNSPacket(const char *name, int &buflen)
{
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

    const char *domain_ptr = name;
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
    buflen = current - buffer;
    return buffer;
}
