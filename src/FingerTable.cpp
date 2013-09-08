#include <cmath>
#include "../include/FingerTable.h"

FingerTable::FingerTable()
{
    //ctor
}

FingerTable::~FingerTable()
{
    for(int i = 0; i < M; ++i)
    {
        if (entries[i] != 0)
        {
            delete entries[i];
            entries[i] = 0;
        }
    }
}

ID* FingerTable::start(unsigned int k)
{
    if (k < 0 || k >= M)
    {
        //error
    }
    byte b[ID_LEN];
    b[ID_LEN - k / 8 - 1] = (0x01 << (k % 8));
    ID bID(b);

    ID* ret = new ID(id + bID);
    return ret;
}

//void interval(unsigned int k, FingerTableEntry* intervalStart, FingerTableEntry* intervalEnd);

INode* FingerTable::node(unsigned int k)
{
    return this->entries[k];
}

void FingerTable::setNode(unsigned int k, INode* n)
{
    if (entries[k] != 0)
    {
        delete entries[k];
        entries[k] = 0;
    }
    this->entries[k] = n;
}
