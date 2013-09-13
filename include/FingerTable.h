#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "../include/INode.h"

class FingerTable
{
    public:
        FingerTable() { }
        virtual ~FingerTable() { }

        const Node& operator[](int) const;
        Node getNode(unsigned int);
        void setNode(unsigned int, Node);

        void printFingerTable() const;
    protected:
    private:
        Node _entries[M] = {NULL};
};

#endif // FINGERTABLE_H
