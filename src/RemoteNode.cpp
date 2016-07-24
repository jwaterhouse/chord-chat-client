#include <cstdlib>
#include <iostream>

#include <asio.hpp>

#include "RemoteNode.h"

#include "Logger.h"

Node
RemoteNode::findPredecessor(const ID & id)
{
    std::string message = createMessage(RPCCode::FIND_PREDECESSOR, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));
        
        return n;
    }

    return NULL;
}

Node
RemoteNode::findSuccessor(const ID & id)
{
    std::string message = createMessage(RPCCode::FIND_SUCCESSOR, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));

        return n;
    }

    return NULL;
}

Node
RemoteNode::closestPrecedingFinger(const ID & id)
{
    std::string message = createMessage(RPCCode::CLOSEST_PRECEDING_FINGER, std::string(id.c_str(), ID_LEN));
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));

        return n;
    }

    return NULL;
}

void
RemoteNode::join(Node n)
{
    LOG->error("RemoteNode::join() - Not implemented");
}

void
RemoteNode::stabilize()
{
    LOG->error("RemoteNode::stabilize() - Not implemented");
}

void
RemoteNode::notify(Node n)
{
    std::string message = createMessage(RPCCode::NOTIFY, n->serialize());
    std::string reply = sendMessage(message, false);
}

void
RemoteNode::fixFingers()
{
    LOG->error("RemoteNode::fixFingers() - Not implemented");
}

bool
RemoteNode::ping()
{
    std::string message = createMessage(RPCCode::PING, "");
    try
    {
        std::string reply = sendMessage(message, false, true);
    }
    catch (const std::exception & e)
    {
        LOG->error("RemoteNode::ping() - Exception: {}", e.what());

        return false;
    }

    return true;
}

Node
RemoteNode::getPredecessor()
{
    std::string message = createMessage(RPCCode::GET_PREDECESSOR, "");
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));

        return n;
    }

    return NULL;
}

void
RemoteNode::setPredecessor(Node n)
{
    std::string message = createMessage(RPCCode::SET_PREDECESSOR, n->serialize());
    std::string reply = sendMessage(message, false);
}

Node
RemoteNode::getSuccessor()
{
    std::string message = createMessage(RPCCode::GET_SUCCESSOR, "");
    std::string reply = sendMessage(message, true);
    if (reply != "")
    {
        Node n(new RemoteNode(reply.c_str(), reply.length()));

        return n;
    }

    return NULL;
}

void
RemoteNode::setSuccessor(Node n)
{
    std::string message = createMessage(RPCCode::SET_SUCCESSOR, n->serialize());
    std::string reply = sendMessage(message, false);
}

void
RemoteNode::receive(const std::string & message)
{
    std::string msg = createMessage(RPCCode::RECEIVE, message);
    std::string reply = sendMessage(msg, false);
}

std::string
RemoteNode::sendMessage(const std::string & message, bool responseExpected, bool throwException)
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
    catch (const std::exception & e)
    {
        if (throwException)
        {
            throw e;
        }
        else
        {
            LOG->error("RemoteNode::sendMessage() - Exception: {}", e.what());
        }
    }
    return std::string("");
}

std::string
RemoteNode::createMessage(RPCCode code, const std::string & payLoad)
{
    char messageLength = (char)(1 + payLoad.length());
    return messageLength + std::string("") + char(code) + payLoad;
}
