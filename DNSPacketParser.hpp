#ifndef DNSPACKETPARSER_HPP
#define DNSPACKETPARSER_HPP

#include "DNSHeader.h"
#include <arpa/inet.h>

class DNSPacketParser{
public:
    DNSPacketParser(const char *pkt, const unsigned int pktlen);
    ~DNSPacketParser(){};
    void printPacketHeader();
    const char *parseQuestion(const char *begin);
    const char *parseResourceRecord(const char *begin);
    bool isptr(unsigned char chr){ return (chr & 0xc0) == 0xc0;};
    unsigned short ptrOffset(unsigned short a){ return (a & 0x3fff); };
    char *parseData(const char *begin, int *data_offset);
    void parse();
    
private:
    const char *pkt;
    const unsigned int pktlen;
    const DNSHeader *header;
    const char *body;
    int questions;
    int answerRRs;
    int authorityRRs;
    int additionalRRs;
};

#endif
