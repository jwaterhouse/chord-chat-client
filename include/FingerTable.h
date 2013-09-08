#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "../include/INode.h"

class FingerTable
{
    public:
        FingerTable();
        virtual ~FingerTable();

        ID* start(unsigned int);
        //void interval(unsigned int, unsigned int*, unsigned int*);
        INode* node(unsigned int);
        void setNode(unsigned int, INode*);
    protected:
    private:
        INode* entries[M];
        ID id;
};

#endif // FINGERTABLE_H
