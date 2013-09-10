#include <iostream>
//#include "../include/main.h"
#include "../include/LocalNode.h"


int main(int argc, char* argv[])
{
    std::string ip1("192.168.1.1");

    std::cout << ip1 << "\n";

    LocalNode* n1 = new LocalNode(ip1, 1234);
    n1->stabilize();

    std::cout << "ID 1: ";
    SHA1::hexPrinter((byte*)(n1->getID().c_str()), ID_LEN);
    std::cout << "\n";

    std::string ip2("192.168.2.2");
    LocalNode* n2 = new LocalNode(ip2, 1235, n1);
    n2->stabilize();

    std::cout << "ID 2: ";
    SHA1::hexPrinter((byte*)(n2->getID().c_str()), ID_LEN);
    std::cout << "\n";

    //INode* n3 = n2->findPredecessor(n1->getID())->getSuccessor();
    INode* n3 = n1->getSuccessor();

    std::cout << "ID 3: ";
    SHA1::hexPrinter((byte*)(n3->getID().c_str()), ID_LEN);
    std::cout << "\n";

    while(1) { }

    return 0;
}
