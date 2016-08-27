#include "DNSHeader.hpp"
#include <iostream>

using namespace std;

ostream & operator<<(ostream &os, const struct QuestionField &q)
{
    return os << q.name << "\t" << q.type_ << "\t" << q.class_;
}


std::ostream & operator<<(std::ostream &os, const ResourceRecord &r)
{
    return os << r.name << "\t" << r.type_ << "\t" << r.data; 
}
