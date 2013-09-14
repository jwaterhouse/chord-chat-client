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
    printUsage();

    std::string line;
    while(1)
    {
        std::cout << "> ";
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

        size_t foundHelp = line.find("help");
        if(foundHelp == 0)
        {
            printUsage();
            continue;
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
        std::string message = line.substr(colon + 1);
        message = trim(message);
        if (message.length() == 0)
        {
            std::cerr << "Error - no message given." << std::endl;
            continue;
        }

        // Check if sending to self
        if (id == _n->getID())
        {
            std::cerr << "Error - cannot send message to self." << std::endl;
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
    Node n = _n->findSuccessor(id);
    if(n->getID() == id)
    {
        n->receive(message);
        return true;
    }
    return false;
}

void ChatClient::printUsage()
{
    std::cout << "Welcome to Chord Chat!" << std::endl;
    std::cout << "Usage\t- to send a message, type the recipients name followed by a double" << std::endl;
    std::cout << "\t  colon, then your chat message (e.g. \"bob: hi bob!\")" << std::endl;
    std::cout << "\t- for help, type \"help\"" << std::endl;
    std::cout << "\t- to quit, type \"quit\" or \"exit\"" << std::endl;

    std::cout << "Your name is '" + _n->getName() << "'." << std::endl;
}
