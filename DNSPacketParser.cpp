#include "DNSPacketParser.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <arpa/inet.h>

using namespace std;

DNSPacketParser::DNSPacketParser(const char *pkt, const unsigned int pktlen)
    : pkt(pkt), pktlen(pktlen)
{
    header = (const DNSHeader *) pkt;
    body = (char *)header + sizeof(DNSHeader);
    questions = ntohs(header->questions);
    answerRRs = ntohs(header->answerRRs);
    authorityRRs = ntohs(header->authorityRRs);
    additionalRRs = ntohs(header->additionalRRs);
}

void DNSPacketParser::printPacketHeader()
{
    cout << "Questions: " << questions <<endl; 
    cout << "AnswerRRs: " << answerRRs <<endl; 
    cout << "AuthorityRRs: " << authorityRRs <<endl;
    cout << "AdditionalRRs: " << additionalRRs <<endl; 
}

const char *DNSPacketParser::parseQuestion(const char *begin) {
    char buf[1024];
    const char *p = begin;
    char *bufptr = buf;
    unsigned char num = *p;
    p++;

    while (num != 0){
        memcpy(bufptr, p, num);
        bufptr += num;
        p += num;
        num = *p;
        p++;
        if(num != 0){
            memset(bufptr, '.', 1);
            bufptr++;
        }
    }
    *bufptr = 0;

    unsigned short type_ = ntohs(*(unsigned short *)p);
    p += 2;
    unsigned short class_ = ntohs(*(unsigned short *)p);
    p += 2;

    QuestionField question;
    question.name = string(buf);
    question.type_ = type_;
    question.class_ = class_;
    questionFields.push_back(question);
    return p; 
}

const char *DNSPacketParser::parseResourceRecord(const char *begin, int rrtype)
{
    char *name, *rr_data;
    const char *p = begin;
    int data_offset;
    ResourceRecord rr;

    name = parseData(begin, &data_offset);

    rr.name = string(name);
    free(name);
    p += data_offset;
    
    unsigned short type_ = ntohs(*(unsigned short *)p);
    p += 2;
    unsigned short class_ = ntohs(*(unsigned short *)p);
    p += 2;
    unsigned int ttl = ntohl(*(unsigned int*)p);
    p += 4;
    unsigned short rr_len = ntohs(*(unsigned short *)p);
    p += 2;

    rr.type_ = type_;
    rr.class_ = class_;
    rr.ttl = ttl;
    rr.data_len = rr_len;

    if(type_ == 1 && class_ == 1){
        // A 记录
        struct in_addr addr = *(struct in_addr *)p;
        rr.data = string(inet_ntoa(addr));
    }
    else if(type_ == 28 && class_ == 1){
        // AAAA IPv6记录
        char ipstr[40];
        inet_ntop(AF_INET6, p, ipstr, 40);
        rr.data = string(ipstr);
    }
    else {
        rr_data = parseData(p, &data_offset);
        rr.data = string(rr_data);
        free(rr_data);
    }
    p += rr_len;

    switch (rrtype) {
        case 1:
            answerFields.push_back(rr);
            break;
        case 2:
            authorityFields.push_back(rr);
            break;
        default:
            additionalFields.push_back(rr); 
    }

    return p;
}

char *DNSPacketParser::parseData(const char *begin, int *data_offset){
    char buf[1024];
    char *str;
    unsigned short cptr = 0; 
    char *bufptr = buf;
    const char *p = begin;
    unsigned char num = *p;
    
    do {
        if (isptr(num)){
            cptr = ntohs(*(unsigned short *)p);
            cptr = ptrOffset(cptr); //获得压缩指针指向的位置
            p += 2;
            str = parseData(pkt+cptr, NULL);
            strcpy(bufptr, str);
            bufptr += strlen(str);
            free(str);
            break;
        }
        p++;

        memcpy(bufptr, p, num);
        bufptr += num;
        p += num;

        num = *p;
        if(num != 0){
            memset(bufptr, '.', 1);
            bufptr++;
        }
    } while(num != 0);
 
    *bufptr = 0;
    if(data_offset != NULL){
        *data_offset = p - begin;
    }
    return strdup(buf);
}

void DNSPacketParser::parse()
{
    int i;
    const char *pos = body;

    printPacketHeader();
    for(i=0; i<questions; i++){ 
        pos = parseQuestion(pos);
    }
    for (i=0; i<answerRRs; i++){
        pos = parseResourceRecord(pos, 1);
    }

    for (i=0; i<authorityRRs; i++){
        pos = parseResourceRecord(pos, 2);
    }

    for (i=0; i<additionalRRs; i++){
        pos = parseResourceRecord(pos, 3);
    }

    //打印
    cout << "==>Question field<==\n" << endl;
    for(vector<QuestionField>::iterator it = questionFields.begin(); it != questionFields.end(); ++it){
        cout << it->name << endl;
        cout << it->type_ << endl;
        cout << it->class_ << endl;
        cout << endl;
    }
    cout << "==>Answer fields<==\n" << endl;
    for(vector<ResourceRecord>::iterator it = answerFields.begin(); it != answerFields.end(); ++it){
        cout << it->name << endl;
        cout << it->type_ << endl;
        cout << it->class_ << endl;
        cout << it->ttl << endl;
        cout << it->data_len << endl;
        cout << it->data << endl;
        cout << endl;
    }
    cout << "==>Authority fields<==\n" << endl;
    for(vector<ResourceRecord>::iterator it = authorityFields.begin(); it != authorityFields.end(); ++it){
        cout << it->name << endl;
        cout << it->type_ << endl;
        cout << it->class_ << endl;
        cout << it->ttl << endl;
        cout << it->data_len << endl;
        cout << it->data << endl;
        cout << endl;
    }
    cout << "==>additional fields<==\n" << endl;
    for(vector<ResourceRecord>::iterator it = additionalFields.begin(); it != additionalFields.end(); ++it){
        cout << it->name << endl;
        cout << it->type_ << endl;
        cout << it->class_ << endl;
        cout << it->ttl << endl;
        cout << it->data_len << endl;
        cout << it->data << endl;
        cout << endl;
    }
}

