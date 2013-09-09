#include <iostream>
//#include "../include/main.h"
#include "../include/LocalNode.h"


int main(int argc, char* argv[])
{
    std::string ip1("192.168.1.1");

    std::cout << ip1 << "\n";

    LocalNode* n1 = new LocalNode(ip1, 22);

    std::cout << "ID 1: ";
    SHA1::hexPrinter((byte*)(n1->getID().c_str()), ID_LEN);
    std::cout << "\n";

    std::string ip2("192.168.2.2");
    LocalNode* n2 = new LocalNode(ip2, 23, (INode*)n1);

    std::cout << "ID 2: ";
    SHA1::hexPrinter((byte*)(n2->getID().c_str()), ID_LEN);
    std::cout << "\n";

    return 0;
}
