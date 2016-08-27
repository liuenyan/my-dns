#ifndef DNSPACKETPARSER_HPP
#define DNSPACKETPARSER_HPP

#include "DNSHeader.hpp"
#include <arpa/inet.h>
#include <vector>

class DNSPacketParser{
public:
    DNSPacketParser(){};
    DNSPacketParser(const char *pkt, unsigned int pktlen) : pkt(pkt), pktlen(pktlen){ setDNSHeader(); };
    ~DNSPacketParser(){};
    void parse();
    void parse(const char *pkt, unsigned int pktlen);
    
private:
    const char *pkt;
    unsigned int pktlen;
    const DNSHeader *header;
    const char *body;
    int questions;
    int answerRRs;
    int authorityRRs;
    int additionalRRs;
    std::vector<QuestionField> questionFields;
    std::vector<ResourceRecord> answerFields;
    std::vector<ResourceRecord> authorityFields;
    std::vector<ResourceRecord> additionalFields;

    void clear();
    void setDNSHeader();
    void printPacketHeader();
    void printFields();
    const char *parseQuestion(const char *begin);
    const char *parseResourceRecord(const char *begin, int rrtype);
    bool isptr(unsigned char chr){ return (chr & 0xc0) == 0xc0;};
    unsigned short ptrOffset(unsigned short a){ return (a & 0x3fff); };
    char *parseData(const char *begin, int *data_offset);
    template <typename T> void printVector(const std::vector<T> &v);
};

#endif
