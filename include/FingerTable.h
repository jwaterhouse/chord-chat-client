#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "../include/INode.h"

class FingerTable
{
    public:
        FingerTable() { }
        FingerTable(const ID& id) : _id(id) { }
        virtual ~FingerTable() { }

        Node getNode(unsigned int);
        Node getNodeAt(unsigned int);
        void setNode(unsigned int, Node);
        ID start(unsigned int) const;
        void setID(const ID&);
        void removeNode(Node);

    protected:
    private:
        Node _entries[M] = {NULL};
        ID _id;

        void printFingerTable() const;
        std::string getIDShort(const ID& id) const;
};

#endif // FINGERTABLE_H
