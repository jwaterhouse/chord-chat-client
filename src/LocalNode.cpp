#include "../include/LocalNode.h"
#include <cstdlib>
#include <ctime>
#include "../include/RemoteNode.h"

LocalNode::LocalNode(const std::string& name, const std::string& host, unsigned int port) : INode::INode(name, host, port)
{
    init();
}

LocalNode::LocalNode(const INode& n) : INode::INode(n.getName(), n.getHost(), n.getPort())
{
    init();
}

LocalNode::~LocalNode()
{
    /*
    if (_predecessor != 0 && _predecessor != this)
    {
        delete _predecessor;
        _predecessor = 0;
    }
    */
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
    if (_socketM != 0)
    {
        delete _socketM;
        _socketM = 0;
    }
}

void LocalNode::init()
{
    _finger = new FingerTable(*this);
    setPredecessor(0);
    //setSuccessor(thisPtr());

    // initialise the mutexes
    _m = new std::mutex();
    _socketM = new std::mutex();

    // start the threads
    _periodicThread = new std::thread(&LocalNode::periodic, this);
    _serverRunning = true;
    _serverThread = new std::thread(&LocalNode::server, this);
}

Node LocalNode::findPredecessor(const ID& id)
{
    Node n = thisPtr();
    while (!(id.isInInterval(n->getID(), n->getSuccessor()->getID())
            || id == n->getSuccessor()->getID()))
    {
        n = n->closestPrecedingFinger(id);
    }
    return n;
}

Node LocalNode::findSuccessor(const ID& id)
{
    //Node n = findPredecessor(id);
    //return n->getSuccessor();

    Node s = getSuccessor();
    if(id.isInInterval(getID(), s->getID())
        || id == s->getID()
        || getID() == s->getID())
    {
        return s;
    }
    else
    {
        Node n = closestPrecedingFinger(id);
        return n->findSuccessor(id);
    }
}

Node LocalNode::closestPrecedingFinger(const ID& id)
{
    for (int i = M - 1; i >= 0; --i)
    {
        if (_finger->node(i)->getID().isInInterval(getID(), id))
            return _finger->node(i);
    }
    return thisPtr();
}

void LocalNode::join(Node n)
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
    setSuccessor(n->findSuccessor(getID()));
}

void LocalNode::initFingerTable(Node n)
{
    ID start = _finger->start(0);
    _finger->setNode(0, n->findSuccessor(_finger->start(0)));
    setPredecessor(getSuccessor()->getPredecessor());
    getSuccessor()->setPredecessor(thisPtr());

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
        char b[ID_LEN];
        b[ID_LEN - i / 8 - 1] = (0x01 << (i % 8));

        ID bID(b);
        ID nId = *_id - bID;

        Node p = findPredecessor(nId);
        p->updateFingerTable(thisPtr(), i);
    }
}

void LocalNode::updateFingerTable(Node s, unsigned int i)
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
    Node x = getSuccessor()->getPredecessor();
    if(x != 0 && x->getID().isInInterval(getID(), getSuccessor()->getID()))
    {
        setSuccessor(x);
    }
    getSuccessor()->notify(thisPtr());
}

void LocalNode::notify(Node n)
{
    if(getPredecessor() == 0
        || n->getID().isInInterval(getPredecessor()->getID(), getID())
        || getPredecessor()->getID() == getID())
    {
        setPredecessor(n);
    }
}

void LocalNode::fixFingers()
{
    //unsigned int i = (unsigned int)rand() % (M - 1) + 1;
    _next++;
    if (_next >= M) _next = 0;
    unsigned int i = _next;
    ID sID = _finger->start(i);
    _finger->setNode(i, findSuccessor(sID));
}

Node LocalNode::getPredecessor()
{
    return _predecessor;
}

void LocalNode::setPredecessor(Node n)
{
    _predecessor = n;
}

Node LocalNode::getSuccessor()
{
    if (_finger->node(0) == 0) _finger->setNode(0, thisPtr());
    return _finger->node(0);
}

void LocalNode::setSuccessor(Node n)
{
    _finger->setNode(0, n);
}

void LocalNode::receive(std::string message)
{
    _rcvFn(message);
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
        if(_stop) return;

        // check the timer
        timerloop = clock();
        if((timerloop - timer) / (double)CLOCKS_PER_SEC > TIME_PERIOD)
        {
            //std::cout << getName() << ": periodic" << std::endl;
            stabilize();
            fixFingers();

            // check the server process, try to restart it if necessary
            if(!_serverRunning)
            {
                if (_serverThread != 0)
                {
                    _serverThread->join();
                    delete _serverThread;
                }
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

        //std::cout << getName() << ": Server ready" << std::endl;
        for(;;)
        {
            // check if the thread needs to exit
            if(_stop) return;

            //std::cout << getName() << ": listen" << std::endl;
            acceptor.listen();
            //std::cout << getName() << ": non_blocking" << std::endl;
            //acceptor.non_blocking(true);
            //std::cout << getName() << ": accept" << std::endl;
            acceptor.accept(socket);

            //std::cout << getName() << ": accepted" << std::endl;
            char data[MAX_DATA_LENGTH];

            asio::error_code error;
            char header = '\0';
            size_t length = asio::read(socket, asio::buffer(&header, 1), error);
            int payloadLength = (int)header;

            length = asio::read(socket, asio::buffer(data, payloadLength), error);

            //size_t length = socket.read_some(asio::buffer(data), error);
            if (error == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error); // Some other error.

            // handle the message in a new thread
            //threads.push_back(std::thread(&LocalNode::handleRequest, this));
            //std::string message(data, length);
            //std::thread t(&LocalNode::handleRequest, this, socket, message);
            //std::thread t(std::bind(&LocalNode::handleRequest, this, socket, message));
            //t.detach();
            std::string response = handleRequest(data, length);

            asio::write(socket, asio::buffer(response));
            socket.close();
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception in " << getName() << ": " << e.what() << std::endl;
    }
    _serverRunning = false;
}

std::string LocalNode::handleRequest(const char* message, size_t length)
{
    std::string response = "";

    RPCCode messageCode = (RPCCode)message[0];
    int offset = 1;
    switch(messageCode)
    {
        case RPCCode::FIND_PREDECESSOR:
        {
            if (length < 21)
            {
                //error - message is too small
            }
            ID id((char*)(message + offset));
            Node n = findPredecessor(id);
            response = n->serialize();
        }
        break;
        case RPCCode::FIND_SUCCESSOR:
        {
            if (length < 21)
            {
                //error - message is too small
            }
            ID id((char*)(message + offset));
            Node n = findSuccessor(id);
            response = n->serialize();
        }
        break;
        case RPCCode::CLOSEST_PRECEDING_FINGER:
        {
            if (length < 21)
            {
                //error - message is too small
            }
            ID id((char*)(message + offset));
            Node n = closestPrecedingFinger(id);
            response = n->serialize();
        }
        break;
        case RPCCode::JOIN:
        {
            std::cerr << "Not yet implemented." << std::endl;
        }
        break;
        case RPCCode::INIT_FINGER_TABLE:
        {
            std::cerr << "Not yet implemented." << std::endl;
        }
        break;
        case RPCCode::UPDATE_OTHERS:
        {
            std::cerr << "Not yet implemented." << std::endl;
        }
        break;
        case RPCCode::UPDATE_FINGER_TABLE:
        {
            std::cerr << "Not yet implemented." << std::endl;
        }
        break;
        case RPCCode::STABILIZE:
        {
            std::cerr << "Not yet implemented." << std::endl;
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
            std::cerr << "Not yet implemented." << std::endl;
        }
        break;
        case RPCCode::GET_PREDECESSOR:
        {
            response = getPredecessor()->serialize();
        }
        break;
        case RPCCode::SET_PREDECESSOR:
        {
            Node n(new RemoteNode(message + offset, length - offset));
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
            Node n(new RemoteNode(message + offset, length - offset));
            setSuccessor(n);
        }
        break;
        case RPCCode::GET_ID:
        {
            response = std::string(getID().c_str(), ID_LEN);
        }
        break;
        case RPCCode::RECEIVE:
        {
            std::string receivedMessage(message + offset, length - offset);
            _rcvFn(receivedMessage);
        }
        break;
        default:
            std::cerr << "Error: Invalid RPC code received - " << (int)messageCode << std::endl;
    }

    // respond if necessary and close the socket
    //if (response != "")
    //   asio::write(socket.get(), asio::buffer(response));
    //socket.get().close();
    char responseLength = (int)(response.length());
    return std::string(&responseLength, 1) + std::string(response);
}
