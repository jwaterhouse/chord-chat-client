#ifndef REMOTENODE_H
#define REMOTENODE_H

#include "../include/INode.h"

class RemoteNode : public INode
{
    public:
        RemoteNode(const char*, size_t);
        virtual ~RemoteNode();

        virtual INode* findPredecessor(const ID&);
        virtual INode* findSuccessor(const ID&);
        virtual INode* closestPrecedingFinger(const ID&);
        virtual void join(INode*);
        virtual void initFingerTable(INode*);
        virtual void updateOthers();
        virtual void updateFingerTable(INode*, unsigned int);
        virtual void stabilize();
        virtual void notify(INode*);
        virtual void fixFingers();

        // Getters and setters;
        virtual INode* getPredecessor();
        virtual void setPredecessor(INode*);
        virtual INode* getSuccessor();
        virtual void setSuccessor(INode*);

        virtual INode* clone() const;
    protected:
        std::string sendMessage(std::string, bool);
        std::string createMessage(RPCCode, const std::string&);
    private:
};

#endif // REMOTENODE_H
