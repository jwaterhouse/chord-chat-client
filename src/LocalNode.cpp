#include <cstdlib>
#include <ctime>
#include "../include/LocalNode.h"
#include "../include/RemoteNode.h"

LocalNode::LocalNode(const std::string& name, const std::string& host, unsigned int port) : INode::INode(name, host, port)
{
    init();
}

LocalNode::LocalNode(const Node& n) : INode::INode(n)
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

void LocalNode::init()
{
    _finger.setID(_id);
    setPredecessor(NULL);
    //setSuccessor(thisPtr());  //cannot set as part of constructor

    // start the threads
    _serverRunning = true;
    _periodicThread = new std::thread(&LocalNode::periodic, this);
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
    if(getID() == id) return thisPtr();
    Node s = getSuccessor();
    if(id.isInInterval(getID(), s->getID())
        || id == s->getID())
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
        Node f = _finger.getNode(i);;
        if (f != NULL && f->getID().isInInterval(getID(), id))
            return f;
    }
    return thisPtr();
}

void LocalNode::join(Node n)
{
    setPredecessor(NULL);
    setSuccessor(n->findSuccessor(getID()));
}

void LocalNode::stabilize()
{
    // check the successor first?
    Node s = getSuccessor();
    while (s->ping() == false)
    {
        // fix the finger table
        _finger.removeNode(s);
        s = getSuccessor();
    }

    Node x = getSuccessor()->getPredecessor();
    if (x != NULL &&
        (x->getID().isInInterval(getID(), getSuccessor()->getID())))
    {
        setSuccessor(x);
    }
    getSuccessor()->notify(thisPtr());
}

void LocalNode::notify(Node n)
{
    if (getPredecessor() == NULL
        || n->getID().isInInterval(getPredecessor()->getID(), getID()))
    {
        setPredecessor(n);
    }
}

void LocalNode::fixFingers()
{
    unsigned int i = (unsigned int)rand() % (M - 2) + 1;
    ID sID = _finger.start(i);
    Node n = findSuccessor(sID);
    if(n != NULL && n->getID() == getID())
        n = thisPtr();
    _finger.setNode(i, findSuccessor(sID));
}

void LocalNode::checkPredecessor()
{
    Node n = getPredecessor();
    if(n != NULL && n != thisPtr() && n->ping() == false)
        setPredecessor(NULL);
}

bool LocalNode::ping()
{
    return true;
}

Node LocalNode::getPredecessor()
{
    std::lock_guard<std::mutex> lock(_m);
    return _predecessor;
}

void LocalNode::setPredecessor(Node n)
{
    std::lock_guard<std::mutex> lock(_m);
    if (n != NULL && n->getID() == getID()) n = thisPtr();
    _predecessor = n;
}

Node LocalNode::getSuccessor()
{
    std::lock_guard<std::mutex> lock(_m);
    if (_finger.getNode(0) == NULL) _finger.setNode(0, thisPtr());
    return _finger.getNode(0);
}

void LocalNode::setSuccessor(Node n)
{
    std::lock_guard<std::mutex> lock(_m);
    if (n != NULL && n->getID() == getID()) n = thisPtr();
    _finger.setNode(0, n);
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
    for (;;)
    {
        // check if the thread needs to exit
        if (_stop) return;

        // check the timer
        timerloop = clock();
        if ((timerloop - timer) / (double)CLOCKS_PER_SEC > TIME_PERIOD)
        {
            //std::cout << getName() << ": periodic" << std::endl;
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
        for (;;)
        {
            // check if the thread needs to exit
            if (_stop) return;

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
        std::cerr << "Exception in " << getName() << "::server() - " << e.what() << std::endl;
    }
    _serverRunning = false;
}

void LocalNode::handleRequest(asio::ip::tcp::socket& socket)
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
            return; // Connection closed cleanly by peer.
        else if (error)
            throw asio::system_error(error); // Some other error.

        // retrieve the rest of the message
        length = asio::read(socket, asio::buffer(message, messageLength), error);
        if (error == asio::error::eof)
            return; // Connection closed cleanly by peer.
        else if (error)
            throw asio::system_error(error); // Some other error.

        // first byte of the message is the RPC code
        RPCCode code = (RPCCode)message[0];
        int offset = 1;
        switch (code)
        {
            case RPCCode::FIND_PREDECESSOR:
            {
                if (length != ID_LEN + 1)
                {
                    std::cerr << "Error: Message length incorrect." << std::endl;
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
                    std::cerr << "Error: Message length incorrect." << std::endl;
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
                    std::cerr << "Error: Message length incorrect." << std::endl;
                }
                ID id((char*)(message + offset));
                Node n = closestPrecedingFinger(id);
                response = n->serialize();
            }
            break;
            case RPCCode::JOIN:
            {
                std::cerr << "Error: join() not yet implemented." << std::endl;
            }
            break;
            case RPCCode::STABILIZE:
            {
                std::cerr << "Error: stabilize() not yet implemented." << std::endl;
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
                std::cerr << "Error: fixFinger() not yet implemented." << std::endl;
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
                if ((length - offset) == 0) n = NULL;
                else n = Node(new RemoteNode(message + offset, length - offset));
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
                if ((length - offset) == 0) n = NULL;
                else n = Node(new RemoteNode(message + offset, length - offset));
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
            case RPCCode::PING:
            {
            }
            break;
            default:
                std::cerr << "Error: Invalid RPC code received - " << (int)messageCode << std::endl;
        }

        char responseLength = (int)(response.length());
        std::string responseMessage = responseLength + response;
        asio::write(socket, asio::buffer(responseMessage));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in " << getName() << "::handleRequest() - " << e.what() << std::endl;
    }
    socket.close();
}
