#include "../include/LocalNode.h"
#include <cstdlib>
#include <ctime>

LocalNode::LocalNode(const std::string& ip, unsigned int port) : INode::INode(ip, port)
{
    init();
}

LocalNode::LocalNode(const std::string& ip, unsigned int port, INode* n) : INode::INode(ip, port)
{
    init();
    join(n);
}

LocalNode::LocalNode(const INode& n) : INode::INode(n.getIP(), n.getPort())
{
    init();
}

LocalNode::~LocalNode()
{
    if (_predecessor != 0 && _predecessor != this)
    {
        delete _predecessor;
        _predecessor = 0;
    }
    if (_finger != 0)
    {
        delete _finger;
        _finger = 0;
    }
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
    if (_m != 0)
    {
        delete _m;
        _m = 0;
    }
}

void LocalNode::init()
{
    _finger = new FingerTable(*this);
    setPredecessor(0);
    setSuccessor(this);

    // start the threads
    _periodicThread = new std::thread(&LocalNode::periodic, this);
    _serverRunning = true;
    _serverThread = new std::thread(&LocalNode::server, this);
}

INode* LocalNode::findPredecessor(const ID& id)
{
    INode* n = this;
    while (!(id.isInInterval(n->getID(), n->getSuccessor()->getID())
            || id == n->getSuccessor()->getID()))
    {
        n = n->closestPrecedingFinger(id);
    }
    return n;
}

INode* LocalNode::findSuccessor(const ID& id)
{
    INode* n = findPredecessor(id);
    return n->getSuccessor();
}

INode* LocalNode::closestPrecedingFinger(const ID& id)
{
    for (int i = M - 1; i >= 0; --i)
    {
        if (_finger->node(i)->getID().isInInterval(getID(), id))
            return _finger->node(i);
    }
    return this;
}

void LocalNode::join(INode* n)
{
    /*
    if (n != 0)
    {
        initFingerTable(n);
        updateOthers();
    }
    else
    {
        for(unsigned int i = 0; i < M; ++i)
        {
            _finger->setNode(i, n);
        }
        setPredecessor(n);
    }
    */
    setPredecessor(0);
    setSuccessor(n->findSuccessor(this->getID()));
}

void LocalNode::initFingerTable(INode* n)
{
    ID start = _finger->start(0);
    _finger->setNode(0, n->findSuccessor(_finger->start(0)));
    setPredecessor(getSuccessor()->getPredecessor());
    getSuccessor()->setPredecessor(this);

    for (int i = 0; i < M; ++i)
    {
        ID si = _finger->start(i + 1);
        if (si.isInInterval(getID(), _finger->node(i)->getID())
            || si == getID())
        {
            _finger->setNode(i + 1, _finger->node(i));
        }
        else
        {
            _finger->setNode(i + 1, n->findSuccessor(si));
        }
    }
}

void LocalNode::updateOthers()
{
    for (int i = 0; i < M; ++i)
    {
        byte b[ID_LEN];
        b[ID_LEN - i / 8 - 1] = (0x01 << (i % 8));

        ID bID(b);
        ID nId = *_id - bID;

        INode* p = findPredecessor(nId);
        p->updateFingerTable(this, i);
    }
}

void LocalNode::updateFingerTable(INode* s, unsigned int i)
{
    if (s->getID().isInInterval(getID(), _finger->node(i)->getID())
        || s->getID() == getID())
    {
        _finger->setNode(i, s);
        if (getPredecessor()) getPredecessor()->updateFingerTable(s, i);
    }
}

void LocalNode::stabilize()
{
    INode* x = getSuccessor()->getPredecessor();
    if (x != 0 && x->getID().isInInterval(getID(), getSuccessor()->getID()))
    {
        setSuccessor(x);
    }
    getSuccessor()->notify(this);
}

void LocalNode::notify(INode* n)
{
    if (getPredecessor() == 0 || n->getID().isInInterval(getPredecessor()->getID(), getID()))
    {
        setPredecessor(n);
    }
}

void LocalNode::fixFingers()
{
    unsigned int i = (unsigned int)rand() % (M - 1) + 1;
    ID sID = _finger->start(i);
    _finger->setNode(i, findSuccessor(sID));
}

INode* LocalNode::getPredecessor()
{
    return _predecessor;
}

void LocalNode::setPredecessor(INode* n)
{
    _predecessor = n;
}

INode* LocalNode::getSuccessor()
{
    return _finger->node(0);
}

void LocalNode::setSuccessor(INode* n)
{
    _finger->setNode(0, n);
}

std::string LocalNode::serialize()
{
    char portCharArr[4];
    portCharArr[0] = (char)_port;
    portCharArr[1] = (char)(_port >> 8);
    portCharArr[2] = (char)(_port >> 16);
    portCharArr[3] = (char)(_port >> 24);
    return getIP() + std::string(portCharArr, 4);
}

void LocalNode::periodic()
{
    // construct a timer
    clock_t timer;
    clock_t timerloop;
    timer = clock();
    for(;;)
    {
        // check if the thread needs to exit
        if(_stop) break;

        // check the timer
        timerloop = clock();
        if((timerloop - timer) / (double)CLOCKS_PER_SEC > TIME_PERIOD)
        {
            //std::cout << (timer / (double)CLOCKS_PER_SEC) << " period hit!\n";
            stabilize();
            fixFingers();

            // check the server process, try to restart it if necessary
            if(!_serverRunning)
            {
                if (_serverThread != 0) delete _serverThread;
                _serverRunning = true;
                _serverThread = new std::thread(&LocalNode::server, this);
            }

            // reset the timer
            timer = clock();
        }
    }
}

void LocalNode::server()
{
    _serverRunning = true;
    std::vector<std::thread> threads();
    try
    {
        asio::io_service io_service;
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), getPort());
        asio::ip::tcp::acceptor acceptor(io_service, endpoint);
        asio::ip::tcp::socket socket(io_service);

        std::cout << "Server ready" << std::endl;
        for(;;)
        {
            acceptor.accept(socket);


            char data[MAX_DATA_LENGTH];

            asio::error_code error;
            size_t length = socket.read_some(asio::buffer(data), error);
            if (error == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error); // Some other error.

            // handle the message in a new thread
            //threads.push_back(std::thread(&LocalNode::handleRequest, this));
            std::string message(data, length);
            //std::thread t(&LocalNode::handleRequest, this, socket, message);
            //std::thread t(std::bind(&LocalNode::handleRequest, this, socket, message));
            //t.detach();
            handleRequest(socket, message);

            //std::string message("Hello from server\n");
            //asio::write(socket, asio::buffer(message));
            //socket.close();
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    _serverRunning = false;
}

void LocalNode::handleRequest(asio::ip::tcp::socket& socket, std::string message)
{
    std::string response = "";

    RPCCode messageCode = (RPCCode)message[0];
    switch(messageCode)
    {
        case RPCCode::FIND_PREDECESSOR:

            break;
        case RPCCode::FIND_SUCCESSOR:

            break;
        case RPCCode::CLOSEST_PRECEDING_FINGER:
        {
            if (message.length() < 21)
            {
                //error - message is too small
            }

            INode* n = closestPrecedingFinger((byte*)(message.c_str() + 1));
            response = n->serialize();
        }
        break;
        case RPCCode::JOIN:

            break;
        case RPCCode::INIT_FINGER_TABLE:

            break;
        case RPCCode::UPDATE_OTHERS:

            break;
        case RPCCode::UPDATE_FINGER_TABLE:

            break;
        case RPCCode::STABILIZE:

            break;
        case RPCCode::NOTIFY:

            break;
        case RPCCode::FIX_FINGER:

            break;
        case RPCCode::GET_PREDECESSOR:

            break;
        case RPCCode::SET_PREDECESSOR:

            break;
        case RPCCode::GET_SUCCESSOR:

            break;
        case RPCCode::SET_SUCCESSOR:

            break;
        case RPCCode::GET_ID:

            break;
        default:
            std::cerr << "Error: Invalid RPC code received - " << (int)messageCode << std::endl;
    }

    // respond if necessary and close the socket
    if (response != "")
        asio::write(socket, asio::buffer(response));
    socket.close();
}
