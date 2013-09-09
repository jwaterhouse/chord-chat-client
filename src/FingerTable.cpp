#include <cmath>
#include <cstring>
#include "../include/FingerTable.h"

FingerTable::FingerTable(const ID& id)
{
    _id = new ID(id);
}

FingerTable::~FingerTable()
{
    /*
    for(int i = 0; i < M; ++i)
    {
        if (entries[i] != 0)
        {
            delete entries[i];
            entries[i] = 0;
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
    byte b[ID_LEN];
    b[ID_LEN - k / 8 - 1] = (0x01 << (k % 8));
    ID bID(b);

    ID id = *_id + bID;
    return id;
}

//void interval(unsigned int k, FingerTableEntry* intervalStart, FingerTableEntry* intervalEnd);

INode* FingerTable::node(unsigned int k)
{
    return _entries[k];
}

void FingerTable::setNode(unsigned int k, INode* n)
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
