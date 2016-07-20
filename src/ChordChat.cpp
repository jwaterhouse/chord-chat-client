#include <iostream>
#include <chrono>
#include <thread>

#include "LocalNode.h"
#include "RemoteNode.h"
#include "ChatClient.h"
#include "StringTrim.h"

static void printUsage()
{
    std::cout << "Usage:\tChordChat name@host:port <name@host:port>" << std::endl;
    std::cout << "\twhere:" << std::endl;
    std::cout << "\t\tname@host:port is the configuration for this local node, and" << std::endl;
    std::cout << "\t\t<name@host:port> is an optional remote node to join." << std::endl;
}

static Node createNodeFromString(std::string str, bool local)
{
    size_t nameSep = str.find("@");
    if (nameSep == std::string::npos)
    {
        // invalid recipient format
        std::cerr << "Error - invalid node format, no '@' symbol." << std::endl;
        return NULL;
    }
    std::string name = str.substr(0, nameSep);
    name = trim(name);
    if (name.length() == 0)
    {
        // invalid recipient format
        std::cerr << "Error - please specify name." << std::endl;
        return NULL;
    }

    size_t hostSep = str.find(":");
    if (hostSep == std::string::npos)
    {
        // invalid recipient format
        std::cerr << "Error - invalid node format, no ':' symbol." << std::endl;
        return NULL;
    }
    std::string host = str.substr(nameSep + 1, hostSep - (nameSep + 1));
    host = trim(host);
    if (host.length() == 0)
    {
        // invalid recipient format
        std::cerr << "Error - please specify host." << std::endl;
        return NULL;
    }

    std::string portStr = str.substr(hostSep + 1, str.length() - (hostSep + 1));
    portStr = trim(portStr);
    if (portStr.length() == 0)
    {
        // invalid recipient format
        std::cerr << "Error - please specify port." << std::endl;
        return NULL;
    }

    unsigned int port = (unsigned int)atoi(portStr.c_str());

    if (local) return Node(new LocalNode(name, host, port));
    else return Node(new RemoteNode(name, host, port));
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        printUsage();
        return 0;
    }

    Node localNode = createNodeFromString(std::string(argv[1]), true);
    if (localNode == NULL) return 1;
    if (argc == 3)
    {
        Node remoteNode = createNodeFromString(std::string(argv[2]), false);
        if (remoteNode == NULL) return 1;
        localNode->join(remoteNode);
    }

    ChatClient c(localNode);
    c.run();

    return 0;
}
