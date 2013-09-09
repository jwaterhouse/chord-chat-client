#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "../include/INode.h"

class FingerTable
{
    public:
        FingerTable(const ID&);
        virtual ~FingerTable();

        ID start(unsigned int);
        INode* node(unsigned int);
        void setNode(unsigned int, INode*);
    protected:
    private:
        INode* _entries[M] = {NULL};
        ID* _id = 0;
};

#endif // FINGERTABLE_H
