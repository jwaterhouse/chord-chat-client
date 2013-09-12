#ifndef LOCALNODE_H
#define LOCALNODE_H

#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <asio.hpp>
#include "../include/INode.h"
#include "../include/FingerTable.h"

#define TIME_PERIOD 1.0 // time to periodically call some methods, in seconds

class LocalNode : public INode
{
    public:
        LocalNode(const std::string&, const std::string&, unsigned int);
        LocalNode(const std::string&, const std::string&, unsigned int, Node);
        LocalNode(const INode&);
        virtual ~LocalNode();

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

        //virtual Node thisPtr() { return shared_from_this(); }

    private:
        void init();
        void checkPredecessor();
        void periodic();
        void server();
        std::string handleRequest(std::string);

    protected:
        Node _predecessor = 0;
        FingerTable* _finger = 0;
        std::thread* _periodicThread = 0;
        std::thread* _serverThread = 0;
        bool _serverRunning = true;
        std::mutex* _m = 0;
        std::mutex* _socketM = 0;
        bool _stop = false;
};

#endif // LOCALNODE_H
