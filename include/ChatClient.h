#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <string>
#include "../include/INode.h"


class ChatClient
{
    public:
        /** \brief Constructor for ChatClient and corresponding LocalNode
         *
         * \param std::string Name of the node
         * \param std::string Host/IP of the node
         * \param unsigned int Port for the node
         */
        ChatClient(std::string, std::string, unsigned int);

        /** \brief Construct a chat client with a prebuilt Node
         *
         * \param Node The node for this ChatClient
         */
        ChatClient(Node);

        /**< Destructor */
        virtual ~ChatClient();

        /**< Run the ChatClient to process input and receive messages */
        void run();

    protected:
    private:

        /** \brief Processes a received message
         *
         * \param std::string The received message
         */
        void receiveMessage(std::string);

        /**< Print the usage details to the console */
        void printUsage();

        /** \brief Find a node and send a message
         *
         * \param ID The ID of the recipient
         * \param std::string The message to send
         * \return bool True of message sent, false if recipient not found
         */
        bool send(const ID&, const std::string&);

        /**< The LocalNode for this ChatClient */
        Node _n = 0;
};

#endif // CHATCLIENT_H
