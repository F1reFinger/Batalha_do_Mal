#ifndef BATALHA_NAVAL_H
#define BATALHA_NAVAL_H

#include <stdio.h>

// Função para validar entrada de linha e coluna
int validaEntradaLinhaColuna(int linha, char coluna);

// Função para validar a posição do barco no tabuleiro
int validaPosicao(int mat[5][5], int barco, int linha, int coluna, char orientacao);

// Função para verificar se pode atirar
int podeAtirar(int mat[5][5], int linha, int coluna);

// Função para imprimir uma mensagem formatada
void imprimeMensagem(char msg[1000]);

// Função para imprimir um caractere correspondente a uma célula do tabuleiro
char imprimeCelula(int celula, int etapa);

// Função para imprimir o tabuleiro
void imprimeTabuleiro(int mat[5][5], int etapa);

// Função para posicionar um barco no tabuleiro
void posicionaBarco(int mat[5][5], int barco);

// Função para atirar em uma posição do tabuleiro
void atirar(int mat[5][5], int linha, int coluna);

// Função para calcular pontuação após um tiro
int calculaPontuacao(int mat[5][5], int linha, int coluna);

// Função para exibir mensagens específicas para posicionamento de barcos
void funcMSN(int tamBarco, int i, int jogador);

#endif // BATALHA_NAVAL_H