#ifndef LOCALNODE_H
#define LOCALNODE_H

#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <asio.hpp>

#include "INode.h"
#include "FingerTable.h"

#define TIME_PERIOD 10000 /**< The time to periodically call maintenance methods, in milliseconds. */

class LocalNode : public INode
{
    public:
        /** \brief Constructs a new LocalNode.
         *
         * \param std::string Name of the node.
         * \param std::string Host/IP of the node.
         * \param unsigned int Port for the node to communicate on.
         */
        LocalNode(const std::string & name, const std::string & host, unsigned int port);

        /** \brief Copy constructor
         *
         * \param Node The Node to copy
         */
        LocalNode(const Node & n);

        /**< Destructor */
        virtual ~LocalNode();

        // chord implementation methods
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
        void handleRequest(asio::ip::tcp::socket & socket);
};

#endif // LOCALNODE_H
