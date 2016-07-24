#include <chrono>
#include <thread>

#include "LocalNode.h"

#include "Logger.h"
#include "RemoteNode.h"

LocalNode::LocalNode(const std::string & name, const std::string & host, unsigned int port) : INode::INode(name, host, port)
{
    init();
}

LocalNode::LocalNode(const Node & n) : INode::INode(n)
{
    init();
}

LocalNode::~LocalNode()
{
    if (_periodicThread != 0)
    {
        _stop = true;
        _periodicThread->join();
        delete _periodicThread;
        _periodicThread = 0;
    }
    if (_serverThread != 0)
    {
        _stop = true;
        _serverThread->join();
        delete _serverThread;
        _serverThread = 0;
    }
}

void
LocalNode::init()
{
    _finger.setId(_id);
    setPredecessor(nullptr);
    //setSuccessor(thisPtr());  //cannot set as part of constructor

    // start the threads
    _serverRunning = true;
    _periodicThread = new std::thread(&LocalNode::periodic, this);
    _serverThread = new std::thread(&LocalNode::server, this);
}

Node
LocalNode::findPredecessor(const ID & id)
{
    LOG->debug("LocalNode::findPredecessor(id={})", id.toHexString());

    Node n = thisPtr();
    while (!(id.isInInterval(n->getId(), n->getSuccessor()->getId()) || id == n->getSuccessor()->getId()))
    {
        n = n->closestPrecedingFinger(id);
    }

    return n;
}

Node
LocalNode::findSuccessor(const ID & id)
{
    LOG->debug("LocalNode::findSuccessor(id={})", id.toHexString());

    if (getId() == id)
    {
        return thisPtr();
    }

    Node s = getSuccessor();
    if (id.isInInterval(getId(), s->getId()) || id == s->getId())
    {
        return s;
    }
    else
    {
        Node n = closestPrecedingFinger(id);
        return n->findSuccessor(id);
    }
}

Node
LocalNode::closestPrecedingFinger(const ID & id)
{
    LOG->debug("LocalNode::closestPrecedingFinger(id={})", id.toHexString());

    for (int i = M - 1; i >= 0; --i)
    {
        Node f = _finger.getNode(i);;
        if (f != nullptr && f->getId().isInInterval(getId(), id))
        {
            return f;
        }
    }

    return thisPtr();
}

void
LocalNode::join(Node n)
{
    LOG->debug("LocalNode::join(n={})", n->getId().toHexString());

    setPredecessor(nullptr);
    setSuccessor(n->findSuccessor(getId()));
}

void
LocalNode::stabilize()
{
    LOG->debug("LocalNode::stabilize()");

    // check the successor first?
    Node s = getSuccessor();
    while (s->ping() == false)
    {
        // fix the finger table
        _finger.removeNode(s);
        s = getSuccessor();
    }

    Node x = getSuccessor()->getPredecessor();
    if (x != nullptr && (x->getId().isInInterval(getId(), getSuccessor()->getId())))
    {
        setSuccessor(x);
    }
    getSuccessor()->notify(thisPtr());
}

void
LocalNode::notify(Node n)
{
    LOG->debug("LocalNode::notify(n={})", n->getId().toHexString());
    if (getPredecessor() == nullptr || n->getId().isInInterval(getPredecessor()->getId(), getId()))
    {
        setPredecessor(n);
    }
}

void
LocalNode::fixFingers()
{
    LOG->debug("LocalNode::fixFingers()");

    // get a random number between from 1 to M - 1
    unsigned int i = (unsigned int)rand() % (M - 2) + 1;
    ID sID = _finger.start(i);
    Node n = findSuccessor(sID);
    if (n != nullptr && n->getId() == getId())
    {
        n = thisPtr();
    }
    _finger.setNode(i, n);
}

void
LocalNode::checkPredecessor()
{
    LOG->debug("LocalNode::checkPredecessor()");

    Node n = getPredecessor();
    if (n != nullptr && n != thisPtr() && n->ping() == false)
    {
        setPredecessor(nullptr);
    }
}

bool
LocalNode::ping()
{
    return true;
}

Node
LocalNode::getPredecessor()
{
    std::lock_guard<std::mutex> lock(_m);

    return _predecessor;
}

void
LocalNode::setPredecessor(Node n)
{
    std::lock_guard<std::mutex> lock(_m);
    if (n != nullptr && n->getId() == getId())
    {
        n = thisPtr();
    }
    _predecessor = n;
}

Node
LocalNode::getSuccessor()
{
    std::lock_guard<std::mutex> lock(_m);
    if (_finger.getNode(0) == nullptr)
    {
        _finger.setNode(0, thisPtr());
    }

    return _finger.getNode(0);
}

void
LocalNode::setSuccessor(Node n)
{
    std::lock_guard<std::mutex> lock(_m);
    if (n != nullptr && n->getId() == getId())
    {
        n = thisPtr();
    }
    _finger.setNode(0, n);
}

void
LocalNode::receive(const std::string & message)
{
    _rcvFn(message);
}

void
LocalNode::periodic()
{
    LOG->debug("LocalNode::periodic() - running");

    // construct a timer
    for (;;)
    {
        LOG->debug("LocalNode::periodic() - loop");

        // check if the thread needs to exit
        if (_stop)
        {
            break;;
        }

        checkPredecessor();
        stabilize();
        fixFingers();

        // check the server process, try to restart it if necessary
        if (!_serverRunning)
        {
            if (_serverThread != 0)
            {
                _serverThread->join();
                delete _serverThread;
            }
            _serverRunning = true;
            _serverThread = new std::thread(&LocalNode::server, this);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(TIME_PERIOD));
    }

    LOG->debug("LocalNode::periodic() - stopped");
}

void
LocalNode::server()
{
    _serverRunning = true;
    std::vector<std::thread> threads();
    try
    {
        asio::io_service io_service;
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), getPort());
        asio::ip::tcp::acceptor acceptor(io_service, endpoint);
        for (;;)
        {
            // check if the thread needs to exit
            if (_stop)
            {
                break;
            }

            // create a new socket for this connection and listen
            asio::ip::tcp::socket socket(io_service);
            acceptor.listen();
            acceptor.accept(socket);

            // release a new thread to handle the request
            std::thread t(std::bind(&LocalNode::handleRequest, this, std::move(socket)));
            t.detach(); // dangerous?
        }
    }
    catch (std::exception& e)
    {
        LOG->error("LocalNode::server() - Exception: {}", e.what());
    }
    _serverRunning = false;
}

void
LocalNode::handleRequest(asio::ip::tcp::socket & socket)
{
    try
    {
        char message[MAX_DATA_LENGTH] = {'\0'};
        asio::error_code error;
        std::string response = "";

        // parse the message - first byte is the header, which is just the
        // length of the rest of the message
        char header = '\0';
        size_t length = asio::read(socket, asio::buffer(&header, 1), error);
        int messageLength = (int)header;
        if (error == asio::error::eof)
        {
            // Connection closed cleanly by peer.
            return;
        }
        else if (error)
        {
            throw asio::system_error(error);
        }

        // retrieve the rest of the message
        length = asio::read(socket, asio::buffer(message, messageLength), error);
        if (error == asio::error::eof)
        {
            // Connection closed cleanly by peer.
            return;
        }
        else if (error)
        {
            throw asio::system_error(error);
        }

        // first byte of the message is the RPC code
        RPCCode code = (RPCCode)message[0];
        int offset = 1;
        switch (code)
        {
            case RPCCode::FIND_PREDECESSOR:
            {
                if (length != ID_LEN + 1)
                {
                    LOG->error("LocalNode::server() - message length incorrent. (length={})", length);
                }
                ID id((char*)(message + offset));
                Node n = findPredecessor(id);
                response = n->serialize();
            }
            break;
            case RPCCode::FIND_SUCCESSOR:
            {
                if (length != ID_LEN + 1)
                {
                    LOG->error("LocalNode::server() - message length incorrent. (length={})", length);
                }
                ID id((char*)(message + offset));
                Node n = findSuccessor(id);
                response = n->serialize();
            }
            break;
            case RPCCode::CLOSEST_PRECEDING_FINGER:
            {
                if (length != ID_LEN + 1)
                {
                    LOG->error("LocalNode::server() - message length incorrent. (length={})", length);
                }
                ID id((char*)(message + offset));
                Node n = closestPrecedingFinger(id);
                response = n->serialize();
            }
            break;
            case RPCCode::JOIN:
            {
                LOG->error("LocalNode::server() - join() not implemented.");
            }
            break;
            case RPCCode::STABILIZE:
            {
                LOG->error("LocalNode::server() - stabilize() not implemented.");
            }
            break;
            case RPCCode::NOTIFY:
            {
                Node n(new RemoteNode(message + offset, length - offset));
                notify(n);
            }
            break;
            case RPCCode::FIX_FINGER:
            {
                LOG->error("LocalNode::server() - fixFinger() not implemented.");
            }
            break;
            case RPCCode::GET_PREDECESSOR:
            {
                response = getPredecessor()->serialize();
            }
            break;
            case RPCCode::SET_PREDECESSOR:
            {
                Node n;
                if ((length - offset) == 0)
                {
                    n = NULL;
                }
                else
                {
                    n = Node(new RemoteNode(message + offset, length - offset));
                }
                setPredecessor(n);
            }
            break;
            case RPCCode::GET_SUCCESSOR:
            {
                response = getSuccessor()->serialize();
            }
            break;
            case RPCCode::SET_SUCCESSOR:
            {
                Node n;
                if ((length - offset) == 0)
                {
                    n = NULL;
                }
                else
                {
                    n = Node(new RemoteNode(message + offset, length - offset));
                }
                setSuccessor(n);
            }
            break;
            case RPCCode::GET_ID:
            {
                response = std::string(getId().c_str(), ID_LEN);
            }
            break;
            case RPCCode::RECEIVE:
            {
                std::string receivedMessage(message + offset, length - offset);
                _rcvFn(receivedMessage);
            }
            break;
            case RPCCode::PING:
            {
            }
            break;
            default:
                LOG->error("LocalNode::server() - Invalid RPC code received. (code={})", (int)code);
        }

        char responseLength = (int)(response.length());
        std::string responseMessage = responseLength + response;
        if (responseLength > 0)
        {
            asio::write(socket, asio::buffer(responseMessage));
        }
    }
    catch (std::exception& e)
    {
        LOG->error("LocalNode::server() - Exception: {}", e.what());
    }
    socket.close();
}
