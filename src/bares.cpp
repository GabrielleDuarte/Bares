#include<stack>
#include<iostream>
#include<string>
# define TAM 100
#include<fstream>
#include "parsser.h"
#include "posfixo.h"

using namespace std;

string leitor(string nome_aquivo)
{
    ifstream ler;// ler o arquivo
    string expression;

    ler.open(nome_aquivo);


    if( !ler.is_open() )// testa se o arquivo tá aberto
    {
    cout << "Falha ao abrir o arquivo!" <<endl;
    //exit(1);
    }
    else
    {
    cout << "Arquivo aberto com sucesso" << endl;

    getline(ler, expression);

    return expression;

    }

    
}


int main(int argc, char const *argv[])
{
	string nome_aquivo;
    Parser p;  
    string aux;
    std::vector<Token> resultado;
    Parser::ResultType e_status;

	cout << "Nome arquivo"<< endl;
	getline(cin,nome_aquivo);

	aux = leitor(nome_aquivo);


     e_status = p.parse(aux);

     
     if (e_status.type == 0)
     {
        cout<< "Status da expreesão inserida: OK" << endl;

        resultado = infix_to_postfix(p.get());// Recebe a sting do arquivo

        cout << "Resultado da expressão " << evaluate_postfix(resultado) << endl;
     }  
     else
     {
        cout<< "Status da expreesão inserida: expressão com erro do tipo " << e_status.type << endl;

     } 
 

	return 0;
}