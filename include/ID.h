#ifndef ID_H
#define ID_H

#include <string>
#include <sstream>
#include "../include/sha1.h"
//#include "../include/INode.h"

#define ID_LEN 20
#define M 160

typedef unsigned char byte;

class ID
{
    public:
        ID() { };
        ID(const byte*);
        ID(const ID&);
        ID(const std::string&, unsigned int);
        virtual ~ID();

        bool isInInterval(const ID&, const ID&) const;
        //bool isInInterval(const INode& n, const ID& id) const { return isInInterval(n.getID(), id); }
        //bool isInInterval(const ID& id, const INode& n) const { return isInInterval(id, n.getID()); }
        //bool isInInterval(const INode& n1, const INode& n2) const { return isInInterval(n1.getID(), n2.getID()); }
        const byte* c_str() const;

        byte& operator[](int);
        const byte& operator[](int) const;
        ID& operator+=(const ID&);
        friend ID operator+(ID&, const ID&);
        ID& operator-=(const ID&);
        friend ID operator-(ID&, const ID&);
        friend bool operator==(const ID&, const ID&);
        friend bool operator<(const ID&, const ID&);
        friend bool operator>(const ID&, const ID&);
        friend bool operator<=(const ID&, const ID&);
        friend bool operator>=(const ID&, const ID&);

    protected:
    private:
        byte _id[ID_LEN] = {0x00};
};

#endif // ID_H
