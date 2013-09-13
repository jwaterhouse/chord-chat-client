#include <iostream>
#include "../include/ChatClient.h"
#include "../include/LocalNode.h"
#include "../include/StringTrim.h"

ChatClient::ChatClient(std::string name, std::string ip, unsigned int port)
{
    _n = Node(new LocalNode(name, ip, port));
    _n->setReceiveFunction(std::bind(&ChatClient::receiveMessage, this, std::placeholders::_1));
}

ChatClient::ChatClient(Node n)
{
    _n = n;
    _n->setReceiveFunction(std::bind(&ChatClient::receiveMessage, this, std::placeholders::_1));
}

ChatClient::~ChatClient()
{
    //dtor
}

void ChatClient::run()
{
    std::cout << "Welcome to Chord Chat!" << std::endl;
    std::cout << "Usage\t- to send a message, type the recipients name ";
    std::cout << "followed by a double colon, then followed by your chat message." << std::endl;
    std::cout << "\t\tExample: \"bob: hi bob!\"" << std::endl;
    std::cout << "\t- to quit, type \"quit\" or \"exit\"" << std::endl;

    std::string line;
    while(1)
    {
        std::getline(std::cin, line);
        line = trim(line);

        // check if we should quit the chat session
        size_t foundExit = line.find("exit");
        size_t foundQuit = line.find("quit");
        if(foundExit == 0 || foundQuit == 0)
        {
            std::cerr << "Exiting... Thanks for playing!" << std::endl;
            break;
        }

        if (line.length() == 0)
        {
            // empty string
            continue;
        }

        size_t colon = line.find(":");
        if (colon == std::string::npos)
        {
            // invalid recipient format
            std::cerr << "Error - please specify recipient." << std::endl;
            continue;
        }
        std::string name = line.substr(0, colon);
        name = trim(name);
        if (name.length() == 0)
        {
            // invalid recipient format
            std::cerr << "Error - please specify recipient." << std::endl;
            continue;
        }
        ID id(name);

        // get the message
        std::string message = line.substr(colon + 1, line.length());
        message = trim(message);
        if (message.length() == 0)
        {
            std::cerr << "Error - no message given." << std::endl;
            continue;
        }

        // add the sender to the message
        message = "(from " + _n->getName() + ") " + message;

        bool exists = send(id, message);

        if (!exists)
        {
            std::cerr << "Error - could not find \"" << name << "\"." << std::endl;
        }
    }
}

void ChatClient::receiveMessage(std::string message)
{
    std::cout << message << std::endl;
}

bool ChatClient::send(const ID& id, const std::string& message)
{
    Node n = _n->findSuccessor(id)->getPredecessor();
    if(n->getID() == id)
    {
        n->receive(message);
        return true;
    }
    return false;
}
