#include<stack>
#include<iostream>
#include<string>
# define TAM 100
#include<fstream>
#include "parsser.h"
#include "posfixo.h"

using namespace std;

std::vector<string> leitor(string nome_aquivo)
{
    ifstream ler;// ler o arquivo
    string expression;
    std::vector<string> v_expression;
    ler.open(nome_aquivo);


    if( !ler.is_open() )// testa se o arquivo tá aberto
    {
    cout << "Falha ao abrir o arquivo!" <<endl;
    //exit(1);
    }
    else
    {
    cout << "Arquivo aberto com sucesso" << endl;

        while(getline(ler, expression))
        {
         v_expression.push_back(expression);
        }
    }
 return v_expression;
    
}

string tratarerros(Parser::ResultType e_status)
{
   switch( e_status.type )
    {
        case 0:    return "";
        case 1:    return "A expressão está vazia";
        case 2:    return "Não é uma expressão de inteiros";
        case 3:    return "Tá faltando um termo na expressão";
        case 4:    return "Tem um simbolo errado na expressão";
        case 5:    return "A expressão tem um inteiro fora do intervalo";          
    }
 return "Erro não catalogado";
}


int main(int argc, char const *argv[])
{
	string nome_aquivo;
    Parser p;  
    std::vector<string> aux;
    std::vector<Token> resultado;
    Parser::ResultType e_status;

	cout << "Nome arquivo"<< endl;
	getline(cin,nome_aquivo);

	aux = leitor(nome_aquivo);

    for (size_t i = 0; i < aux.size(); ++i)
    {
        e_status = p.parse(aux[i]);

     
     if (e_status.type == 0)
     {
        cout<< "Status da expreesão inserida: OK" << endl;

        resultado = infix_to_postfix(p.get());// Recebe a sting do arquivo

        cout << "Resultado da expressão " << evaluate_postfix(resultado) << endl;
     }  
     else
     {
        cout <<tratarerros(e_status)<<endl;

     } 
    }

	return 0;
}