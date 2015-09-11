/*
 * CalcLin.cpp
 *
 * Copyright 2014 Rafael Andrade <rafaelsandrade@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * 26/11/2014 (versão 7.0)
 */


#include "CalcLin.h"

// Função complexa para o arco tangente
// Fonte: http://www.physics.nyu.edu/grierlab/idl_html_help/A19.html
complexo atan(const complexo& z)
{
    double x = real(z), y = imag(z);

    return  0.5 * atan2(x + x, 1.0 - x * x - y * y) +
            0.25 * log((x * x + (y + 1.0) * (y + 1.0)) / (x * x + (y - 1.0) * (y - 1.0))) * cpx_i;
    //return -i / 2.0 * log((1.0 + z * i) / (1.0 - z * i));
}

// Função complexa para o arco cosseno
// Fonte: http://www.physics.nyu.edu/grierlab/idl_html_help/A4.html
complexo acos(const complexo& z)
{
    double x = real(z), y = imag(z);

    double a = 0.5 * sqrt((x + 1.0) * (x + 1.0) + y * y);
    double b = 0.5 * sqrt((x - 1.0) * (x - 1.0) + y * y);

    if (y >= 0.0) return acos(a - b) - log(a + b + sqrt((a + b) * (a + b) - 1.0)) * cpx_i;

    return acos(a - b) + log(a + b + sqrt((a + b) * (a + b)-1.0)) * cpx_i;
}

// Função complexa para arco seno
// Fonte: http://www.physics.nyu.edu/grierlab/idl_html_help/A17.html
complexo asin(complexo z)
{
    double x = real(z), y = imag(z);

    double a = 0.5 * sqrt((x + 1.0) * (x + 1.0) + y * y);
    double b = 0.5 * sqrt((x - 1.0) * (x - 1.0) + y * y);

    if (y >= 0.0) return asin(a - b) + log(a + b + sqrt((a + b) * (a + b) - 1.0)) * cpx_i;

    return asin(a - b) - log(a + b + sqrt((a + b) * (a + b) - 1.0)) * cpx_i;
}

///////// Avalia a expressão
int CalcLin::Solucao(complexo &total, char expr[])
{
    unsigned int i, j = 0, dim_expr = strlen(expr);
    int contpar = 0;    // Saldo de parênteses
    char temp[20] = ""; // Armazena operandos
    char nome_var[20] = ""; // Armazena nome de variáveis
    bool operando = true, operador = false, atr = true; // Verifica próxima entrada

    // Se for expressão vazia retorna
    if (dim_expr == 0) return -2;

    // Esvazia as pilhas
    while (!p_operando.empty())     p_operando.pop();
    while (!p_operador.empty()) p_operador.pop();
    while (!p_argumento.empty())    p_argumento.pop();

    // Avalia a expressão
    for (i = 0; i < dim_expr; i++)
    {
        // Se for espaço ignora
        if (expr[i] == ' ')
        {
            // Se já foi digitada algum caracter, o próximo deve ser operador
            if (j > 0)  operando = false;
            continue;
        }

        // Verifica se é letra ou número
        if (isalnum(expr[i]) || expr[i] == ',' || expr[i] == '_')
        {
            // Se não for esperado operando, retorna erro
            if (operando)
            {
                // Recebe até 19 dígitos
                if (j < 19)
                {
                    // Troca ',' por '.'
                    if (expr[i] == ',') temp[j++] = '.';
                    else temp[j++] = expr[i];
                    temp[j] = '\0';
                }
                operador = true;
                continue;
            }

            cout << "Erro de sintaxe: '" << expr[i] << "' inesperado.\n";
            return i;
        }

        // Se não foi digitado nenhum alfanumérico ativa 'operando'
        operando = true;

        // Se for ':' e o próximo for '='
        if (expr[i] == ':' && (i + 1) < dim_expr && expr[i + 1] == '=')
        {
            // Verifica se teve string digitada antes e se 'atr' está ativado
            if (j > 0 && atr)
            {
                // Se 'temp' é uma string válida para um nome de variável
                if (StringValida(temp))
                {
                    operador = false;
                    strcpy(nome_var, temp);
                    j = 0;
                    atr = false;
                    i++;
                    continue;
                }
                cout << "Erro: '" << temp << "' é um nome inválido para uma variável.\n";
                return i;
            }

            cout << "Erro de sintaxe: ':' inesperado.\n";
            return i;
        }

        // Se ':=' não foi utilizado, bloqueia a atribuição
        atr = false;

        // Se for '('
        if (expr[i] == '(')
        {
            contpar++;
            operador = false;

            // Se tem string informada, verifica se é função
            if (j > 0)
            {
                // Procura o nome da função
                list<struct fun>::iterator it;

                for (it = l_funcao.begin(); it != l_funcao.end(); it++)
                {
                    if (!strcmp((*it).nome, temp))  break;
                }

                // Se encontrar a função
                if (it != l_funcao.end())
                {
                    p_operador.push((*it).cod_op);
                    struct argm a;
                    a.contpar = contpar;
                    a.nun_arg = 1;
                    p_argumento.push(a);
                    j = 0;
                }
                else
                {
                    cout << "Erro: função '" << temp << "' desconhecida.\n";
                    return i - 1;
                }
            }

            // Empilha o parêntese na pilha de operações
            p_operador.push(PAR);
            continue;
        }

        // Se 'temp' não está vazia
        if (j > 0)
        {
            // Verifica se 'temp' é número
            if (EhNumero(temp))
            {
                // Empilha o número na pilha de operandos
                p_operando.push(atof(temp));
            }
            else
            {
                // Verifica se 'temp' é variável
                list<struct var>::iterator it;

                for (it = l_variavel.begin(); it != l_variavel.end(); it++)
                {
                    if (!strcmp((*it).nome, temp))  break;
                }

                // Se encontrar a variável, empilha seu valor na pilha de operandos
                if(it != l_variavel.end())
                {
                    p_operando.push((*it).valor);
                }
                else
                {
                    cout << "Erro: '" << temp << "' desconhecido.\n";
                    return i - 1;
                }
            }

            j = 0;
        }

        // Se for operação...
        if (expr[i] == '*' || expr[i] == '/' || expr[i] == '^' || expr[i] == '+' || expr[i] == '-' ||
            expr[i] == '=' || expr[i] == '<' || expr[i] == '>')
        {
            // Para '+' ou '-' no início das expressões ou após '('
            if (!operador && expr[i] == '+')    continue;

            if (!operador && expr[i] == '-')
            {
                p_operador.push(OPO);
                continue;
            }

            // Se for esperado um operando
            if (operador)
            {
                operacao cod_op = IdOperacao(expr[i]);
                operador = false;

                // Operação '<='
                if(expr[i] == '<' && (i + 1) < dim_expr && expr[i + 1] == '=')
                {
                    cod_op = MEIG;
                    i++;
                }

                // Operação '>='
                if(expr[i] == '>' && (i + 1) < dim_expr && expr[i + 1] == '=')
                {
                    cod_op = MAIG;
                    i++;
                }

                // Operação '<>'
                if(expr[i] == '<' && (i + 1) < dim_expr && expr[i + 1] == '>')
                {
                    cod_op = DIF;
                    i++;
                }

                // Empilha a operação atual se ela for de prioridade maior
                // que a operação do topo da pilha
                if (p_operador.empty() || Prioridade(p_operador.top()) < Prioridade(cod_op))
                {
                    p_operador.push(cod_op);
                    continue;
                }

                // Resolve as operações da pilha enquanto elas forem de
                // prioridade maior ou igual a operação atual
                while (!p_operador.empty() && Prioridade(p_operador.top()) >= Prioridade(cod_op))
                {
                    if(CalculoPilha()) return i;
                }

                // Empilha a operação atual
                p_operador.push(cod_op);
                continue;
            }

            cout << "Erro de sintaxe: '" << expr[i] << "' inesperado.\n";
            return i;
        }

        // Se for ';' (separador de argumentos de funções)
        if (expr[i] == ';')
        {
            // Verifica se é esperado um operador e se o parêntese esta no âmbito da função
            if (operador && !p_argumento.empty() && (p_argumento.top().contpar == contpar))
            {
                // Resolve as operações antes do ';'
                while (!p_operador.empty() && p_operador.top() != PAR && p_operador.top() != ARG)
                {
                    if (CalculoPilha()) return i;
                }

                // Empilha argumento na pilha de operadores
                p_operador.push(ARG);

                // Incrementa 1 na contagem de argumentos
                p_argumento.top().nun_arg += 1;
                operador = false;
                continue;
            }

            cout << "Erro de sintaxe: ';' inesperado.\n";
            return i;
        }

        // Se for ')'
        if (expr[i] == ')')
        {
            // Se '(' nao foi digitado antes, retorna erro
            if (contpar <= 0)
            {
                cout << "Erro de sintaxe: Faltou um abre parentese.\n";
                return i;
            }

            if (operador)
            {
                operando = false;

                // Resolve as operações anteriores
                while (!p_operador.empty() && p_operador.top() != PAR)
                {
                    if (CalculoPilha()) return i;
                }

                // Retira ')' da pilha de operadores
                if (!p_operador.empty())    p_operador.pop();

                // Se tiver função na pilha de operadores, resolve
                if (!p_operador.empty() && p_operador.top() >= SQR)
                {
                    if (CalculoPilha()) return i;
                }

                contpar--;
                continue;
            }

            cout << "Erro de sintaxe: ')' inesperado.\n";
            return i;
        }

        // Se 'expr[i]' for desconhecido
        cout << "Erro de sintaxe: '" << expr[i] <<"' desconhecido.\n";
        return i;
    }

    ////// Fim da avaliação da expressão ///////////

    // Se tiver '(' em excesso
    if (contpar > 0)
    {
        cout << "Erro de sintaxe: Faltou um fecha parentese.\n";
        return i;
    }

    // Se tiver ')' em excesso
    if (contpar < 0)
    {
        cout << "Erro de sintaxe: Faltou um abre parentese.\n";
        return i;
    }

    // Se 'temp' não está vazia
    if (j > 0)
    {
        // Verifica se 'temp' é número
        if (EhNumero(temp))
        {
            // Empilha 'temp' na pilha de operandos
            p_operando.push(atof(temp));
        }
        else
        {
            // Verifica se 'temp' é variável
            list<struct var>::iterator it;

            for (it = l_variavel.begin(); it != l_variavel.end(); it++)
            {
                if (!strcmp((*it).nome, temp))  break;
            }

            // Se encontrar a variavel 'temp' empilha em p_operando
            if(it != l_variavel.end())
            {
                p_operando.push((*it).valor);
            }
            else
            {
                cout << "Erro: '" << temp << "' desconhecido.\n";
                return i;
            }
        }
    }

    // Se tiver operação no final da expressão
    if (!operador)
    {
        cout << "Erro de sintaxe: Faltou um operando.\n";
        return i;
    }

    // Calcula o restante das operações
    while (!p_operador.empty())
    {
        if (CalculoPilha()) return i;
    }

    // Retorna o resultado
    if (p_operando.size() == 1)
    {
        total = p_operando.top();   // 'total' recebe o resultado da expressão

        l_variavel.front().valor = total; // 'ans' recebe o valor da expressão

        if (strlen(nome_var) > 0) // Atribuição
        {
            int cont = 1;
            list<struct var>::iterator it;

            // Procura se já existe a variavel 'nome_var'
            for (it = l_variavel.begin(); it != l_variavel.end(); it++)
            {
                if (!strcmp((*it).nome, nome_var))
                {
                    // Se não for constante, atribui o valor à variável
                    if (cont > num_constante)
                    {
                        (*it).valor = total;
                        cout << "    " << nome_var << " = ";
                        break;
                    }
                    else
                    {
                        cout << "Erro: O valor de '" << nome_var << "' não pode ser mudado.\n";
                        return 0;
                    }
                }
                cont++;
            }

            // Se não existir a variável, cria uma nova
            if(it == l_variavel.end())
            {
                struct var x;
                strcpy(x.nome, nome_var);
                x.valor = total;
                l_variavel.push_back(x);
                cout << "    " << nome_var << " = ";
            }
        }
    }
    else
    {
        cout << "\nErro interno: Pilha operando vazia ou com mais de um elemento.\n";
        return 0;
    }

    return -1;
}

// Realiza os calculos na pilha de operandos
int CalcLin::CalculoPilha(void)
{
    complexo temp1, temp2;  // Operandos
    operacao id_op = p_operador.top();  // Código da operação

    p_operador.pop();

    switch(id_op)
    {
        // Operação '+'
        case ADD:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '+'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            p_operando.push(temp2 + temp1);
            return 0;

        // Operação '-' (subtração)
        case SUB:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '-'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            p_operando.push(temp2 - temp1);
            return 0;

        // Operação '-' (oposto aditivo)
        case OPO:
            if(p_operando.size() < 1)
            {
                cout << "\nErro interno (CalculoPilha): 'OPO'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();

            // Corrige bug da operação '-' da biblioteca 'complex'
            if (imag(temp1) == 0.0) p_operando.push(-real(temp1));
            else p_operando.push(-temp1);
            return 0;

        // Operação '*'
        case MUL:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '*'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            p_operando.push(temp2 * temp1);
            return 0;

        // Operação '/'
        case DIV:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '/'\n";
                return 1;
            }
            temp1 = p_operando.top();
            if(temp1 == 0.0)
            {
                cout << "Erro em '/': divisão por zero.\n";
                return 1;
            }
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            p_operando.push(temp2 / temp1);
            return 0;

        // Operação '^' (potência)
        case POT:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '^'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            temp1 = pow(temp2, temp1);
            p_operando.push(temp1);
            return 0;

        // Operação '='
        case IGL:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '='\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            p_operando.push((double)(temp2 == temp1));
            return 0;

        // Operação '<>' (diferença)
        case DIF:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '<>'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            p_operando.push((double)(temp2 != temp1));
            return 0;

        // Operação '<' (somente para real puro)
        case MEN:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '<'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            if (imag(temp1) != 0.0 || imag(temp2) != 0.0)
            {
                cout << "Erro em '<': operação inválida para números complexos.\n";
                return 1;
            }
            p_operando.push((double)(real(temp2) < real(temp1)));
            return 0;

        // Operação '>' (somente para real puro)
        case MAI:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '>'\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            if (imag(temp1) != 0.0 || imag(temp2) != 0.0)
            {
                cout << "Erro em '>': operação inválida para números complexos.\n";
                return 1;
            }
            p_operando.push((double)(real(temp2) > real(temp1)));
            return 0;

        // Operação '>=' (somente para real puro)
        case MAIG:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '>='\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            if (imag(temp1) != 0.0 || imag(temp2) != 0.0)
            {
                cout << "Erro em '>=': operação inválida para números complexos.\n";
                return 1;
            }
            p_operando.push((double)(real(temp2) >= real(temp1)));
            return 0;

        // Operação '<=' (somente para real puro)
        case MEIG:
            if(p_operando.size() < 2)
            {
                cout << "\nErro interno (CalculoPilha): '<='\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            if (imag(temp1) != 0.0 || imag(temp2) != 0.0)
            {
                cout << "Erro em '<=': operação inválida para números complexos.\n";
                return 1;
            }
            p_operando.push((double)(real(temp2) <= real(temp1)));
            return 0;

        // Função raiz quadrada
        case SQR:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'raiz' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(sqrt(temp1));
            p_argumento.pop();
            return 0;

        // Função 'ln' (logaritmo natural)
        case LN:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'ln' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            if (temp1 == 0.0)
            {
                cout << "Erro em 'ln': argumento '0' inválido.\n";
                return 1;
            }
            p_operando.pop();
            p_operando.push(log(temp1));
            p_argumento.pop();
            return 0;

        // Função 'log' (base 10)
        case LOG:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'log' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            if (temp1 == 0.0)
            {
                cout << "Erro em 'log': argumento '0' inválido.\n";
                return 1;
            }
            p_operando.pop();
            p_operando.push(log10(temp1));
            p_argumento.pop();
            return 0;

        // Função seno
        case SEN:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'sen' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(sin(temp1));
            p_argumento.pop();
            return 0;

        // Função cosseno
        case COS:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'cos' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(cos(temp1));
            p_argumento.pop();
            return 0;

        // Função tangente
        case TAN:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'tan' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            if (isnan(norm(temp1)))
            {
                cout << "Erro em 'tan': argumento '" << temp1 << "' inválido.\n";
                return 1;
            }
            p_operando.push(tan(temp1));
            p_argumento.pop();
            return 0;

        // Função 'abs' (valor absoluto ou módulo)
        case ABS:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'abs' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(abs(temp1));
            p_argumento.pop();
            return 0;

        // Função arco-cosseno
        case ACOS:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'acos' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(acos(temp1));
            p_argumento.pop();
            return 0;

        // Função arco-seno
        case ASEN:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'asen' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(asin(temp1));
            p_argumento.pop();
            return 0;

        // Função arco-tangente
        case ATAN:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'atan' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(atan(temp1));
            p_argumento.pop();
            return 0;

        // Função exponencial (e^x)
        case EXP:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'exp' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(exp(temp1));
            p_argumento.pop();
            return 0;

        // Função módulo (resto da divisão, somente para reais puros)
        case MOD:
            if(!p_argumento.empty() && (p_argumento.top().nun_arg != 2))
            {
                cout << "Erro de sintaxe: função 'mod' necessita de 2 argumentos\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            if (imag(temp1) != 0.0 || imag(temp2) != 0.0)
            {
                cout << "Erro em 'mod': operação inválida para números complexos.\n";
                return 1;
            }

            if (real(temp1) == 0.0)
            {
                cout << "Erro em 'mod': argumento '0' inválido.\n";
                return 1;
            }
            p_operando.push(fmod(real(temp2), real(temp1)));
            p_argumento.pop();
            return 0;

        // Função que retorna valor máximo entre os argumentos (somente para reais puros)
        case MAX:
            if(!p_argumento.empty() && p_operando.size() < p_argumento.top().nun_arg)
            {
                cout << "Erro interno 'max': operandos menor que argumentos\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            for (unsigned int i = 0; i < p_argumento.top().nun_arg - 1; i++)
            {
                if (imag(temp1) != 0.0)
                {
                    cout << "Erro em 'max': função não definida para números complexos.\n";
                    return 1;
                }

                if (!p_operando.empty() && real(p_operando.top()) > real(temp1)) temp1 = p_operando.top();
                p_operando.pop();
            }
            p_operando.push(temp1);
            p_argumento.pop();
            return 0;

        // Função que retorna valor mínimo entre os argumentos (somente para reais puros)
        case MIN:
            if(!p_argumento.empty() && p_operando.size() < p_argumento.top().nun_arg)
            {
                cout << "Erro interno 'min': operandos menor que argumentos\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            for (unsigned int i = 0; i < p_argumento.top().nun_arg - 1; i++)
            {
                if (imag(temp1) != 0.0)
                {
                    cout << "Erro em 'min': função não definida para números complexos.\n";
                    return 1;
                }

                if (!p_operando.empty() && real(p_operando.top()) < real(temp1)) temp1 = p_operando.top();
                p_operando.pop();
            }
            p_operando.push(temp1);
            p_argumento.pop();
            return 0;

        // Função que calcula a média entre os argumentos
        case MED:
            if(!p_argumento.empty() && p_operando.size() < p_argumento.top().nun_arg)
            {
                cout << "Erro interno 'media': operandos menor que argumentos\n";
                return 1;
            }
            temp1 = 0.0;
            for (unsigned int i = 0; i < p_argumento.top().nun_arg; i++)
            {
                temp1 += p_operando.top();
                p_operando.pop();
            }

            p_operando.push(temp1 / (double)p_argumento.top().nun_arg);
            p_argumento.pop();
            return 0;

        // Função que calcula a soma dos argumentos
        case SOM:
            if(!p_argumento.empty() && p_operando.size() < p_argumento.top().nun_arg)
            {
                cout << "Erro interno 'soma': operandos menor que argumentos\n";
                return 1;
            }
            temp1 = 0.0;
            for (unsigned int i = 0; i < p_argumento.top().nun_arg; i++)
            {
                temp1 += p_operando.top();
                p_operando.pop();
            }
            p_operando.push(temp1);
            p_argumento.pop();
            return 0;

        // Função que calcula o produto dos argumentos
        case PROD:
            if(!p_argumento.empty() && p_operando.size() < p_argumento.top().nun_arg)
            {
                cout << "Erro interno 'prod': operandos menor que argumentos\n";
                return 1;
            }
            temp1 = 1.0;
            for (unsigned int i = 0; i < p_argumento.top().nun_arg; i++)
            {
                temp1 *= p_operando.top();
                p_operando.pop();
            }
            p_operando.push(temp1);
            p_argumento.pop();
            return 0;

        // Função que retorna a parte real do complexo
        case REAL:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'real' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(real(temp1));
            p_argumento.pop();
            return 0;

        // Função que retorna a parte imaginária do complexo
        case IMAG:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'imag' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(imag(temp1));
            p_argumento.pop();
            return 0;

        // Função que retorna o argumento do complexo
        case ARGM:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'arg' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(arg(temp1));
            p_argumento.pop();
            return 0;

        // Função que retorna o conjugado do complexo
        case CONJ:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 1)
            {
                cout << "Erro de sintaxe: função 'conj' necessita de um argumento\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            p_operando.push(conj(temp1));
            p_argumento.pop();
            return 0;

        // Função que retorna o complexo correspondente à forma polar dada
        case POLAR:
            if(!p_argumento.empty() && p_argumento.top().nun_arg != 2)
            {
                cout << "Erro de sintaxe: função 'polar' necessita de dois argumentos\n";
                return 1;
            }
            temp1 = p_operando.top();
            p_operando.pop();
            temp2 = p_operando.top();
            p_operando.pop();
            if (imag(temp1) != 0.0 || imag(temp2) != 0.0)
            {
                cout << "Erro em 'polar': função não definida para números complexos.\n";
                return 1;
            }
            p_operando.push(polar(real(temp2), real(temp1)));
            p_argumento.pop();
            return 0;

        // Se for '(' ou ';' ignora
        case PAR:
        case ARG: return 0;
    }

    cout << "\nErro interno (CalculoPilha): Operação '" << id_op << "'não conhecida\n";
    return 1;
}

// Verifica se 's' é um número
bool CalcLin::EhNumero(const char s[])
{
    bool ponto_decimal = false;

    if(strlen(s) == 0 || !isdigit(s[0])) return false;

    for (unsigned int i = 1; i < strlen(s); i++)
    {
        if (!isdigit(s[i]))
        {
            if (s[i] == '.' && !ponto_decimal) ponto_decimal = true;
            else return false;
        }
    }

    return true;
}

// Verifica se 's' é uma string válida como variável
bool CalcLin::StringValida(const char s[])
{
    if(strlen(s) == 0 || !isalpha(s[0])) return false;

    for (unsigned int i = 1; i < strlen(s); i++)
    {
        if(!isalnum(s[i]))
        {
            if(s[i] != '_') return false;
        }
    }

    return true;
}

// Retorna o codigo da operação
operacao CalcLin::IdOperacao(char s)
{
    switch (s)
    {
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MUL;
        case '/': return DIV;
        case '^': return POT;
        case '(': return PAR;
        case '<': return MEN;
        case '>': return MAI;
        case '=': return IGL;
    }

    cout << "\nErro interno (Operacao). '" << s << "' desconhecido.\n";
    return ADD;
}

// Verifica as prioridades das operações
int CalcLin::Prioridade(operacao o)
{
    int cod_op = o;

    if(cod_op >= SQR) return 7;

    switch(cod_op)
    {
        case PAR:
        case ARG: return 1;
        case IGL:
        case MAI:
        case MEN:
        case MAIG:
        case MEIG:
        case DIF: return 2;
        case ADD:
        case SUB: return 3;
        case MUL:
        case DIV: return 4;
        case OPO: return 5;
        case POT: return 6;
    }

    cout << "\nErro interno (Prioridade). '" << o << "' desconhecido.\n";
    return 0;
}

// Inicializa a calculadora
CalcLin::CalcLin(void)
{
    struct var x;

    // Armazena as constantes
    strcpy(x.nome, "ans");  x.valor = 0.0;                  l_variavel.push_back(x);
    strcpy(x.nome, "pi");   x.valor = 3.141592653589793;    l_variavel.push_back(x);
    strcpy(x.nome, "e");    x.valor = 2.718281828459045;    l_variavel.push_back(x);
    strcpy(x.nome, "i");    x.valor = cpx_i;                l_variavel.push_back(x);

    num_constante = l_variavel.size();

    struct fun f;

    //Armazena as funções
    strcpy(f.nome, "raiz");     f.cod_op = SQR;     l_funcao.push_back(f);
    strcpy(f.nome, "ln");       f.cod_op = LN;      l_funcao.push_back(f);
    strcpy(f.nome, "log");      f.cod_op = LOG;     l_funcao.push_back(f);
    strcpy(f.nome, "sen");      f.cod_op = SEN;     l_funcao.push_back(f);
    strcpy(f.nome, "cos");      f.cod_op = COS;     l_funcao.push_back(f);
    strcpy(f.nome, "tan");      f.cod_op = TAN;     l_funcao.push_back(f);
    strcpy(f.nome, "abs");      f.cod_op = ABS;     l_funcao.push_back(f);
    strcpy(f.nome, "acos");     f.cod_op = ACOS;    l_funcao.push_back(f);
    strcpy(f.nome, "asen");     f.cod_op = ASEN;    l_funcao.push_back(f);
    strcpy(f.nome, "atan");     f.cod_op = ATAN;    l_funcao.push_back(f);
    strcpy(f.nome, "exp");      f.cod_op = EXP;     l_funcao.push_back(f);
    strcpy(f.nome, "mod");      f.cod_op = MOD;     l_funcao.push_back(f);
    strcpy(f.nome, "max");      f.cod_op = MAX;     l_funcao.push_back(f);
    strcpy(f.nome, "min");      f.cod_op = MIN;     l_funcao.push_back(f);
    strcpy(f.nome, "med");      f.cod_op = MED;     l_funcao.push_back(f);
    strcpy(f.nome, "soma");     f.cod_op = SOM;     l_funcao.push_back(f);
    strcpy(f.nome, "prod");     f.cod_op = PROD;    l_funcao.push_back(f);
    strcpy(f.nome, "real");     f.cod_op = REAL;    l_funcao.push_back(f);
    strcpy(f.nome, "imag");     f.cod_op = IMAG;    l_funcao.push_back(f);
    strcpy(f.nome, "arg");      f.cod_op = ARGM;    l_funcao.push_back(f);
    strcpy(f.nome, "conj");     f.cod_op = CONJ;    l_funcao.push_back(f);
    strcpy(f.nome, "polar");    f.cod_op = POLAR;   l_funcao.push_back(f);
}

CalcLin::~CalcLin(void)
{
    l_variavel.clear();
    l_funcao.clear();
}
