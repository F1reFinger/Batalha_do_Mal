#include <stdio.h>
#include "ships.h"

int main(){
	int player2[5][5]={0,0,0,0,0,
						 0,0,0,0,0,
						 0,0,0,0,0,
						 0,0,0,0,0,
						 0,0,0,0,0};
    //posicionamento de barcos PLAYER 1
    for(int i=0;i<4;i++){
        funcMSN(1,i,1);
        imprimeTabuleiro(player2,0);
        posicionaBarco(player2,1);
    }
    //agora a etapa de atirar;
		//Etapa 1;
		int jogadas=0,pontant=0,pontant1=0,glauber=0,pos=0,kleber=0,pts1=0;
		char L;
		while(jogadas!=41){
                if(pts1==42){
                    break;
                }
                if(jogadas==40){
                    break;
                }

			printf("************************\n");
	 		printf("*\n");
	 		printf("* PONTUACAO DO JOGADOR 1: %d\n",pts1);
	 		printf("*\n");
	 		printf("************************\n");
			printf("************************\n");
	 		printf("*\n");
	 		printf("* VEZ DO JOGADOR 1\n");
	 		printf("*\n");
	 		printf("************************\n");
			imprimeTabuleiro(player2,1);
			scanf("%d %c",&pos,&L);
			kleber=L-65;
			pos=pos-1;
			while(validaEntradaLinhaColuna(pos,L)==0||podeAtirar(player2,pos,kleber)==0){
				printf("Posicao indisponivel!\n");
				scanf("%d %c",&pos,&L);
				kleber=L-65;
				pos=pos-1;
			}
			atirar(player2,pos,kleber);
			pts1+=calculaPontuacao(player2,pos,kleber);
			if(pontant<pts1){
				printf("************************\n");
	 			printf("*\n");
	 			printf("* JOGADOR 1 DERRUBOU UM NAVIO!\n");
	 			printf("*\n");
	 			printf("************************\n");
				printf("\n");
				pontant=pts1;
			}
		jogadas++;

		}

		printf("************************\n");
	 	printf("*\n");
	 	printf("* FIM DE JOGO\n");
	 	printf("*\n");
	 	printf("************************\n");
		printf("************************\n");
	 	printf("*\n");
	 	printf("* PONTUACAO DO JOGADOR : %d\n",pts1);
	 	printf("*\n");
	 	printf("************************\n");
	return 0;
}
//eu nao aguento maiss aaaaaa;