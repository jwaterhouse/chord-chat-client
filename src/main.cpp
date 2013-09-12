#include <iostream>
#include <chrono>
#include <thread>
//#include "../include/main.h"
#include "../include/LocalNode.h"
#include "../include/RemoteNode.h"


int main(int argc, char* argv[])
{
    std::string ip1("localhost");

    std::cout << ip1 << "\n";

    Node n1(new LocalNode("n1", ip1, 1234));
    //n1->stabilize();

    std::cout << "ID 1: ";
    SHA1::hexPrinter((unsigned char*)(n1->getID().c_str()), ID_LEN);
    std::cout << "\n";


    std::cout << "Sleep a moment..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "and awaking." << std::endl;

    std::string ip2("localhost");
    Node r1(new RemoteNode(n1));
    Node n2(new LocalNode("n2", ip2, 1235, r1));
    //n2->stabilize();

    std::cout << "ID 2: ";
    SHA1::hexPrinter((unsigned char*)(n2->getID().c_str()), ID_LEN);
    std::cout << "\n";

    std::cout << "Sleep a moment..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "and awaking." << std::endl;

    //Node n3 = n2->findPredecessor(n1->getID())->getSuccessor();
    Node n3 = n1->getSuccessor();

    std::cout << "ID 3: ";
    SHA1::hexPrinter((unsigned char*)(n3->getID().c_str()), ID_LEN);
    std::cout << "\n";

    while(1) { }

    return 0;
}
