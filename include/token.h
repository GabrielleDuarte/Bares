#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>   // std::string
#include <iostream> // std::ostream

/// Represents a token.
/*!
 * This struct represents a token, which is just a pair of information
 * `value/type` that identifies the content and its type.
 * One or more tokens are extracted from an input string in the BARES project.
 * A BARES expression is composed of one or more tokens.
 */
struct Token
{
    public:
        enum class token_t : int
        {
            OPERAND = 0, //!< A type representing numbers.
            OPERATOR     //!< A type representing  "+", "-".
        };

        std::string value; //!< The token value as a string.
        token_t type;      //!< The token type, which is either token_t::OPERAND or token_t::OPERATOR.

        /// Construtor default.
        explicit Token( std::string value_="", token_t type_ = token_t::OPERAND )
            : value( value_ )
            , type( type_ )
        {/* empty */}

        /// Just to help us debug the code.
        friend std::ostream & operator<<( std::ostream& os_, const Token & t_ )
        {
            std::string types[] = { "OPERAND", "OPERATOR" };

            os_ << "<" << t_.value << "," << types[(int)(t_.type)] << ">";

            return os_;
        }
};

#endif
