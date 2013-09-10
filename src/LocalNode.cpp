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
    if (_t != 0)
    {
        _stop = true;
        _t->join();
        delete _t;
        _t = 0;
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

    // start the thread
    _t = new std::thread(periodic, this);
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

void LocalNode::periodic(LocalNode* n)
{
    // construct a timer
    clock_t timer;
    clock_t timerloop;
    timer = clock();
    while(1)
    {
        // check if the thread needs to exit
        if (n->getStop()) break;

        // check the timer
        timerloop = clock();
        if ((timerloop - timer) / (double)CLOCKS_PER_SEC > TIME_PERIOD)
        {
            //std::cout << (timer / (double)CLOCKS_PER_SEC) << " period hit!\n";
            n->stabilize();
            n->fixFingers();

            // reset the timer
            timer = clock();
        }
    }
}
