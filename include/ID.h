#ifndef ID_H
#define ID_H

#include <string>
#include <sstream>
#include "../include/sha1.h"

#define ID_LEN 20
#define M 160

typedef unsigned char byte;

class ID
{
    public:
        ID() { };
        ID(byte*);
        ID(const ID&);
        ID(std::string, unsigned int);
        virtual ~ID();

        bool isInInterval(const ID*, const ID*) const;
        const byte* get();

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
        byte id[ID_LEN];
};

#endif // ID_H
