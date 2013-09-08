#ifndef LOCALNODE_H
#define LOCALNODE_H

#include "../include/INode.h"
#include "../include/FingerTable.h"

class LocalNode : INode
{
    public:
        LocalNode(std::string ip, unsigned int port) : INode(ip, port) { };

        LocalNode(std::string ip, unsigned int port, INode* n) : INode(ip, port)
        {
            join(n);
        };

        virtual ~LocalNode()
        {
            if (predecessor != 0)
            {
                delete predecessor;
                predecessor = 0;
            }
        }

    protected:
        virtual INode* findPredecessor(ID*);
        virtual INode* findSuccessor(ID*);
        virtual INode* closestPrecedingFinger(ID*);
        virtual void join(INode*);
        virtual void initFingerTable(INode*);
        virtual void updateOthers();
        virtual void updateFingerTable(INode*, unsigned int);
        virtual void stabilize();
        virtual void notify(INode*);
        virtual void fixFingers();

        // Getters and setters;
        virtual INode* getPredecessor();
        virtual void setPredecessor(INode* n);
        virtual INode* getSuccessor();
        virtual void setSuccessor(INode* n);

        INode* predecessor;
        FingerTable finger;
};

#endif // LOCALNODE_H
