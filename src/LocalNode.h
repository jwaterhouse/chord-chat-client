#ifndef LOCALNODE_H
#define LOCALNODE_H

#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <asio.hpp>

#include "INode.h"
#include "FingerTable.h"

#define TIME_PERIOD 0.1 /**< The time to periodically call maintenance methods, in seconds. */

class LocalNode : public INode
{
    public:
        /** \brief Constructs a new LocalNode.
         *
         * \param std::string Name of the node.
         * \param std::string Host/IP of the node.
         * \param unsigned int Port for the node to communicate on.
         */
        LocalNode(const std::string&, const std::string&, unsigned int);

        /** \brief Copy constructor
         *
         * \param Node The Node to copy
         */
        LocalNode(const Node&);

        /**< Destructor */
        virtual ~LocalNode();

        // chord implementation methods
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
        /**< Pointer to this node's predecessor */
        Node _predecessor = 0;

        /**< The finger table for this node */
        FingerTable _finger;

        /**< Pointer to thread that is executed periodically */
        std::thread* _periodicThread = 0;

        /**< Pointer to server thread to handle communication between nodes */
        std::thread* _serverThread = 0;

        /**< Boolean to check if the communication server is runnning */
        bool _serverRunning = true;

        /**< Mutex */
        std::mutex _m;

        /**< Signal for the threads to signal they should exit */
        bool _stop = false;

        /**< Initialisation method called by the constructor */
        void init();

        /**< Polls the predecessor mode to see if it is available */
        void checkPredecessor();

        /**< Method for the periodic thread */
        void periodic();

        /**< Method for the communication server */
        void server();

        /**< Helper method to handle received messages */
        void handleRequest(asio::ip::tcp::socket&);
};

#endif // LOCALNODE_H
