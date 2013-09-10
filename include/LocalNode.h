#ifndef LOCALNODE_H
#define LOCALNODE_H

#include <iostream>
#include <thread>
#include <mutex>
#include <asio.hpp>
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
        bool getStop() { return _stop; }

        virtual INode* clone() const { return new LocalNode(*this); }

    private:
        void init();
        void checkPredecessor();
        static void listener(LocalNode*);

    protected:
        INode* _predecessor = 0;
        FingerTable* _finger = 0;
        std::thread* _t = 0;
        std::mutex* _m = 0;
        bool _stop = false;
};

#endif // LOCALNODE_H
