#include <cmath>
#include <cstring>
#include <exception>
#include <iostream>

#include "FingerTable.h"

#include "Logger.h"

Node
FingerTable::getNode(unsigned int index)
{
    if (index < 0 || index >= M)
    {
        throw std::runtime_error("FingerTable::getNode() - index out of bounds: " + index);
    }
    while (_entries[index] == NULL && index < M)
    {
        index++;
    }
    if (index == M)
    {
        return NULL;
    }

    return _entries[index];
}

Node
FingerTable::getNodeAt(unsigned int index)
{
    if (index < 0 || index >= M)
    {
        throw std::runtime_error("FingerTable::getNodeAt() - index out of bounds: " + index);
    }

    return _entries[index];
}

void
FingerTable::setNode(unsigned int index, Node n)
{
    if (index < 0 || index >= M)
    {
        throw std::runtime_error("FingerTable::setNode() - index out of bounds: " + index);
    }
    _entries[index] = n;
}


ID
FingerTable::start(unsigned int k) const
{
    // n + 2**i

    if (k < 0 || k >= M)
    {
        throw std::runtime_error("FingerTable::start() - k out of bounds: " + k);
    }
    // MSB is at index 0
    char b[ID_LEN] = {'\0'};
    b[ID_LEN - (k / 8) - 1] = (0x01 << (k % 8));
    ID bID(b);

    return _id + bID;
}

void
FingerTable::setId(const ID & id)
{
    _id = id;
}

void
FingerTable::removeNode(Node n)
{
    if (n != 0)
    {
        for (int i = 0; i < M; ++i)
        {
            if (_entries[i] == n)
            {
                _entries[i] = NULL;
            }
        }
    }
}

std::string
FingerTable::toString() const
{
    std::ostringstream oss;
    oss << "Finger Table: n = ";
    SHA1::hexPrinter((unsigned char*)(_id.c_str()), ID_LEN);
    oss << std::endl;
    oss << "i\tstart\tid\tstart+id\tname" << std::endl;
    for (int i = 0; i < M; ++i)
    {
        if (_entries[i] != NULL)
        {
            oss << i << "\t" << getIdShort(start(i)) << "\t" << getIdShort(_entries[i]->getId()) << "\t" << getIdShort(start(i) + _entries[i]->getId()) << "\t"<< _entries[i]->getName() << "\t" << std::endl;
        }
    }
    return oss.str();
}

std::string
FingerTable::getIdShort(const ID & id) const
{
    unsigned char first = (unsigned char)(id.c_str()[0]);
    unsigned char last = (unsigned char)(id.c_str()[ID_LEN - 1]);

    unsigned char st[7];

    st[0] = ((first >> 4) >= 0 && (first >> 4) <= 9) ? (first >> 4) + '0' : (first >> 4) + 'a' - 10;
    st[1] = ((first & 0x0f) >= 0 && (first & 0x0f) <= 9) ? (first & 0x0f) + '0' : (first & 0x0f) + 'a' - 10;

    st[5] = ((last >> 4) >= 0 && (last >> 4) <= 9) ? (last >> 4) + '0' : (last >> 4) + 'a' - 10;
    st[6] = ((last & 0x0f) >= 0 && (last & 0x0f) <= 9) ? (last & 0x0f) + '0' : (last & 0x0f) + 'a' - 10;

    st[2] = '.';
    st[3] = '.';
    st[4] = '.';

    return std::string((char*)st);
}
