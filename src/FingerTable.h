#ifndef FINGERTABLE_H
#define FINGERTABLE_H

#include "INode.h"

class FingerTable
{
    public:
        /**< Constructs a new FingerTable */
        FingerTable() { }

        /** \brief Constructs a new FingerTable with the given ID
         *
         * \param IF The ID for this FingerTable
         */
        FingerTable(const ID & id) : _id(id) { }

        /**< Destructor */
        virtual ~FingerTable() { }

        /** \brief Gets the nearest successor for the given index (start(index))
         *
         * \param unsigned int The index to retrieve
         * \return Node
         *
         */
        Node getNode(unsigned int index);

        /** \brief Get the exact node at the given index
         *
         * \param unsigned int Index to retrieve node
         * \return Node
         */
        Node getNodeAt(unsigned int index);

        /** \brief Sets the Node in the finger table for the given index
         *
         * \param unsigned int The index to set
         * \param Node The Node to set
         */
        void setNode(unsigned int index, Node n);

        /** \brief Computes the ID for the given index ((n + 2**i) % 2**M)
         *
         * \param unsigned int
         * \return ID The computed ID
         */
        ID start(unsigned int k) const;

        /** \brief Sets the ID
         *
         * \param ID The ID to set
         */
        void setId(const ID & id);

        /** \brief Removes all references to the given Node from the finger table
         *
         * \param Node The Node to remove
         */
        void removeNode(Node n);

        /**< Formath the current finger table state as a string */
        std::string toString() const;

    protected:
    private:
        /**< The entries table to store Nodes */
        Node _entries[M] = {NULL};

        /**< The ID for this finger table */
        ID _id;

        /**< Returns a short Hex string represenation of the given ID */
        std::string getIdShort(const ID & id) const;
};

#endif // FINGERTABLE_H
