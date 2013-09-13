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
    while (_entries[index] == NULL) index--;
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
    printFingerTable();
}

void FingerTable::printFingerTable() const
{
    std::cout << "Finger Table:" << std::endl;
    for(int i = 0; i < M; ++i)
    {
        if (_entries[i] != NULL)
        {
            std::cout << "\t" << i << "\t" << _entries[i]->getName() << std::endl;
        }
    }
}
