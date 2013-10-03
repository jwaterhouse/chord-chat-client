#ifndef REMOTENODE_H
#define REMOTENODE_H

#include "../include/INode.h"

class RemoteNode : public INode
{
    public:
        /** \brief Constructs a new RemoteNode.
         *
         * \param std::string Name of the node.
         * \param std::string Host/IP of the node.
         * \param unsigned int Port for the node to communicate on.
         */
        RemoteNode(const std::string& name, const std::string& host, unsigned int port) : INode::INode(name, host, port) { }

        /** \brief Constructs a RemoteNode from a node serialisation
         *
         * \param const char* Serialzed representation of the node
         * \param size_t The length of the serial array
         */
        RemoteNode(const char* serial, size_t s) : INode::INode(serial, s) { }

        /** \brief Copy constructor
         *
         * \param Node Node to copy
         */
        RemoteNode(const Node& n) : INode::INode(n) { }

        /**< Destructor */
        virtual ~RemoteNode() { }

        // Chord implementation methods
        virtual Node findPredecessor(const ID&);
        virtual Node findSuccessor(const ID&);
        virtual Node closestPrecedingFinger(const ID&);
        virtual void join(Node);
        virtual void stabilize();
        virtual void notify(Node);
        virtual void fixFingers();
        virtual bool ping();
        virtual void receive(std::string);

        // Getters and setters
        virtual Node getPredecessor();
        virtual void setPredecessor(Node);
        virtual Node getSuccessor();
        virtual void setSuccessor(Node);

    protected:
    private:
        /** \brief Sends a message to the LocalNode corresponding to this RemoteNode
         *
         * \param std::string The message to send
         * \param bool True if a response is expected, false if not
         * \param bool True if the method should throw excpetions, false to handle them internally
         * \return std::string The reply message, or an empty string if none
         */
        std::string sendMessage(std::string, bool = false, bool = false);

        /** \brief Packages an RPCCode and payload into a message string
         *
         * \param RPCCode The RPC Code to send
         * \param std::string The payload of the message
         * \return std::string The created message
         */
        std::string createMessage(RPCCode, const std::string&);
};

#endif // REMOTENODE_H
