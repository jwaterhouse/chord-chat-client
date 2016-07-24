#include <stdlib.h>
#include <exception>
#include <iomanip>

#include "ID.h"

#include "Logger.h"

ID::ID(const char * id)
{
    for (int i = 0; i < ID_LEN; ++i)
    {
        _id[i] = id[i];
    }
}

ID::ID(const ID & other)
{
    for (int i = 0; i < ID_LEN; ++i)
    {
        _id[i] = other[i];
    }
}

ID::ID(const std::string & host, unsigned int port)
{
    SHA1* sha1 = new SHA1();
    sha1->addBytes(host.c_str(), host.length());
    char portByteArr[4];
    portByteArr[0] = (char)port;
    portByteArr[1] = (char)(port >> 8);
    portByteArr[2] = (char)(port >> 16);
    portByteArr[3] = (char)(port >> 24);
    sha1->addBytes((const char*)portByteArr, 4);

    unsigned char* digest = sha1->getDigest();
    for (int i = 0; i < ID_LEN; ++i)
    {
        _id[i] = (char)digest[i];
    }
    free(digest);
    delete sha1;
}

ID::ID(const std::string & str)
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

}

bool
ID::isInInterval(const ID & lower, const ID & upper) const
{
    if (lower < upper)
        return *this > lower && *this < upper;
    if (lower > upper)
        return *this > lower || *this < upper;
    return true;
}

const char *
ID::c_str() const
{
    return _id;
}

std::string
ID::toHexString() const
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for(int i = 0; i < ID_LEN; ++i)
    {
        ss << std::setw(2) << static_cast<unsigned>(_id[i]);
    }

    return ss.str();
}

char &
ID::operator[](int index)
{
    if (index < 0 || index >= ID_LEN)
    {
        throw std::runtime_error("ID::operator[] - index out of bounds: " + index);
    }

    return _id[index];
}

const char &
ID::operator[](int index) const
{
    if (index < 0 || index >= ID_LEN)
    {
        throw std::runtime_error("ID::operator[] - index out of bounds: " + index);
    }

    return _id[index];
}

ID &
ID::operator+=(const ID & other)
{
    char overflow = 0x00;
    // MSB is at index 0
    for (int i = ID_LEN - 1; i >= 0; --i)
    {
        unsigned int val = (unsigned int)_id[i] + (unsigned int)other[i] + (unsigned int)overflow;
        _id[i] = (char)val;
        overflow = (char)(val >> 8) & 0x01;
    }

    return *this;
}

const ID
ID::operator+(const ID & rhs) const
{
  return ID(*this) += rhs;
}

ID &
ID::operator-=(const ID & other)
{
    char overflow = 0x00;
    for (int i = 0; i < ID_LEN; ++i)
    {
        unsigned int val = (unsigned int)_id[i] + (unsigned int)(other[i] ^ 0xff) + (unsigned int)overflow;
        if (i == 0)
        {
            val = val + 1;
        }
        _id[i] = (char)val;
        overflow = (char)(val >> 8) & 0x01;
    }

    return *this;
}

const ID
ID::operator-(const ID & rhs) const
{
    return ID(*this) -= rhs;
}

bool
operator==(const ID & lhs, const ID & rhs)
{
    for (int i = 0; i < ID_LEN; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }

    return true;
}

bool
operator!=(const ID & lhs, const ID & rhs)
{
    return !operator==(lhs, rhs);
}

bool
operator<(const ID & lhs, const ID & rhs)
{
    // MSB is at index 0
    for (int i = 0; i < ID_LEN; ++i)
    {
        if (lhs[i] < rhs[i])
        {
            return true;
        }
        if (lhs[i] > rhs[i])
        {
            return false;
        }
    }

    return false;
}

bool
operator>(const ID & lhs, const ID & rhs)
{
    return operator<(rhs, lhs);
}

bool
operator<=(const ID & lhs, const ID & rhs)
{
    return !operator>(lhs, rhs);
}

bool
operator>=(const ID & lhs, const ID & rhs)
{
    return !operator<(lhs, rhs);
}
