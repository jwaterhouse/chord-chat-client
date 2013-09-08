#ifndef INODE_H
#define INODE_H

#include <string>
#include "../include/ID.h"

enum class RPCCodes
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
    INVALID_METHOD
};

class INode
{
    public:
        INode(std::string ip, unsigned int port)
        {
            init(ip, port);
        }

        virtual ~INode()
        {
            if (id != 0)
            {
                delete id;
                id = 0;
            }
        }

        virtual void init(std::string ip, unsigned int port)
        {
            this->setPredecessor(0);
            this->setSuccessor(this);
            this->ip = ip;
            this->port = port;
            this->id = new ID(ip, port);
        }

        virtual INode* findPredecessor(ID*) = 0;
        virtual INode* findSuccessor(ID*) = 0;
        virtual INode* closestPrecedingFinger(ID*) = 0;
        virtual void join(INode*) = 0;
        virtual void initFingerTable(INode*) = 0;
        virtual void updateOthers() = 0;
        virtual void updateFingerTable(INode*, unsigned int) = 0;
        virtual void stabilize() = 0;
        virtual void notify(INode*) = 0;
        virtual void fixFingers() = 0;

        // Getters and setters;
        virtual INode* getPredecessor() = 0;
        virtual void setPredecessor(INode* n) = 0;
        virtual INode* getSuccessor() = 0;
        virtual void setSuccessor(INode* n) = 0;
        virtual const ID* getID() const
        {
            return id;
        }

    protected:
        ID* id;
        std::string ip;
        unsigned int port;

};

#endif // INODE_H
