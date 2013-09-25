#ifndef ID_H
#define ID_H

#include <string>
#include <sstream>
#include "../include/sha1.h"
//#include "../include/INode.h"

#define ID_LEN 20 /**< Number of bytes in an ID string. */
#define M 160 /**< Number of bits in an ID. Depends on consistent hash algorithm. */

class ID
{
    public:
        ID() { };
        ID(const char*);
        ID(const ID&);
        ID(const std::string&, unsigned int);
        ID(const std::string&);
        virtual ~ID();

        bool isInInterval(const ID&, const ID&) const;
        const char* c_str() const;

        char& operator[](int);
        const char& operator[](int) const;
        ID& operator+=(const ID&);
        const ID operator+(const ID&) const;
        ID& operator-=(const ID&);
        const ID operator-(const ID&) const;
        friend bool operator==(const ID&, const ID&);
        friend bool operator!=(const ID&, const ID&);
        friend bool operator<(const ID&, const ID&);
        friend bool operator>(const ID&, const ID&);
        friend bool operator<=(const ID&, const ID&);
        friend bool operator>=(const ID&, const ID&);

    protected:
    private:
        char _id[ID_LEN] = {0x00};
};

#endif // ID_H
