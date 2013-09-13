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
    INIT_FINGER_TABLE,
    UPDATE_OTHERS,
    UPDATE_FINGER_TABLE,
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
        INode() { };
        INode(const std::string& name, const std::string& ip, unsigned int port)
        {
            _name = new std::string(name);
            _ip = new std::string(ip);
            _port = port;
            //_id = new ID(*_ip, _port);
            _id = new ID(*_name);
        }

        virtual ~INode()
        {
            if (_id != 0)
            {
                delete _id;
                _id = 0;
            }

            if (_ip != 0)
            {
                delete _ip;
                _ip = 0;
            }
        };

        // Chord implementation methods
        virtual Node findPredecessor(const ID&) = 0;
        virtual Node findSuccessor(const ID&) = 0;
        virtual Node closestPrecedingFinger(const ID&) = 0;
        virtual void join(Node) = 0;
        virtual void initFingerTable(Node) = 0;
        virtual void updateOthers() = 0;
        virtual void updateFingerTable(Node, unsigned int) = 0;
        virtual void stabilize() = 0;
        virtual void notify(Node) = 0;
        virtual void fixFingers() = 0;

        // Getters and setters;
        virtual Node getPredecessor() = 0;
        virtual void setPredecessor(Node) = 0;
        virtual Node getSuccessor() = 0;
        virtual void setSuccessor(Node) = 0;
        const ID getID() const { return ID(*_id); }
        const std::string getName() const { return std::string(*_name); }
        const std::string getIP() const { return std::string(*_ip); }
        unsigned int getPort() const { return _port; }

        //virtual Node clone() const = 0;
        friend bool operator==(const INode& lhs, const INode& rhs) { return lhs.getID() == rhs.getID(); }
        virtual Node thisPtr() { return shared_from_this(); }
        virtual void setReceiveFunction(std::function<void(std::string)> rcvFn) { _rcvFn = rcvFn; }
        virtual void receive(std::string) = 0;
        virtual std::string serialize()
        {
            char nameLen = (char)(getName().length());
            char ipLen = (char)(getIP().length());
            std::string portStr;
            std::ostringstream convert;
            convert << getPort();
            portStr = convert.str();
            char portStrLen = (char)(portStr.length());
            return nameLen + getName() + ipLen + getIP() + portStrLen + portStr;
        }

    protected:
        std::string* _name = 0;
        ID* _id = 0;
        std::string* _ip = 0;
        unsigned int _port = 0;
        std::function<void(std::string)> _rcvFn = 0;
};

#endif // INODE_H
