#ifndef INODE_H
#define INODE_H

#include <string>
#include <memory>
#include <functional>
#include "../include/ID.h"

#define MAX_DATA_LENGTH 1024
#define Node std::shared_ptr<INode>

enum class RPCCode
{
    FIND_PREDECESSOR,
    FIND_SUCCESSOR,
    CLOSEST_PRECEDING_FINGER,
    JOIN,
    STABILIZE,
    NOTIFY,
    FIX_FINGER,
    GET_PREDECESSOR,
    SET_PREDECESSOR,
    GET_SUCCESSOR,
    SET_SUCCESSOR,
    GET_ID,
    RECEIVE
};

class INode : public std::enable_shared_from_this<INode>
{
    public:

        /** \brief Construct a new Node with a name, host and port.
         *
         * \param name The Node's name. This is used to generate the Node's ID.
         * \param host Hostname or IP address for this Node to listen on.
         * \param port The port for this Node to listen on.
         */
        INode(const std::string& name, const std::string& host, unsigned int port)
        {
            _name = new std::string(name);
            _host = new std::string(host);
            _port = port;
            _id = new ID(*_name);
        }

        /** \brief Create a new Node from a serialization string.
         *
         * \param serial The serialized Node string.
         * \param length The length of the serial string.
         */
        INode(const char* serial, size_t length)
        {
            int position = 0;
            int nameLen = (int)serial[position];
            position++;
            _name = new std::string((char*)serial + position, nameLen);

            position += nameLen;
            int hostLen = (int)serial[position];
            position++;
            _host = new std::string((char*)serial + position, hostLen);

            position += hostLen;
            int portLen = (int)serial[position];
            position++;
            std::string portStr((char*)serial + position, portLen);

            _port = atoi(portStr.c_str());

            _id = new ID(*_name);
        }

        /** \brief Copy constructor.
         *
         * \param n The Node to copy
         */
        INode(const Node& n) : INode(n->getName(), n->getHost(), n->getPort()) { }

        virtual ~INode()
        {
            if (_id != 0)
            {
                delete _id;
                _id = 0;
            }

            if (_host != 0)
            {
                delete _host;
                _host = 0;
            }
        };

        /** \brief Find the predecessor of the given ID.
         *
         * \param ID The ID to find the predecessor of.
         * \return Node The Node preceding ID.
         */
        virtual Node findPredecessor(const ID&) = 0;

        /** \brief Find the successor of the given ID.
         *
         * \param ID The ID to find the successor of.
         * \return Node The Node succeeding ID.
         */
        virtual Node findSuccessor(const ID&) = 0;

        /** \brief Return the closest preceding finger of the given ID.
         *
         * \param ID The ID to find the CPF of.
         * \return Node The closest Node preceding ID.
         */
        virtual Node closestPrecedingFinger(const ID&) = 0;

        /** \brief Join an existing network through the given Node.
         *
         * \param Node Node of the network to join.
         * \return void
         */
        virtual void join(Node) = 0;

        /** \brief Stabilize this Node by checking it's successor.
         *
         * \return void
         */
        virtual void stabilize() = 0;

        /** \brief Notifies the given Node that this Node may be it's predecessor.
         *
         * \param Node The Node to notify.
         * \return void
         */
        virtual void notify(Node) = 0;

        /** \brief Called periodically to check the finger table.
         *
         * \return void
         */
        virtual void fixFingers() = 0;

        // Getters and setters;
        virtual Node getPredecessor() = 0;
        virtual void setPredecessor(Node) = 0;
        virtual Node getSuccessor() = 0;
        virtual void setSuccessor(Node) = 0;
        const ID getID() const { return ID(*_id); }
        const std::string getName() const { return std::string(*_name); }
        const std::string getHost() const { return std::string(*_host); }
        unsigned int getPort() const { return _port; }

        //virtual Node clone() const = 0;
        friend bool operator==(const INode& lhs, const INode& rhs) { return lhs.getID() == rhs.getID(); }
        virtual Node thisPtr() { return shared_from_this(); }
        virtual void setReceiveFunction(std::function<void(std::string)> rcvFn) { _rcvFn = rcvFn; }
        virtual void receive(std::string) = 0;
        virtual std::string serialize()
        {
            char nameLen = (char)(getName().length());
            char hostLen = (char)(getHost().length());
            std::string portStr;
            std::ostringstream convert;
            convert << getPort();
            portStr = convert.str();
            char portStrLen = (char)(portStr.length());
            return nameLen + getName() + hostLen + getHost() + portStrLen + portStr;
        }

    protected:
        std::string* _name = 0;
        ID* _id = 0;
        std::string* _host = 0;
        unsigned int _port = 0;
        std::function<void(std::string)> _rcvFn = 0;
};

#endif // INODE_H
