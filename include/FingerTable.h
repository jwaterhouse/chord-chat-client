#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "../include/INode.h"

class FingerTable
{
    public:
        /**< Constructs a new FingerTable */
        FingerTable() { }

        /** \brief Constructs a new FingerTable with the given ID
         *
         * \param IF The ID for this FingerTable
         */
        FingerTable(const ID& id) : _id(id) { }

        /**< Destructor */
        virtual ~FingerTable() { }

        /** \brief Gets the nearest successor for the given index (start(index))
         *
         * \param unsigned int The index to retrieve
         * \return Node
         *
         */
        Node getNode(unsigned int);

        /** \brief Get the exact node at the given index
         *
         * \param unsigned int Index to retrieve node
         * \return Node
         */
        Node getNodeAt(unsigned int);

        /** \brief Sets the Node in the finger table for the given index
         *
         * \param unsigned int The index to set
         * \param Node The Node to set
         */
        void setNode(unsigned int, Node);

        /** \brief Computes the ID for the given index ((n + 2**i) % 2**M)
         *
         * \param unsigned int
         * \return ID The computed ID
         */
        ID start(unsigned int) const;

        /** \brief Sets the ID
         *
         * \param ID The ID to set
         */
        void setID(const ID&);

        /** \brief Removes all references to the given Node from the finger table
         *
         * \param Node The Node to remove
         */
        void removeNode(Node);

    protected:
    private:
        /**< The entries table to store Nodes */
        Node _entries[M] = {NULL};

        /**< The ID for this finger table */
        ID _id;

        /**< Debug method to print the finger table to the console */
        void printFingerTable() const;

        /**< Returns a short Hex string represenation of the given ID */
        std::string getIDShort(const ID& id) const;
};

#endif // FINGERTABLE_H
