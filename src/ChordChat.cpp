#include <iostream>
#include <chrono>
#include <thread>
#include "../include/LocalNode.h"
#include "../include/RemoteNode.h"
#include "../include/ChatClient.h"
#include "../include/StringTrim.h"

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
        // invalid rechostient format
        std::cerr << "Error - invalid node format, no '@' symbol." << std::endl;
        return NULL;
    }
    std::string name = str.substr(0, nameSep);
    name = trim(name);
    if (name.length() == 0)
    {
        // invalid rechostient format
        std::cerr << "Error - please specify name." << std::endl;
        return NULL;
    }

    size_t hostSep = str.find(":");
    if (hostSep == std::string::npos)
    {
        // invalid rechostient format
        std::cerr << "Error - invalid node format, no ':' symbol." << std::endl;
        return NULL;
    }
    std::string host = str.substr(nameSep + 1, hostSep - (nameSep + 1));
    host = trim(host);
    if (host.length() == 0)
    {
        // invalid rechostient format
        std::cerr << "Error - please specify host." << std::endl;
        return NULL;
    }

    std::string portStr = str.substr(hostSep + 1, str.length() - (hostSep + 1));
    portStr = trim(portStr);
    if (portStr.length() == 0)
    {
        // invalid rechostient format
        std::cerr << "Error - please specify port." << std::endl;
        return NULL;
    }

    unsigned int port = (unsigned int)atoi(portStr.c_str());

    if (local) return Node(new LocalNode(name, host, port));
    else return Node(new RemoteNode(name, host, port));
}

int main(int argc, char* argv[])
{
    /*
    char test1[ID_LEN] = {'\0'};
    char test2[ID_LEN] = {'\0'};
    test1[0] = 0x21;
    test2[0] = 0x01;

    ID t1(test1);
    ID t2(test2);
    std::cout << "Test 1:" << std::endl;
    SHA1::hexPrinter((unsigned char*)(t1.c_str()), ID_LEN);
    std::cout << std::endl;
    std::cout << "Test 2:" << std::endl;
    SHA1::hexPrinter((unsigned char*)(t2.c_str()), ID_LEN);
    std::cout << std::endl;

    ID t3 = t1 - t2;
    std::cout << "Test 3:" << std::endl;
    SHA1::hexPrinter((unsigned char*)(t3.c_str()), ID_LEN);
    std::cout << std::endl;

    std::cout << "Test 3: " << (t1 > t2 ? "true" : "false") << std::endl;

    return 0;
    */

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


