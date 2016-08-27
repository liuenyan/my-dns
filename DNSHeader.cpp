#include "DNSHeader.hpp"
#include <iostream>

using namespace std;

ostream & operator<<(ostream &os, const QuestionField &q)
{
    return os << q.name << "\t" << q.type_ << "\t" << q.class_;
}


ostream & operator<<(ostream &os, const ResourceRecord &r)
{
    return os << r.name << "\t" << r.type_ << "\t" << r.data; 
}
