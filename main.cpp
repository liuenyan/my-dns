#include "DNSPacketSender.hpp"
#include "DNSPacketParser.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
    if (argc < 2){
        cout << "usage: mydns name"<<endl;
        exit(1);
    }
    DNSPacketSender *sender = new DNSPacketSender("114.114.114.114", 53);
    int recvlen;
    char *recvdata = sender->sendDNSQuery(argv[1], recvlen);
    DNSPacketParser *parser = new DNSPacketParser(recvdata, recvlen);
    parser->parse();
    free(recvdata);
    delete parser;
    delete sender;
    return 0;
}
