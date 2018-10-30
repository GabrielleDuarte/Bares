#include "parsser.h"

#include <iterator>
#include <algorithm>
/// Converts the input character c_ into its corresponding terminal symbol code

Parser::terminal_symbol_t  Parser::lexer( char c_ ) const
{
    switch( c_ )
    {
        case '+':  return terminal_symbol_t::TS_PLUS;
        case '-':  return terminal_symbol_t::TS_MINUS;
        case ' ':  return terminal_symbol_t::TS_WS;
        case   9:  return terminal_symbol_t::TS_TAB;
        case '*':  return terminal_symbol_t::TS_MULT;           
        case '/':  return terminal_symbol_t::TS_DIV;             
        case '%':  return terminal_symbol_t::TS_MODE;            
        case '^':  return terminal_symbol_t::TS_EXPO;
        case '0':  return terminal_symbol_t::TS_ZERO;
        case '(':  return terminal_symbol_t::TS_OPEN_P;          
        case ')':  return terminal_symbol_t::TS_OPEN_C;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':  return terminal_symbol_t::TS_NON_ZERO_DIGIT;
        case '\0': return terminal_symbol_t::TS_EOS; // end of string: the $ terminal symbol
    }
    return terminal_symbol_t::TS_INVALID;
}


/// Consumes a valid character from the input source expression.
void Parser::next_symbol( void )
{
    // Advances iterator to the next valid symbol for processing
    std::advance( m_it_curr_symb, 1 );
}
/// Checks whether we reached the end of the input expression string.
bool Parser::end_input( void ) const
{
    // "Fim de entrada" ocorre quando o iterador chega ao
    // fim da string que guarda a expressão.
    return m_it_curr_symb == m_expr.end();
}
#ifdef PEEK
// Returns the result of trying to match the current character with c_, **without** consuming the current character from the input expression.
bool Parser::peek( terminal_symbol_t c_ ) const
{
    // Checks whether the input symbol is equal to the argument symbol.
    return ( not end_input() and
             lexer( *m_it_curr_symb ) == c_ );
}
#endif
/// Returns the result of trying to match and consume the current character with c_.
/*!
 * If the match is not successful, the current character (from the input) **is not consumed**.
 * @param c_ the current character from the input source expression we wish to parse.
 * @see peek().
 * @return true if we got a successful match; false otherwise.
 */
bool Parser::accept( terminal_symbol_t c_ )
{
    // If we have a match, we consume the character from the input source expression.
    // caractere da entrada.
    if ( not end_input() and
             lexer( *m_it_curr_symb ) == c_  )
    {
        next_symbol();
        return true;
    }
    return false;
}
#ifdef EXPECT
// Skips all white spaces and tries to accept() the next valid character. @see accept().
bool Parser::expect( terminal_symbol_t c_ )
{
    // Skip all white spaces first.
    skip_ws();
    return accept( c_ );
}
#endif
/// Ignores any white space or tabs in the expression until reach a valid character or end of input.
void Parser::skip_ws( void )
{
    // Skip white spaces, while at the same time, check for end of string.
    while ( not end_input() and std::isspace( *m_it_curr_symb ) )
            //( lexer( *m_it_curr_symb ) == Parser::terminal_symbol_t::TS_WS  or
            //  lexer( *m_it_curr_symb ) == Parser::terminal_symbol_t::TS_TAB ) )
    {
        next_symbol();
    }
}
//=== Non Terminal Symbols (NTS) methods.
/// Validates (i.e. returns true or false) and consumes an **expression** from the input expression string.
/*! This method parses a valid expression from the input and, at the same time, it tokenizes its components.
 *
 * Production rule is:
 * ```
 *  <expr> := <term>,{ ("+"|"-"),<term> };
 * ```
 * An expression might be just a term or one or more terms with '+'/'-' between them.
 */
bool Parser::expression()
{
    term();
    // Process terms
    while( m_result.type == ResultType::OK )
    {
        skip_ws();
        if ( accept( Parser::terminal_symbol_t::TS_MINUS ) )
        {
            // Stores the "-" token in the list.
            m_tk_list.emplace_back( Token( "-", Token::token_t::OPERATOR ) );
        }
        else if ( accept( Parser::terminal_symbol_t::TS_PLUS ) )
        {
            // Stores the "+" token in the list.
            m_tk_list.emplace_back( Token( "+", Token::token_t::OPERATOR ) );
        }
        
        else if ( accept( Parser::terminal_symbol_t::TS_MULT ) )
        {
            // Stores the "*" token in the list.
            m_tk_list.emplace_back( Token( "*", Token::token_t::OPERATOR ) );
        }
        else if ( accept( Parser::terminal_symbol_t::TS_DIV ) )
        {
            // Stores the "/" token in the list.
            m_tk_list.emplace_back( Token( "/", Token::token_t::OPERATOR ) );
        }
        else if ( accept( Parser::terminal_symbol_t::TS_MODE ) )
        {
            // Stores the "%" token in the list.
            m_tk_list.emplace_back( Token( "%", Token::token_t::OPERATOR ) );
        }
        else if ( accept( Parser::terminal_symbol_t::TS_EXPO ) )
        {
            // Stores the "/" token in the list.
            m_tk_list.emplace_back( Token( "^", Token::token_t::OPERATOR ) );
        }


        else break;
        // After a '+' or '-' we expect a valid term, otherwise we have a missing term.
        if ( not term() )
        {
            // Make the error more specific.
            m_result.type = ResultType::MISSING_TERM;
        }
    }
    return m_result.type == ResultType::OK;
}
/// Validates (i.e. returns true or false) and consumes a **term** from the input expression string.
/*! This method parses and tokenizes a valid term from the input.
 *
 * Production rule is:
 * ```
 *  <term> := <integer>;
 * ```
 * A term is made of a single integer.
 *
 * @return true if a term has been successfuly parsed from the input; false otherwise.
 */
bool Parser::term()
{
    skip_ws();
    // Guarda o início do termo no input, para possíveis mensagens de erro.
    auto begin_token( m_it_curr_symb );
    // Vamos tokenizar o inteiro, se ele for bem formado.
    if ( integer() )
    {
        // Copiar a substring correspondente para uma variável string.
        std::string token_str;
        std::copy( begin_token, m_it_curr_symb, std::back_inserter( token_str ) );
        // Tentar realizar a conversão de string para inteiro (usar stoll()).
        input_int_type token_int;
        try { token_int = stoll( token_str ); }
        catch( const std::invalid_argument & e )
        {
            m_result = ResultType( ResultType::ILL_FORMED_INTEGER,
                               std::distance( m_expr.begin(), begin_token ) );
            return false;
        }
        // Recebemos um inteiro válido, resta saber se está dentro da faixa.
        if ( token_int < std::numeric_limits< required_int_type >::min() or
             token_int > std::numeric_limits< required_int_type >::max() )
        {
            // Fora da faixa, reportar erro.
            m_result = ResultType( ResultType::INTEGER_OUT_OF_RANGE, 
                               std::distance( m_expr.begin(), begin_token ) );
        }
        else
        {
            // Coloca o novo token na nossa lista de tokens.
            m_tk_list.emplace_back( Token( token_str, Token::token_t::OPERAND ) );
        }
    }
    else
    {
        // Create the corresponding error.
        m_result =  ResultType( ResultType::ILL_FORMED_INTEGER, std::distance( m_expr.begin(), m_it_curr_symb ) ) ;
    }
    return m_result.type == ResultType::OK;
}
/// Validates (i.e. returns true or false) and consumes an **integer** from the input expression string.
/*! This method parses a valid integer from the input and, at the same time, add the integer to the token list.
 *
 * Production rule is:
 * ```
 * <integer> := 0 | ["-"],<natural_number>;
 * ```
 * A integer might be a zero or a natural number, which, in turn, might begin with an unary minus.
 *
 * @return true if an integer has been successfuly parsed from the input; false otherwise.
 */
bool Parser::integer()
{
    // Se aceitarmos um zero, então o inteiro acabou aqui.
    if ( accept( terminal_symbol_t::TS_ZERO ) )
        return true; // OK
    // Vamos tentar aceitar o '-'.
    accept( terminal_symbol_t::TS_MINUS );
    return  natural_number();
}
/// Validates (i.e. returns true or false) and consumes a **natural number** from the input string.
/*! This method parses a valid natural number from the input.
 *
 * Production rule is:
 * ```
 * <natural_number> := <digit_excl_zero>,{<digit>};
 * ```
 *
 * @return true if a natural number has been successfuly parsed from the input; false otherwise.
 */
bool Parser::natural_number()
{
    // Tem que vir um número que não seja zero! (de acordo com a definição).
    if ( not digit_excl_zero() )
        return false; // FAILED HERE.
    // Cosumir os demais dígitos, se existirem...
    while( digit() ) /* empty */ ;
    return true; // OK
}
/// Validates (i.e. returns true or false) and consumes a **non-zero digit** from the input expression string.
/*! This method parses a single valid non-zero digit from the input.
 *
 * Production rule is:
 * ```
 * <digit_excl_zero> := "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
 * ```
 *
 * @return true if a non-zero digit has been successfuly parsed from the input; false otherwise.
 */
bool Parser::digit_excl_zero()
{
    return accept( terminal_symbol_t::TS_NON_ZERO_DIGIT );
}
/// Validates (i.e. returns true or false) and consumes a **digit** from the input expression string.
/*! This method parses a single valid digit from the input.
 *
 * Production rule is:
 * ```
 * <digit> := "0"| <digit_excl_zero>;
 * ```
 *
 * @return true if a digit has been successfuly parsed from the input; false otherwise.
 */
bool Parser::digit()
{
    return ( accept( terminal_symbol_t::TS_ZERO ) or digit_excl_zero() ) ? true : false;
/*
    if ( not accept( terminal_symbol_t::TS_ZERO ) )
    {
        return digit_excl_zero();
    }
    return true;
*/
}
/*!
 * This is the parser's  **entry point** method that should be called in the client code.
 * This method tries to (recursivelly) validate an expression.
 * During this process, we also store the tokens into a container.
 *
 m e_ The string with the expression to parse.
 * \return The parsing result.
 *
 * @see ResultType
 */
Parser::ResultType  Parser::parse( std::string e_ )
{
    m_expr = e_; //  Stores the input expression in the private class member.
    m_it_curr_symb = m_expr.begin(); // Defines the first char to be processed (consumed).
    m_result = ResultType( ResultType::OK ); // Ok, by default,
    // We alway clean up the token from (possible) previous processing.
    m_tk_list.clear();
    // Let us ignore any leading white spaces.
    skip_ws();
    if ( end_input() ) // Premature end?
    {
        // Input has finished before we even started to parse...
        m_result =  ResultType( ResultType::UNEXPECTED_END_OF_EXPRESSION,
                std::distance( m_expr.begin(), m_it_curr_symb ) );
    }
    else
    {
        // Trying to validate an expression.
        if ( expression() )
        {
            // At this point there should not be any non-whitespace character in the input expression.
            skip_ws(); // Anyway, let us clear any remaining 'whitespaces'.
            if ( not end_input() ) // Still got a symbol?
            {
                m_result =  ResultType( ResultType::EXTRANEOUS_SYMBOL,
                        std::distance( m_expr.begin(), m_it_curr_symb ) );
            }
        }
    }
    return m_result;
}
/*!
 * Return the list of tokens, which is the by-product created during the syntax analysis.
 * This method should be called in the cliente code **after** tha parser has
 *  returned successfuly.
 */
std::vector< Token >
Parser::get_tokens( void ) const
{
    return m_tk_list;
}
