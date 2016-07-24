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
        ID(const char * id);

        /**< Copy constructor */
        ID(const ID & other);

        /**< Construct an ID from a host/IP and port, which are hashed */
        ID(const std::string & host, unsigned int port);

        /**< Construct an ID from a string, which is hashed */
        ID(const std::string & str);

        /**< Destructor */
        virtual ~ID();

        /** \brief Method to check if this ID is within the bounds of the
         *         the two given IDs.
         *
         * \param ID The lower limit comparator
         * \param ID The upper limit comparator
         * \return bool True if in interval, false if not
         */
        bool isInInterval(const ID & lower, const ID & upper) const;

        /**< Return this ID as a char array */
        const char * c_str() const;

        /**< Return this ID as a string of hexadecimal characters */
        std::string toHexString() const;

        /**< Array access operators */
        char & operator[](int index);
        const char & operator[](int index) const;

        /**< Arithmetic operators */
        ID & operator+=(const ID & other);
        const ID operator+(const ID & other) const;
        ID & operator-=(const ID & other);
        const ID operator-(const ID & other) const;

        /**< Comparison operators */
        friend bool operator==(const ID & lhs, const ID & rhs);
        friend bool operator!=(const ID & lhs, const ID & rhs);
        friend bool operator<(const ID & lhs, const ID & rhs);
        friend bool operator>(const ID & lhs, const ID & rhs);
        friend bool operator<=(const ID & lhs, const ID & rhs);
        friend bool operator>=(const ID & lhs, const ID & rhs);

    protected:
    private:
        /**< Char array representing this ID */
        char _id[ID_LEN] = {'\0'};
};

#endif // ID_H
