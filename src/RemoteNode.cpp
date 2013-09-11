#include "../include/RemoteNode.h"
#include  <cstdlib>
#include <iostream>
#include <asio.hpp>

RemoteNode::RemoteNode(const char* serial, size_t s) : INode::INode()
{
    int ipLen = (int)serial[0];
    _ip = new std::string((char*)serial + 1, ipLen);
    _port = ((unsigned int)(serial + ipLen + 1));
    _port += ((unsigned int)(serial + ipLen + 1)) << 8;
    _port += ((unsigned int)(serial + ipLen + 1)) << 16;
    _port += ((unsigned int)(serial + ipLen + 1)) << 24;

    _id = new ID(*_ip, _port);
}

RemoteNode::~RemoteNode()
{
    //dtor
}

INode* RemoteNode::findPredecessor(const ID& id)
{
    std::string message = createMessage(RPCCode::FIND_PREDECESSOR, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    INode* n = new RemoteNode(reply.c_str(), reply.length());
    return n;
}

INode* RemoteNode::findSuccessor(const ID& id)
{
    std::string message = createMessage(RPCCode::FIND_SUCCESSOR, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    INode* n = new RemoteNode(reply.c_str(), reply.length());
    return n;
}

INode* RemoteNode::closestPrecedingFinger(const ID& id)
{
    std::string message = createMessage(RPCCode::CLOSEST_PRECEDING_FINGER, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    INode* n = new RemoteNode(reply.c_str(), reply.length());
    return n;
}

void RemoteNode::join(INode* n)
{
    std::cerr << "Not yet implemented." << std::endl;
}

void RemoteNode::initFingerTable(INode* n)
{
    std::cerr << "Not yet implemented." << std::endl;
}

void RemoteNode::updateOthers()
{
    std::cerr << "Not yet implemented." << std::endl;
}

void RemoteNode::updateFingerTable(INode* n, unsigned int k)
{
    std::cerr << "Not yet implemented." << std::endl;
}

void RemoteNode::stabilize()
{
    std::cerr << "Not yet implemented." << std::endl;
}

void RemoteNode::notify(INode* n)
{
    std::string message = createMessage(RPCCode::NOTIFY, n->serialize());
    std::string reply = sendMessage(message, false);
}

void RemoteNode::fixFingers()
{
    std::cerr << "Not yet implemented." << std::endl;
}

INode* RemoteNode::getPredecessor()
{
    std::string message = createMessage(RPCCode::GET_PREDECESSOR, "");
    std::string reply = sendMessage(message, true);
    INode* n = new RemoteNode(reply.c_str(), reply.length());
    return n;
}

void RemoteNode::setPredecessor(INode* n)
{
    std::string message = createMessage(RPCCode::SET_PREDECESSOR, n->serialize());
    std::string reply = sendMessage(message, false);
}

INode* RemoteNode::getSuccessor()
{
    std::string message = createMessage(RPCCode::GET_SUCCESSOR, "");
    std::string reply = sendMessage(message, true);
    INode* n = new RemoteNode(reply.c_str(), reply.length());
    return n;
}

void RemoteNode::setSuccessor(INode* n)
{
    std::string message = createMessage(RPCCode::SET_SUCCESSOR, n->serialize());
    std::string reply = sendMessage(message, false);
}

INode* RemoteNode::clone() const
{
    std::cerr << "Not yet implemented." << std::endl;
    return NULL;
}

std::string RemoteNode::sendMessage(std::string message, bool responseExpected = false)
{
    std::string portStr;
    std::ostringstream convert;
    convert << getPort();
    portStr = convert.str();

    try
    {
        asio::io_service io_service;
        asio::ip::tcp::socket s(io_service);
        asio::ip::tcp::resolver resolver(io_service);
        asio::ip::tcp::resolver::query query(getIP(), portStr);
        asio::connect(s, resolver.resolve(query));

        asio::write(s, asio::buffer(message));

        if (responseExpected)
        {
            char reply[MAX_DATA_LENGTH];
            size_t reply_length = asio::read(s, asio::buffer(reply, MAX_DATA_LENGTH));
            return std::string(reply, reply_length);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return std::string("");
}

std::string RemoteNode::createMessage(RPCCode code, const std::string& payLoad)
{
    char c = (char)code;
    return std::string(&c, 1) + payLoad;
}
