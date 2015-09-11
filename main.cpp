/*
 * main.cpp
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
 *  26/11/2014
 */

#include "CalcLin.h"
#include <cstdio>


//// Função para imprimir um número complexo
void ImprimeComplexo(complexo z)
{
    char str[60];
    double z_re = real(z), z_im = imag(z);

    // Arredonda o valor de zero
    if (fabs(z_re) < 1.0E-15) z_re = 0.0;
    if (fabs(z_im) < 1.0E-15) z_im = 0.0;

    // Se for real puro
    if (z_im == 0.0)
    {
        sprintf(str, "%.10G", z_re);
    }
    else
    {
        // Se for imaginário puro
        if (z_re == 0.0)
        {
            if (z_im == 1.0)
            {
                sprintf(str, "i");
            }
            else
            {
                if (z_im == -1.0)
                {
                    sprintf(str, "-i");
                }
                else
                {
                    sprintf(str, "%.10Gi", z_im);
                }
            }
        }
        else
        {
            // Se tiver partes real e imaginária
            if (z_im == 1.0)
            {
                sprintf(str, "%.10G+i", z_re);
            }
            else
            {
                if (z_im == -1.0)
                {
                    sprintf(str, "%.10G-i", z_re);
                }
                else
                {
                    sprintf(str, "%.10G %+.10Gi", z_re, z_im);
                }
            }
        }
    }

    // Troca '.' por ','
    for(unsigned int i = 0; i < strlen(str); i++)
    {
        if(str[i] == '.')   str[i] = ',';
    }

    // Exibe o Resultado
    cout << "\n\t" << str << endl;
}

/////// Função principal
int main(void)
{
    char expressao[100];
    complexo resultado;
    CalcLin Calc;
    int cod_erro;

    // Cabeçalho da interface
    cout << endl << "CalcLin - 7.0\n";
    cout << "GNU General Public License versão 2 ou posterior\n";
    cout << "por Rafael Andrade - rafaelsandrade@gmail.com\n";
    cout << "_________________________________________________\n\n";

    while(1)
    {
        // Inicia a Linha de comando
        cout << endl << ">> ";

        // Recebe a expessão
        cin.getline(expressao, 100);

        //Calcula a expressão
        cod_erro = Calc.Solucao(resultado, expressao);

        // Se 'expressão' for string vazia ignora
        if (cod_erro == -2) continue;

        // Se não tiver erro exibe o resultado
        if( cod_erro == -1 )
        {
            ImprimeComplexo(resultado);
        }
        else
        {
            // Indica onde houve erro
            cout << expressao << endl;
            for (int i = 0; i < cod_erro; i++)  cout << " ";
            cout << "^" << endl;
        }
    }
}
