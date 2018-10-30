/*!
 * Converts an arithmetic expression in infix notation into postfix
 * representation. After the convertion is successfyly done, the
 * program evaluates the expression based on stacks and queues.
 *
 * Input Postfix expression must be in a desired format.
 * Operands and operator, **both must be single character** (all positives).
 * Only '+', '-', '*', '%', '/', and '^' (for exponentiation) operators are expected;
 * Any other character is just ignored.
 */

#include <iostream>  // cout, endl
#include <stack>     // stack
#include <string>    // string
#include <cassert>   // assert
#include <cmath>     // pow
#include <stdexcept> // std::runtime_error
#include <vector>    // std::vector
#include "token.h"

//=== Aliases
using value_type = long int; //!< Type we operate on.
using symbol = Token; //!< A symbol in this implementation is just a char.

// Simple helper functions that identify the incoming symbol.
bool is_operator( symbol s )
{
    return std::string("*^/%+-").find( s.value ) != std::string::npos;

}

bool is_operand( symbol s )
{   
  try
  {
   std::stoi(s.value); 
   return true;
  }
  catch (...)
  {
  return false;
  }
}

bool is_opening_scope( symbol s )
{ return s.value == "("; }

bool is_closing_scope( symbol s )
{ return s.value == ")"; }

/// Check the operand's type of association.
bool is_right_association( symbol op )
{ return op.value == "^"; }

/// Converts a expression in infix notation to a corresponding profix representation.
std::vector<Token> infix_to_postfix( std::vector<Token> v);


/// Change an infix expression into its corresponding postfix representation.
value_type evaluate_postfix( std::vector<Token> v);

/// Returns the precedence value (number) associated with an operator.
short get_precedence( symbol op )
{
    switch( op.value.c_str()[0] )
    {
        case '^' : return 3;
        case '*' : return 2;
        case '/' : return 2;
        case '%' : return 2;
        case '+' : return 1;
        case '-' : return 1;
        case '(' : return 0;
        default  : assert( false );  return -1;
    }
}

/// Determines whether the first operator is >= than the second operator.
bool has_higher_or_eq_precedence( symbol op1 , symbol op2 )
{
    // pega os valores numericos correspondentes aas precedencias.
    int p_op1 = get_precedence( op1 );
    int p_op2 = get_precedence( op2 );

    if ( p_op1 > p_op2 )
    {
        return true;
    }
    else if ( p_op1 < p_op2 )
    {
        return false;
    }
    else // p_op1 == p_op2 
    {
        // exceção a regra, que acontece com associacao direita-esquerda.
        if ( is_right_association( op1 ) ) return false;
        // regra normal.
        return true;
    }
}

/// Execute the binary operator on two operands and return the result.
value_type execute_operator( value_type v1, value_type v2, symbol op )
{
    switch( op.value.c_str()[0] )
    {
        case '^':  return pow( v1,v2 );
        case '*':  return v1*v2;
        case '/':  if ( v2 == 0 ) throw std::runtime_error( "divisao por zero" );
                   return v1/v2;
        case '%':  if ( v2 == 0 ) throw std::runtime_error( "divisao por zero" );
                   return v1%v2;
        case '+': return v1+v2;
        case '-': return v1-v2;
        default: throw std::runtime_error( "undefined operator" );
    }
}

std::vector<Token> infix_to_postfix( std::vector<Token> infix )
{
    std::vector<Token> postfix; // resultado da conversao.
    std::stack< symbol > s; // pilha de ajuda na conversao.

    // Percorrer a entrada, para processar cada item/token/caractere
    for( auto c : infix )
    {
        //std::cout << "\n>>> Infix: \"" << infix << "\"\n";
        //std::cout << ">>> Processando o \'" << c << "\'\n";
        // Operando vai direto pra saida.
        if( is_operand( c ) )
            postfix.push_back(c);
        else if( is_opening_scope(c) )
            s.push( c ); // '(' entra na pilha de espera em cima de quem estiver por la.
        else if( is_closing_scope(c) )
        {
            // desempilhe ateh achar o escopo de abertura correspondente.
            while( not is_opening_scope( s.top() ) )
            {
                postfix.push_back(s.top());
                s.pop();
            }
            s.pop(); // Lembre de descartar o '(' que estah no topo da pilha.
        }
        else // + - ^ *...
        {
            // Desempilhar as operaçoes em espera que são iguais ou superiores
            // em prioridade (com excecao da associacao direita-esquerda).
            while( not s.empty() and has_higher_or_eq_precedence( s.top(), c ) ) // s.top() >= c
            {
                postfix.push_back(s.top());
                s.pop();
            }

            // A operacao que chegar, sempre tem que esperar.
            s.push( c );
        }
        //std::cout << ">>> Posfix: \"" << postfix << "\"\n";
    }

    // Lembre-se de descarregar as operacoes pendentes da pilha.
    while( not s.empty() )
    {
        postfix.push_back(s.top());
        s.pop();
    }

    return postfix;
}

value_type evaluate_postfix( std::vector<Token> postfix )
{
    std::stack< Token > s;

    for( auto c : postfix )
    {
        if ( is_operand( c ) )
            s.push(c);
        else if ( is_operator( c ) )
        {
            value_type op2 = std::stoi(s.top().value); s.pop();
            value_type op1 = std::stoi(s.top().value); s.pop();
            auto result = execute_operator( op1, op2, c ); // ( 2, 9, '*' )
            s.push(Token(std::to_string(result)) );
        }
        else assert( false );
    }

    return std::stoi(s.top().value);
}