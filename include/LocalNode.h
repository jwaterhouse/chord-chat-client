#ifndef LOCALNODE_H
#define LOCALNODE_H

//#include <boost/asio/ip/address.hpp>
#include "../include/INode.h"
#include "../include/FingerTable.h"

class LocalNode : public INode
{
    public:
        LocalNode(const std::string&, unsigned int);
        LocalNode(const std::string&, unsigned int, INode*);
        LocalNode(const INode&);
        virtual ~LocalNode();

        virtual INode* findPredecessor(const ID&);
        virtual INode* findSuccessor(const ID&);
        virtual INode* closestPrecedingFinger(const ID&);
        virtual void join(INode*);
        virtual void initFingerTable(INode*);
        virtual void updateOthers();
        virtual void updateFingerTable(INode*, unsigned int);
        virtual void stabilize();
        virtual void notify(INode*);
        virtual void fixFingers();

        // Getters and setters;
        virtual INode* getPredecessor();
        virtual void setPredecessor(INode* n);
        virtual INode* getSuccessor();
        virtual void setSuccessor(INode* n);

        virtual INode* clone() const { return new LocalNode(*this); }

    private:
        void init();
        void checkPredecessor();

    protected:
        INode* _predecessor = 0;
        FingerTable* _finger = 0;

        //boost::thread t;
};

#endif // LOCALNODE_H
