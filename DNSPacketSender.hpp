#ifndef DNSPACKETSENDER_H
#define DNSPACKETSENDER_H

#include <arpa/inet.h>

class DNSPacketSender {
public:
    DNSPacketSender(struct sockaddr_in &addr);
    DNSPacketSender(const char *host, unsigned short port);
    ~DNSPacketSender();
    char *sendDNSQuery(const char *name, int &recvlen);

private:
    int sockfd;
    struct sockaddr_in addr;

    void makeSockAddrIn(const char *host, unsigned short port);
    char *makeDNSPacket(const char *name, int &buflen);
};

#endif
