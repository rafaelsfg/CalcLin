/*
 * CalcLin.h
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

#ifndef __CALCLIN_H__
#define __CALCLIN_H__

#include <stack>
#include <list>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <complex>

using namespace std;

typedef complex<double> complexo;

// Lista de operações
enum operacao { ADD, SUB, MUL, DIV, POT, OPO, IGL, MAI, MEN, MAIG, MEIG, DIF, PAR,
                ARG, SQR, LN, LOG, SEN, COS, TAN, ABS, ASEN, ACOS, ATAN, EXP, MOD,
                MAX, MIN, MED, SOM, PROD, REAL, IMAG, ARGM, CONJ, POLAR };

// Define o número complexo 'i'
const complexo cpx_i(0.0, 1.0);

// Define a classe 'CalcLin'
class CalcLin
{
    // Pilha de Operadores
    stack <operacao> p_operador;

    // Pilha de Operandos
    stack <complexo> p_operando;

    // Estrutura de uma variável
    struct var { char nome[20]; complexo valor; };

    // Lista de variáveis
    list<struct var> l_variavel;

    // Estrutura de uma função
    struct fun { char nome[20]; operacao cod_op; };

    // Lista de funções
    list<struct fun> l_funcao;

    // Estrutura de argumentos de funções
    struct argm { int contpar; unsigned int nun_arg; };

    // Pilha de argumentos de funções
    stack <struct argm> p_argumento;

    // Armazena o número de constantes
    int num_constante;

    // Verifica se 's' é um número decimal
    bool EhNumero(const char s[]);

    // Verifica se 's' é uma string válida como variável
    bool StringValida(const char s[]);

    // Retorna o codigo da operação
    operacao IdOperacao(char s);

    // Retorna o nível de prioridade das operções
    int Prioridade(operacao o);

    // Realiza as operações na pilha
    int CalculoPilha(void);

    public:

    // Resolve a expressão
    int Solucao(complexo &solucao, char exp[]);

    CalcLin();

    ~CalcLin();

};
#endif
