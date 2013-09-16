#include "../include/RemoteNode.h"
#include  <cstdlib>
#include <iostream>
#include <asio.hpp>

Node RemoteNode::findPredecessor(const ID& id)
{
    std::string message = createMessage(RPCCode::FIND_PREDECESSOR, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));
        return n;
    }
    else return NULL;
}

Node RemoteNode::findSuccessor(const ID& id)
{
    std::string message = createMessage(RPCCode::FIND_SUCCESSOR, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));
        return n;
    }
    else return NULL;
}

Node RemoteNode::closestPrecedingFinger(const ID& id)
{
    std::string message = createMessage(RPCCode::CLOSEST_PRECEDING_FINGER, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));
        return n;
    }
    else return NULL;
}

void RemoteNode::join(Node n)
{
    std::cerr << "RemoteNode::join() - Not yet implemented." << std::endl;
}

void RemoteNode::stabilize()
{
    std::cerr << "RemoteNode::stabilize() - Not yet implemented." << std::endl;
}

void RemoteNode::notify(Node n)
{
    std::string message = createMessage(RPCCode::NOTIFY, n->serialize());
    std::string reply = sendMessage(message, false);
}

void RemoteNode::fixFingers()
{
    std::cerr << "RemoteNode::fixFingers() - Not yet implemented." << std::endl;
}

bool RemoteNode::ping()
{
    std::string message = createMessage(RPCCode::PING, "");
    try
    {
        std::string reply = sendMessage(message, false, true);
    }
    catch (std::exception& e)
    {
        // Communication error
        return false;
    }
    return true;
}

Node RemoteNode::getPredecessor()
{
    std::string message = createMessage(RPCCode::GET_PREDECESSOR, "");
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));
        return n;
    }
    else return NULL;
}

void RemoteNode::setPredecessor(Node n)
{
    std::string message = createMessage(RPCCode::SET_PREDECESSOR, n->serialize());
    std::string reply = sendMessage(message, false);
}

Node RemoteNode::getSuccessor()
{
    std::string message = createMessage(RPCCode::GET_SUCCESSOR, "");
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));
        return n;
    }
    else return NULL;
}

void RemoteNode::setSuccessor(Node n)
{
    std::string message = createMessage(RPCCode::SET_SUCCESSOR, n->serialize());
    std::string reply = sendMessage(message, false);
}

void RemoteNode::receive(std::string message)
{
    std::string msg = createMessage(RPCCode::RECEIVE, message);
    std::string reply = sendMessage(msg, false);
}

std::string RemoteNode::sendMessage(std::string message, bool responseExpected, bool throwException)
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
        asio::ip::tcp::resolver::query query(getHost(), portStr);
        asio::connect(s, resolver.resolve(query));

        asio::write(s, asio::buffer(message));

        if (responseExpected)
        {
            char header = '\0';
            size_t reply_length = asio::read(s, asio::buffer(&header, 1));
            int payloadLength = (int)header;

            char reply[MAX_DATA_LENGTH] = {'\0'};
            reply_length = asio::read(s, asio::buffer(reply, payloadLength));
            return std::string(reply, reply_length);
        }
    }
    catch (std::exception& e)
    {
        if (throwException)
            throw e;
        else
            std::cerr << "Exception in " << getName() << "(remote)::sendMessage() - " << e.what() << std::endl;
    }
    return std::string("");
}

std::string RemoteNode::createMessage(RPCCode code, const std::string& payLoad)
{
    char messageLength = (char)(1 + payLoad.length());
    char c = (char)code;
    return messageLength + std::string("") + c + payLoad;
}
