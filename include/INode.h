#ifndef INODE_H
#define INODE_H

#include <string>
#include "../include/ID.h"

#define MAX_DATA_LENGTH 1024

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
    GET_ID
};

class INode
{
    public:
        INode() { };
        INode(const std::string& ip, unsigned int port)
        {
            _ip = new std::string(ip.c_str(), ip.length());
            _port = port;
            _id = new ID(*_ip, _port);
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

        virtual INode* findPredecessor(const ID&) = 0;
        virtual INode* findSuccessor(const ID&) = 0;
        virtual INode* closestPrecedingFinger(const ID&) = 0;
        virtual void join(INode*) = 0;
        virtual void initFingerTable(INode*) = 0;
        virtual void updateOthers() = 0;
        virtual void updateFingerTable(INode*, unsigned int) = 0;
        virtual void stabilize() = 0;
        virtual void notify(INode*) = 0;
        virtual void fixFingers() = 0;

        // Getters and setters;
        virtual INode* getPredecessor() = 0;
        virtual void setPredecessor(INode*) = 0;
        virtual INode* getSuccessor() = 0;
        virtual void setSuccessor(INode*) = 0;
        const ID getID() const { return ID(*_id); }
        const std::string getIP() const { return std::string(*_ip); }
        unsigned int getPort() const { return _port; }

        virtual INode* clone() const = 0;
        virtual std::string serialize()
        {
            char portCharArr[4];
            portCharArr[0] = (char)_port;
            portCharArr[1] = (char)(_port >> 8);
            portCharArr[2] = (char)(_port >> 16);
            portCharArr[3] = (char)(_port >> 24);
            char ipLen = (char)(getIP().length());
            return std::string(&ipLen) + getIP() + std::string(portCharArr, 4);
        }

    protected:
        ID* _id = 0;
        std::string* _ip = 0;
        unsigned int _port = 0;

};

#endif // INODE_H
