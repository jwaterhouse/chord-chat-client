#ifndef ID_H
#define ID_H

#include <string>
#include <sstream>

#include "sha1.h"

#define ID_LEN 20 /**< Number of bytes in an ID string. */
#define M 160 /**< Number of bits in an ID. Depends on consistent hash algorithm. */

class ID
{
    public:
        /**< Default constructor */
        ID() { };

        /**< Construct an ID from a char array representing an ID */
        ID(const char*);

        /**< Copy constructor */
        ID(const ID&);

        /**< Construct an ID from a host/IP and port, which are hashed */
        ID(const std::string&, unsigned int);

        /**< Construct an ID from a string, which is hashed */
        ID(const std::string&);

        /**< Destructor */
        virtual ~ID();

        /** \brief Method to check if this ID is within the bounds of the
         *         the two given IDs.
         *
         * \param ID The lower limit comparator
         * \param ID The upper limit comparator
         * \return bool True if in interval, false if not
         */
        bool isInInterval(const ID&, const ID&) const;

        /**< Return this ID as a char array */
        const char* c_str() const;

        /**< Array access operators */
        char& operator[](int);
        const char& operator[](int) const;

        /**< Arithmetic operators */
        ID& operator+=(const ID&);
        const ID operator+(const ID&) const;
        ID& operator-=(const ID&);
        const ID operator-(const ID&) const;

        /**< Comparison operators */
        friend bool operator==(const ID&, const ID&);
        friend bool operator!=(const ID&, const ID&);
        friend bool operator<(const ID&, const ID&);
        friend bool operator>(const ID&, const ID&);
        friend bool operator<=(const ID&, const ID&);
        friend bool operator>=(const ID&, const ID&);

    protected:
    private:
        /**< Char array representing this ID */
        char _id[ID_LEN] = {0x00};
};

#endif // ID_H
