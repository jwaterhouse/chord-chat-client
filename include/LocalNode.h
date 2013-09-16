#ifndef LOCALNODE_H
#define LOCALNODE_H

#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <asio.hpp>
#include "../include/INode.h"
#include "../include/FingerTable.h"

#define TIME_PERIOD 0.1 // time to periodically call some methods, in seconds

class LocalNode : public INode
{
    public:
        LocalNode(const std::string&, const std::string&, unsigned int);
        LocalNode(const Node&);
        virtual ~LocalNode();

        // chord implementation methods
        virtual Node findPredecessor(const ID&);
        virtual Node findSuccessor(const ID&);
        virtual Node closestPrecedingFinger(const ID&);
        virtual void join(Node);
        virtual void stabilize();
        virtual void notify(Node);
        virtual void fixFingers();

        // Getters and setters;
        virtual Node getPredecessor();
        virtual void setPredecessor(Node);
        virtual Node getSuccessor();
        virtual void setSuccessor(Node);

        virtual void receive(std::string);

    private:
        void init();
        void checkPredecessor();
        void periodic();
        void server();
        void handleRequest(asio::ip::tcp::socket&);

    protected:
        virtual bool ping();

        Node _predecessor = 0;
        FingerTable _finger;
        std::thread* _periodicThread = 0;
        std::thread* _serverThread = 0;
        bool _serverRunning = true;
        std::mutex _m;
        bool _stop = false;
        unsigned int _next = 0;
};

#endif // LOCALNODE_H
