#include <cmath>
#include <cstring>
#include "../include/FingerTable.h"

FingerTable::FingerTable(INode& n)
{
    _id = new ID(n.getID());
    //for (int i = 0; i < M; ++i)
    //   _entries[i] = &n;
}

FingerTable::~FingerTable()
{
    /*
    for(int i = 0; i < M; ++i)
    {
        if (_entries[i] != 0)
        {
            delete _entries[i];
            _entries[i] = 0;
        }
    }
    */

    if (_id != 0)
    {
        delete _id;
        _id = 0;
    }
}

ID FingerTable::start(unsigned int k)
{
    if (k < 0 || k >= M)
    {
        //error
    }
    char b[ID_LEN];
    b[ID_LEN - k / 8 - 1] = (0x01 << (k % 8));
    ID bID(b);

    ID id = *_id + bID;
    return id;
}

//void interval(unsigned int k, FingerTableEntry* intervalStart, FingerTableEntry* intervalEnd);

Node FingerTable::node(unsigned int k)
{
    return _entries[k];
}

void FingerTable::setNode(unsigned int k, Node n)
{
    /*
    if (_entries[k] != 0)
    {
        delete _entries[k];
        _entries[k] = 0;
    }
    */

    _entries[k] = n;
}
