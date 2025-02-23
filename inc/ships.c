#include "ships.h"
#include <stdio.h>

int validaEntradaLinhaColuna(int linha, char coluna) {
    if (coluna < 'A' || coluna > 'E') return 0;
    if (linha < 0 || linha > 5) return 0;
    return 1;
}

int validaPosicao(int mat[5][5], int barco, int linha, int coluna, char orientacao) {
    int aux = 0, litrao = 1;
    if (linha < 0 || linha > 5 || coluna < 0 || coluna > 5 || barco < 1 || barco > 3) return 0;
    return mat[linha][coluna] == 0 ? 1 : 0;
}

int podeAtirar(int mat[5][5], int linha, int coluna) {
    return !(mat[linha][coluna] == -2 || mat[linha][coluna] == 10 || mat[linha][coluna] == 20 || mat[linha][coluna] == 30 || mat[linha][coluna] == 50);
}

void imprimeMensagem(char msg[1000]) {
    printf("************************\n");
    printf("*\n");
    printf("* %s\n", msg);
    printf("*\n");
    printf("************************\n");
}

char imprimeCelula(int celula, int etapa) {
    if (etapa == 0) {
        if (celula == 0) return '.';
        if (celula == -1) return '*';
        if (celula >= 1 && celula <= 3) return '0' + celula;
    } else if (etapa == 1) {
        if (celula == 0 || celula == 1 || celula == -1 || celula == 2 || celula == 3) return '.';
        if (celula == -2) return 'x';
        if (celula == 10 || celula == 20 || celula == 30) return 'N';
        if (celula == 50) return 'A';
    }
    return '?';
}

void imprimeTabuleiro(int mat[5][5], int etapa) {
    printf("     A B C D E\n");
    printf("   # # # # # # # \n");
    for (int i = 0; i < 5; i++) {
        printf("%02d # ", i + 1);
        for (int j = 0; j < 5; j++) {
            printf("%c ", imprimeCelula(mat[i][j], etapa));
        }
        printf("#\n");
    }
    printf("   # # # # # # # \n");
}

void posicionaBarco(int mat[5][5], int barco) {
    int linha, coluna1, linha1;
    char coluna;
    
    if (barco == 1) {
        scanf("%d %c", &linha, &coluna);
        coluna1 = coluna - 'A';
        linha1 = linha - 1;
        
        while (!validaPosicao(mat, barco, linha1, coluna1, 'V') || !validaEntradaLinhaColuna(linha, coluna)) {
            printf("Posicao indisponivel!\n");
            scanf("%d %c", &linha, &coluna);
            coluna1 = coluna - 'A';
            linha1 = linha - 1;
        }
        
        for (int i = linha1 - 1; i <= linha1 + 1; i++) {
            for (int j = coluna1 - 1; j < coluna1 + 2; j++) {
                if (i >= 0 && i <= 9 && j >= 0 && j <= 9) mat[i][j] = -1;
            }
        }
        mat[linha1][coluna1] = 1;
    }
}

void atirar(int mat[5][5], int linha, int coluna) {
    if (mat[linha][coluna] == 0 || mat[linha][coluna] == -1) mat[linha][coluna] = -2;
    if (mat[linha][coluna] == 1) mat[linha][coluna] = 10;
}

int calculaPontuacao(int mat[5][5], int linha, int coluna) {
    if (mat[linha][coluna] == 10) {
        mat[linha][coluna] = 50;
        return 2;
    }
    return 0;
}

void funcMSN(int tamBarco, int i, int jogador) {
    if (tamBarco == 1) {
        printf("************************\n");
        printf("*\n");
        printf("* Jogador %d - Posicione o barco de tamanho 1 (%d/6)\n", jogador, i + 1);
        printf("*\n");
        printf("************************\n");
    }
}
