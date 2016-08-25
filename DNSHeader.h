#ifndef DNSHEADER_H
#define DNSHEADER_H

#pragma pack (1)

struct DNSHeader{
    unsigned short transactionID;
    unsigned short flags;
    unsigned short questions;
    unsigned short answerRRs;
    unsigned short authorityRRs;
    unsigned short additionalRRs;
};

typedef struct DNSHeader DNSHeader;

struct QuestionField{
    char *name;
    unsigned short type_;
    unsigned short class_;
};

typedef struct QuestionField QuestionField;

struct ResourceRecord{
    char *name;
    unsigned short type_;
    unsigned short class_;
    unsigned int ttl;
    unsigned short data_len;
    char *data;
};
typedef struct ResourceRecord ResourceRecord;

#endif
