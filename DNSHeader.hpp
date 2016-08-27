#ifndef DNSHEADER_HPP
#define DNSHEADER_HPP

#include <string>

#pragma pack (1)

struct DNSHeader{
    unsigned short transactionID;
    unsigned short flags;
    unsigned short questions;
    unsigned short answerRRs;
    unsigned short authorityRRs;
    unsigned short additionalRRs;
};

struct QuestionField{
    std::string name;
    unsigned short type_;
    unsigned short class_;
};
std::ostream & operator<<(std::ostream &os, const QuestionField &q);

struct ResourceRecord{
    std::string name;
    unsigned short type_;
    unsigned short class_;
    unsigned int ttl;
    unsigned short data_len;
    std::string data;
};
std::ostream & operator<<(std::ostream &os, const ResourceRecord &r);

#endif
