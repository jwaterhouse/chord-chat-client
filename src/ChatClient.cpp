#include <iostream>
#include "../include/ChatClient.h"
#include "../include/LocalNode.h"
#include "../include/StringTrim.h"

ChatClient::ChatClient(std::string name, std::string host, unsigned int port)
{
    _n = Node(new LocalNode(name, host, port));
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
    std::cout << "followed by a double colon, then your chat message." << std::endl;
    std::cout << "\t\tExample: \"bob: hi bob!\"" << std::endl;
    std::cout << "\t- to quit, type \"quit\" or \"exit\"" << std::endl;

    std::cout << "My name is " + _n->getName() + " and my ID is:" << std::endl;
    SHA1::hexPrinter((unsigned char*)(_n->getID().c_str()), ID_LEN);
    std::cout << std::endl << std::endl;

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
            // invalid rechostient format
            std::cerr << "Error - please specify rechostient." << std::endl;
            continue;
        }
        std::string name = line.substr(0, colon);
        name = trim(name);
        if (name.length() == 0)
        {
            // invalid rechostient format
            std::cerr << "Error - please specify rechostient." << std::endl;
            continue;
        }
        ID id(name);

        std::cout << "Sending to:\n";
        SHA1::hexPrinter((unsigned char*)id.c_str(), ID_LEN);

        // get the message
        std::string message = line.substr(colon + 1);
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
