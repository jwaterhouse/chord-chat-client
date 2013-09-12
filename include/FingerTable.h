#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "../include/INode.h"

class FingerTable
{
    public:
        FingerTable(INode&);
        virtual ~FingerTable();

        ID start(unsigned int);
        Node node(unsigned int);
        void setNode(unsigned int, Node);
    protected:
    private:
        Node _entries[M] = {NULL};
        ID* _id = 0;
};

#endif // FINGERTABLE_H
