#ifndef REMOTENODE_H
#define REMOTENODE_H

#include "INode.h"

class RemoteNode : public INode
{
    public:
        /** \brief Constructs a new RemoteNode.
         *
         * \param std::string Name of the node.
         * \param std::string Host/IP of the node.
         * \param unsigned int Port for the node to communicate on.
         */
        RemoteNode(const std::string & name, const std::string & host, unsigned int port) : INode::INode(name, host, port) { }

        /** \brief Constructs a RemoteNode from a node serialisation
         *
         * \param const char* Serialzed representation of the node
         * \param size_t The length of the serial array
         */
        RemoteNode(const char * serial, size_t s) : INode::INode(serial, s) { }

        /** \brief Copy constructor
         *
         * \param Node Node to copy
         */
        RemoteNode(const Node & n) : INode::INode(n) { }

        /**< Destructor */
        virtual ~RemoteNode() { }

        // Chord implementation methods
        virtual Node findPredecessor(const ID & id);
        virtual Node findSuccessor(const ID & id);
        virtual Node closestPrecedingFinger(const ID & id);
        virtual void join(Node n);
        virtual void stabilize();
        virtual void notify(Node n);
        virtual void fixFingers();
        virtual bool ping();
        virtual void receive(const std::string & message);

        // Getters and setters
        virtual Node getPredecessor();
        virtual void setPredecessor(Node n);
        virtual Node getSuccessor();
        virtual void setSuccessor(Node n);

    protected:
    private:
        /** \brief Sends a message to the LocalNode corresponding to this RemoteNode
         *
         * \param std::string The message to send
         * \param bool True if a response is expected, false if not
         * \param bool True if the method should throw excpetions, false to handle them internally
         * \return std::string The reply message, or an empty string if none
         */
        std::string sendMessage(const std::string & message, bool responseExpected = false, bool throwException = false);

        /** \brief Packages an RPCCode and payload into a message string
         *
         * \param RPCCode The RPC Code to send
         * \param std::string The payload of the message
         * \return std::string The created message
         */
        std::string createMessage(RPCCode code, const std::string & message);
};

#endif // REMOTENODE_H
