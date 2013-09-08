#include "../include/LocalNode.h"
#include <cstdlib>

INode* LocalNode::findPredecessor(ID* id)
{
    INode* n = this;
    while(!(getID()->isInInterval(n->getID(), n->getSuccessor()->getID())
            || getID() == n->getSuccessor()->getID()))
    {
        n = n->closestPrecedingFinger(id);
    }
    return n;
}

INode* LocalNode::findSuccessor(ID* id)
{
    INode* n = this->findPredecessor(id);
    return n->getSuccessor();
}

INode* LocalNode::closestPrecedingFinger(ID* id)
{
    for(int i = M - 1; i >= 0; --i)
    {
        INode* n = this->finger.node(i);
        if (finger.node(i)->getID()->isInInterval(n->getID(), getID()))
            return n;
    }
    return this;
}

void LocalNode::join(INode* n)
{
    if (n != 0)
    {
        this->initFingerTable(n);
        this->updateOthers();
    }
    else
    {
        for(unsigned int i = 0; i < M; ++i)
        {
            this->finger.setNode(i, n);
        }
        this->setPredecessor(n);
    }
}

void LocalNode::initFingerTable(INode* n)
{
    ID* start = finger.start(0);
    finger.setNode(0, n->findSuccessor(finger.start(0)));
    setPredecessor(getSuccessor()->getPredecessor());
    getSuccessor()->setPredecessor(this);

    for(int i = 0; i < M; ++i)
    {
        ID* si = finger.start(i + 1);
        if (si->isInInterval(getID(), finger.node(i)->getID())
            || si == getID())
        {
            finger.setNode(i + 1, finger.node(i));
        }
        else
        {
            finger.setNode(i + 1, n->findSuccessor(si));
        }

        delete si;
    }
    delete start;
}

void LocalNode::updateOthers()
{
    for(int i = 0; i < M; ++i)
    {
        byte b[ID_LEN];
        b[ID_LEN - i / 8 - 1] = (0x01 << (i % 8));

        ID bID(b);
        ID nId = *id - bID;

        INode* p = findPredecessor(&nId);
        p->updateFingerTable(this, i);
    }
}

void LocalNode::updateFingerTable(INode* s, unsigned int i)
{
    if (s->getID()->isInInterval(getID(), finger.node(i)->getID())
        || s->getID() == getID())
    {
        finger.setNode(i, s);
        getPredecessor()->updateFingerTable(s, i);
    }
}
void LocalNode::stabilize()
{
    INode* x = getSuccessor()->getPredecessor();
    if (x->getID()->isInInterval(getID(), getSuccessor()->getID()))
    {
        setSuccessor(x);
    }
    getSuccessor()->notify(x);
}

void LocalNode::notify(INode* n)
{
    if (getPredecessor() == 0)
        setPredecessor(n);
    else
    {
        if (n->getID()->isInInterval(getPredecessor()->getID(), getID()))
        {
            setPredecessor(n);
        }
    }
}

void LocalNode::fixFingers()
{
    unsigned int i = (unsigned int)rand() % (M - 1) + 1;
    ID* sID = finger.start(i);
    finger.setNode(i, findSuccessor(sID));
    delete sID;
}

INode* LocalNode::getPredecessor()
{
    return this->predecessor;
}

void LocalNode::setPredecessor(INode* n)
{
    this->predecessor = n;
}

INode* LocalNode::getSuccessor()
{
    return this->finger.node(0);
}

void LocalNode::setSuccessor(INode* n)
{
    this->finger.setNode(0, n);
}
