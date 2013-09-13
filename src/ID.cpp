#include <stdlib.h>     /* malloc, free, rand */
#include "../include/ID.h"

ID::ID(const char* id)
{
    for (int i = 0; i < ID_LEN; ++i)
        _id[i] = id[i];
};

ID::ID(const ID& other)
{
    for(int i = 0; i < ID_LEN; ++i)
        _id[i] = other[i];
}

ID::ID(const std::string& ip, unsigned int port)
{
    SHA1* sha1 = new SHA1();
	sha1->addBytes(ip.c_str(), ip.length());
	char portByteArr[4];
    portByteArr[0] = (char)port;
    portByteArr[1] = (char)(port >> 8);
    portByteArr[2] = (char)(port >> 16);
    portByteArr[3] = (char)(port >> 24);
    sha1->addBytes((const char*)portByteArr, 4);

	unsigned char* digest = sha1->getDigest();
	for (int i = 0; i < ID_LEN; ++i) _id[i] = (char)digest[i];
	free(digest);
	delete sha1;
}

ID::ID(const std::string& str)
{
    SHA1* sha1 = new SHA1();
	sha1->addBytes(str.c_str(), str.length());
	unsigned char* digest = sha1->getDigest();
	for (int i = 0; i < ID_LEN; ++i) _id[i] = (char)digest[i];
	free(digest);
	delete sha1;
}

ID::~ID()
{
    /*
    if (_id != 0)
    {
        delete _id;
        _id = 0;
    }
    */
}

bool ID::isInInterval(const ID& lower, const ID& upper) const
{
    if (lower < upper)
        return *this > lower && *this < upper;
    if (lower > upper)
        return *this > lower || *this < upper;
    return true;
}

const char* ID::c_str() const
{
    return _id;
}

char& ID::operator[](int index)
{
    if (index < 0 || index >= ID_LEN)
    {
        //error
    }
    return _id[index];
}

const char& ID::operator[](int index) const
{
    if (index < 0 || index >= ID_LEN)
    {
        //error
    }
    return _id[index];
}

ID& ID::operator+=(const ID& other)
{
    char overflow = 0x00;
    // MSB is at index 0
    for(int i = ID_LEN - 1; i >= 0; --i)
    {
        unsigned int val = (unsigned int)_id[i] + (unsigned int)other[i] + (unsigned int)overflow;
        _id[i] = (char)val;
        overflow = (char)(val >> 8) & 0x01;
    }
    return *this;
}

const ID ID::operator+(const ID& rhs) const
{
  return ID(*this) += rhs;
}

ID& ID::operator-=(const ID& other)
{
    char overflow = 0x00;
    for(int i = 0; i < ID_LEN; ++i)
    {
        unsigned int val = (unsigned int)_id[i] + (unsigned int)(other[i] ^ 0xff) + (unsigned int)overflow;
        if (i == 0) val = val + 1;
        _id[i] = (char)val;
        overflow = (char)(val >> 8) & 0x01;
    }

    return *this;
}

const ID ID::operator-(const ID& rhs) const
{
  return ID(*this) -= rhs;
}

bool operator==(const ID& lhs, const ID& rhs)
{
    for(int i = 0; i < ID_LEN; ++i)
        if (lhs[i] != rhs[i]) return false;
    return true;
}
bool operator!=(const ID& lhs, const ID& rhs) { return !operator==(lhs,rhs); }
bool operator<(const ID& lhs, const ID& rhs)
{
    // MSB is at index 0
    for(int i = 0; i < ID_LEN; ++i)
    {
        if (lhs[i] < rhs[i]) return true;
        if (lhs[i] > rhs[i]) return false;
    }
    return false;
}
bool operator>(const ID& lhs, const ID& rhs) { return  operator<(rhs,lhs);}
bool operator<=(const ID& lhs, const ID& rhs) { return !operator>(lhs,rhs); }
bool operator>=(const ID& lhs, const ID& rhs) { return !operator<(lhs,rhs); }
