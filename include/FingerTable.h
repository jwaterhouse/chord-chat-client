#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "../include/INode.h"

class FingerTable
{
    public:
        FingerTable() { }
        FingerTable(const ID& id) : _id(id) { }
        virtual ~FingerTable() { }

        const Node& operator[](int) const;
        Node getNode(unsigned int);
        void setNode(unsigned int, Node);
        ID start(unsigned int) const;
        void setID(const ID&);
        void removeNode(Node);

    protected:
    private:
        Node _entries[M] = {NULL};
        ID _id;

        std::string getIDShort(const ID& id) const;

        void printFingerTable() const;
};

#endif // FINGERTABLE_H
