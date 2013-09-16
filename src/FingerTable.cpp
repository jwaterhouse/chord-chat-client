#include <cmath>
#include <cstring>
#include "../include/FingerTable.h"
#include <iostream>

const Node& FingerTable::operator[](int index) const
{
    if (index < 0 || index >= M)
    {
        //error
    }
    while (_entries[index] == NULL && index < M) index++;
    if (index == M) return NULL;
    return _entries[index];
}

Node FingerTable::getNode(unsigned int index)
{
    if (index < 0 || index >= M)
    {
        //error
    }
    return _entries[index];
}

void FingerTable::setNode(unsigned int index, Node n)
{
    if (index < 0 || index >= M)
    {
        //error
    }
    _entries[index] = n;
    //printFingerTable();
}


ID FingerTable::start(unsigned int k) const
{
    // n + 2**i

    if (k < 0 || k >= M)
    {
        //error
    }
    // MSB is at index 0
    char b[ID_LEN] = {'\0'};
    b[ID_LEN - (k / 8) - 1] = (0x01 << (k % 8));
    ID bID(b);

    return _id + bID;
}

void FingerTable::setID(const ID& id)
{
    _id = id;
}

void FingerTable::removeNode(Node n)
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

void FingerTable::printFingerTable() const
{
    std::cout << std::endl;
    std::cout << "Finger Table: n = ";
    SHA1::hexPrinter((unsigned char*)(_id.c_str()), ID_LEN);
    std::cout << std::endl;
    std::cout << "i\tstart\tid\tstart+id\tname" << std::endl;
    for (int i = 0; i < M; ++i)
    {
        if (_entries[i] != NULL)
        {
            std::cout << i << "\t" << getIDShort(start(i)) << "\t" << getIDShort(_entries[i]->getID()) << "\t" << getIDShort(start(i) + _entries[i]->getID()) << "\t"<< _entries[i]->getName() << "\t" << std::endl;
        }
    }
}

std::string FingerTable::getIDShort(const ID& id) const
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
