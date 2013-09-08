#include "../include/ID.h"

ID::ID(byte* id)
{
    for (int i = 0; i < ID_LEN; ++i)
        this->id[i] = id[i];
};

ID::ID(const ID& other)
{
    for(int i = 0; i < ID_LEN; ++i)
        (*this)[i] = other[i];
}

ID::ID(std::string ip, unsigned int port)
{
    // Create the node's ID as a sha-1 of it's ip and port (e.g. "192.168.1.1:55")
    //std::ostringstream stream;
    //stream << port;
    //std::string source = stream.str();

    //CryptoPP::SHA1 sha1;
    //std::string hash = "";
    //CryptoPP::StringSource(source, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash))));
    //id = hash;

    //CryptoPP::SHA1 hash;
    //hash.Restart();
    //hash.Update((byte*)ip.c_str(), ip.length());
    //byte portByteArr[4];
    //portByteArr[0] = (byte)port;
    //portByteArr[1] = (byte)(port >> 8);
    //portByteArr[2] = (byte)(port >> 16);
    //portByteArr[3] = (byte)(port >> 24);
    //hash.Update(portByteArr, 4);
    //hash.Final(id);

    SHA1* sha1 = new SHA1();
	sha1->addBytes(ip.c_str(), ip.length());
	byte portByteArr[4];
    portByteArr[0] = (byte)port;
    portByteArr[1] = (byte)(port >> 8);
    portByteArr[2] = (byte)(port >> 16);
    portByteArr[3] = (byte)(port >> 24);
    sha1->addBytes((const char*)portByteArr, 4);

	byte* digest = sha1->getDigest();
	for (int i = 0; i < ID_LEN; ++i) id[0] = (byte)(digest[0]);
	delete digest;
	delete sha1;
}

ID::~ID()
{
    //dtor
}

bool ID::isInInterval(const ID* lower, const ID* upper) const
{
    if (lower < upper)
        return *this > *lower && *this < *upper;
    if (lower > upper)
        return *this > *lower || *this < *upper;
    return false;
}

const byte* ID::get()
{
    return id;
}

byte& ID::operator[](int index)
{
    if (index < 0 || index >= ID_LEN)
    {
        //error
    }
    return id[index];
}
const byte& ID::operator[](int index) const
{
    if (index < 0 || index >= ID_LEN)
    {
        //error
    }
    return id[index];
}


ID& ID::operator+=(const ID& other)
{
    byte overflow = 0x00;
    for(int i = 0; i < ID_LEN; ++i)
    {
        unsigned int val = (unsigned int)id[i] + (unsigned int)other[i] + (unsigned int)overflow;
        id[i] = (byte)val;
        overflow = (byte)(val >> 8) & 0x01;
    }
    return *this;
}
ID operator+(ID& lhs, const ID& rhs)
{
  lhs += rhs;
  return lhs;
}

ID& ID::operator-=(const ID& other)
{
    byte temp[ID_LEN];
    temp[0] = 0x01;
    for(int i = 0; i < ID_LEN; ++i)
        id[i] = id[i] ^ 0xff;

    byte overflow = 0x00;
    for(int i = 0; i < ID_LEN; ++i)
    {
        unsigned int val = (unsigned int)id[i] + (unsigned int)temp[i] + (unsigned int)overflow;
        id[i] = (byte)val;
        overflow = (byte)(val >> 8) & 0x01;
    }

    *this += other;
    return *this;
}

ID operator-(ID& lhs, const ID& rhs)
{
  lhs -= rhs;
  return lhs;
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
    // most significant byte is at index 0
    for(int i = 0; i < ID_LEN; ++i)
        if (lhs[i] < rhs[i]) return true;
    return false;
}
bool operator>(const ID& lhs, const ID& rhs) { return  operator<(rhs,lhs);}
bool operator<=(const ID& lhs, const ID& rhs) { return !operator>(lhs,rhs); }
bool operator>=(const ID& lhs, const ID& rhs) { return !operator<(lhs,rhs); }
