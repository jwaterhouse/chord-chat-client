#ifndef REMOTENODE_H
#define REMOTENODE_H

#include "../include/INode.h"

class RemoteNode : public INode
{
    public:
        RemoteNode(const std::string&, const std::string&, unsigned int);
        RemoteNode(const char*, size_t);
        RemoteNode(const Node&);
        virtual ~RemoteNode();

        virtual Node findPredecessor(const ID&);
        virtual Node findSuccessor(const ID&);
        virtual Node closestPrecedingFinger(const ID&);
        virtual void join(Node);
        virtual void initFingerTable(Node);
        virtual void updateOthers();
        virtual void updateFingerTable(Node, unsigned int);
        virtual void stabilize();
        virtual void notify(Node);
        virtual void fixFingers();

        // Getters and setters;
        virtual Node getPredecessor();
        virtual void setPredecessor(Node);
        virtual Node getSuccessor();
        virtual void setSuccessor(Node);

        virtual void receive(std::string);

    protected:
        std::string sendMessage(std::string, bool);
        std::string createMessage(RPCCode, const std::string&);
    private:
};

#endif // REMOTENODE_H
