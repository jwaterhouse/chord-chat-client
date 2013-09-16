#ifndef REMOTENODE_H
#define REMOTENODE_H

#include "../include/INode.h"

class RemoteNode : public INode
{
    public:
        RemoteNode(const std::string& name, const std::string& host, unsigned int port) : INode::INode(name, host, port) { }

        RemoteNode(const char* serial, size_t s) : INode::INode(serial, s) { }

        RemoteNode(const Node& n) : INode::INode(n) { }

        virtual ~RemoteNode() { }

        virtual Node findPredecessor(const ID&);
        virtual Node findSuccessor(const ID&);
        virtual Node closestPrecedingFinger(const ID&);
        virtual void join(Node);
        virtual void stabilize();
        virtual void notify(Node);
        virtual void fixFingers();
        virtual bool ping();
        virtual void receive(std::string);

        // Getters and setters;
        virtual Node getPredecessor();
        virtual void setPredecessor(Node);
        virtual Node getSuccessor();
        virtual void setSuccessor(Node);

    protected:
    private:
        std::string sendMessage(std::string, bool = false, bool = false);
        std::string createMessage(RPCCode, const std::string&);
};

#endif // REMOTENODE_H
