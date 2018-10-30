#ifndef _PARSSER_H_
#define _PARSSER_H_

#include <vector>
#include "token.h"

class Parser
{
    public:
        /// This struct represents the result of the parsing operation.
        struct ResultType
        {
            //=== Alias
            typedef std::ptrdiff_t size_type; //!< Used for column location determination.
            /// List of possible syntax errors.
            enum code_t {
                    OK = 0, //!< Expression successfuly parsed.
                    UNEXPECTED_END_OF_EXPRESSION,
                    ILL_FORMED_INTEGER,
                    MISSING_TERM,
                    EXTRANEOUS_SYMBOL,
                    INTEGER_OUT_OF_RANGE
            };
            //=== Members (public).
            code_t type;      //!< Error code.
            size_type at_col; //!< Stores the column number where the error happened.
            /// Default contructor.
            explicit ResultType( code_t type_=OK , size_type col_=0u )
                    : type{ type_ }
                    , at_col{ col_ }
            { /* empty */ }
        };
         std::vector< Token > get()
        {
        	return m_tk_list;
        }
        //==== Aliases
        typedef short int required_int_type; //!< The interger type we accept as valid for an expression.
        typedef long long int input_int_type; //!< The integer type that we read from the input, which should be larger than  he required integer range (so we can identify input errors).
        //==== Public interface
        /// Parses and tokenizes an input source expression.  Return the result as a struct.
        ResultType parse( std::string e_ );
        /// Retrieves the list of tokens created during the partins process.
        std::vector< Token > get_tokens( void ) const;
        //==== Special methods
        /// Default constructor
        Parser() = default;
        /// Default destructor
        ~Parser() = default;
        /// Turn off copy constructor. We do not need it.
        Parser( const Parser & ) = delete;  // Construtor cópia.
        /// Turn off assignment operator.
        Parser & operator=( const Parser & ) = delete; // Atribuição.
    private:
        /// Terminal symbols table
        enum class terminal_symbol_t{  // The symbols:-
           TS_PLUS,	        //!< code for "+"
            TS_MINUS,	        //!< code for "-"
            TS_ZERO,            //!< code for "0"
            TS_MULT,			// multiplication
            TS_DIV,				// divisions
            TS_MODE,		    // % 
            TS_EXPO,			// exponencial '^'
            TS_NON_ZERO_DIGIT,  //!< code for digits, from "1" to "9"
            TS_WS,              //!< code for a white-space
            TS_TAB,             //!< code for tab
            TS_EOS,             //!< code for "End Of String" '\n'
            TS_INVALID,	        //!< invalid token
            TS_OPEN_P,		    //'('
            TS_OPEN_C,			//')'
        };
        //==== Private members.

        std::string m_expr;                //!< The source expression to be parsed
        std::string::iterator m_it_curr_symb; //!< Pointer to the current char inside the expression.
        std::vector< Token > m_tk_list; //!< Resulting list of tokens extracted from the expression.
        ResultType m_result; //!< The result for the current expression (either error of OK).
        //=== Support methods.
        terminal_symbol_t lexer( char c_ ) const;// Get the corresponding code for a given input char.
        void next_symbol( void );                // Advances iterator to the next char in the expression.
        //bool peek( terminal_symbol_t c_ ) const; // Peeks the current character.
        bool accept( terminal_symbol_t c_ );     // Tries to accept the requested symbol.
        //bool expect( terminal_symbol_t c_ );        // Skips any WS/Tab and tries to accept the requested symbol.
        void skip_ws( void );                    // Skips any WS/Tab ans stops at the next character.
        bool end_input( void ) const;            // Checks whether we reached the end of the expression string.
        //=== NTS methods.
        bool expression();
        bool term();
        bool integer();
        bool natural_number();
        bool digit_excl_zero();
        bool digit();
};
#endif