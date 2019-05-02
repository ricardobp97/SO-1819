#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "headers/artigo.h"


int codigoGLOBAL = 0 ;

int iniciaFicheiros(){
	int fd1 = open("Artigos",O_CREAT,0666);
	int fd2 = open("Strings",O_CREAT,0666);
	return 1;
}


int insereArtigo(char* nome, float preco){

	char c = ' ';
	int fd1 = open("Artigos",O_WRONLY | O_RDONLY, 0666);
	int fd2 = open("Strings",O_WRONLY| O_RDONLY, 0666);
	off_t posAux = lseek(fd2,0,SEEK_END);
	Artigo aux = new_artigo(codigoGLOBAL,posAux,preco);
	write(fd1,aux,sizeof(struct Artigo));
	//free(aux);
	/* concatenar em vez de dois writes?
	char* nomeAux;
	my_concat(nome,c,nomeAux);
	write(fd2,nomeAux,strlen(nome)+1);
	*/
	write(fd2,nome,strlen(nome));
	write(fd2,&c,1);
	close(fd1);
	close(fd2);
	codigoGLOBAL++;
	return 0;
}

int alteraPreco(int cod, double pre){

	Artigo aux = new_artigoN();
	int fd1 = open("Artigos", O_WRONLY | O_RDONLY, 0666);
	off_t posAux = lseek(fd1,cod*sizeof(struct Artigo),SEEK_SET);
	read(fd1,aux,sizeof(struct Artigo));
	aux->preco = pre;
	lseek(fd1,cod*sizeof(struct Artigo),SEEK_SET);
	write(fd1,aux,sizeof(struct Artigo));
	close(fd1);
	return 0;
}

int alteraNome(int cod, char* n) {

	char c = ' ';
	int fd1 = open("Artigos",O_WRONLY | O_RDONLY, 0666);
	int fd2 = open("Strings",O_WRONLY| O_RDONLY, 0666);
	off_t posNova = lseek(fd2,0,SEEK_END);
	write(fd2,n,strlen(n));
	write(fd2,&c,1);
	Artigo aux = new_artigoN();
  lseek(fd1,cod*sizeof(struct Artigo),SEEK_SET);
	read(fd1,aux,sizeof(struct Artigo));
	aux->posicao = posNova;
	lseek(fd1,cod*sizeof(struct Artigo),SEEK_SET);
	write(fd1,aux,sizeof(struct Artigo));
	close(fd1);
	close(fd2);
	return 0;
}


int main (){

	if (iniciaFicheiros() != 1) perror("Falha a inicializar ficheiros");
	else {
			insereArtigo("bola",12.00);
			alteraPreco(0,23.00);
			alteraNome(0,"caneca");
			Artigo teste1 = new_artigoN();
			int fd1 = open("Artigos", O_RDONLY, 0666);
			read(fd1,teste1,sizeof(struct Artigo));
			printf("Codigo = %d \n ",teste1->codigo);
			printf("Posicao = %lld \n ",teste1->posicao);
			printf("Preco = %f \n ",teste1->preco);

			printf("\n");

			insereArtigo("cartao",15.00);
			alteraPreco(1,25.00);
			alteraNome(1,"plastico");
			Artigo teste2 = new_artigoN();
			read(fd1,teste2,sizeof(struct Artigo));
			printf("Codigo = %d \n ",teste2->codigo);
			printf("Posicao = %lld \n ",teste2->posicao);
			printf("Preco = %f \n ",teste2->preco);
		}

	return 0;
}
