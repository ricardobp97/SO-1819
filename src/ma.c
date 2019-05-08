#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "headers/artigo.h"

int codigoGLOBAL = -1 ;

int iniciaCodigoGLobal(){
	  off_t tamanho;
	  int fd1 = open("./files/artigos", O_RDONLY, 0666);
    tamanho = lseek(fd1,0,SEEK_END);
    int tamanhoAux = (int)tamanho;
    ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
    codigoGLOBAL = tamanhoAux/tamanhoArtigoAux;
    return 0;
}

int iniciaFicheiros(){
	open("./files/artigos",O_CREAT,0666);
	open("./files/strings",O_CREAT,0666);
	return 0;
}

int insereArtigo(char* nome, int preco){
	char c = ' ';
	int fd1 = open("./files/artigos",O_WRONLY | O_RDONLY, 0666);
	int fd2 = open("./files/strings",O_WRONLY| O_RDONLY, 0666);
	lseek(fd1,0,SEEK_END);
	off_t posAux = lseek(fd2,0,SEEK_END);
	Artigo aux = novo_Artigo(codigoGLOBAL,posAux,preco);
	write(fd1,&aux,sizeof(Artigo));
	write(fd2,nome,strlen(nome));
	write(fd2,&c,1);
	close(fd1);
	close(fd2);
	codigoGLOBAL++;
	return 0;
}

Artigo alteraPrecoAux (int cod){
	int fd1 = open("./files/artigos",O_RDONLY ,0666);
    Artigo a;
    ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
	lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
	read(fd1,&a,sizeof(Artigo));
	close(fd1);
	return a;
}


int alteraPreco(int cod,int pre){
	if (cod > codigoGLOBAL) perror("Artigo inexistente");
	else {
		int fd1 = open("./files/artigos",O_WRONLY ,0666);
		Artigo a;
		a = alteraPrecoAux(cod);
		a.preco=pre;
		ssize_t tamanhoArtigo = sizeof(Artigo);
    	int tamanhoArtigoAux = (int)tamanhoArtigo;
    	lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
    	write(fd1,&a,sizeof(Artigo));
    	close(fd1);
		}
	return 0;
}


Artigo alteraNomeAux(int cod){
	int fd1 = open("./files/artigos",O_RDONLY ,0666);
	Artigo a;
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
	lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
	read(fd1,&a,sizeof(Artigo));
	close(fd1);
	return a;

}



int alteraNome(int cod, char* n) {
	if (cod > codigoGLOBAL) perror("Artigo inexistente");
	else {
		Artigo a;
		char c = ' ';
		int fd1 = open("./files/artigos",O_RDONLY, 0666);
		int fd2 = open("./files/strings",O_WRONLY| O_RDONLY, 0666);
		a = alteraNomeAux(cod);
		off_t posNova = lseek(fd2,0,SEEK_END);
		write(fd2,n,strlen(n));
		write(fd2,&c,1);
		a.posicao = posNova;
    	lseek(fd1,cod*sizeof(Artigo),SEEK_SET);
		write(fd1,&a,sizeof(Artigo));
		close(fd1);
		close(fd2);
		}
	return 0;
}

ssize_t readln(int fildes, void *buf, size_t nbyte) {

	char* b = buf;
	int i = 0;

	while(i < nbyte) {
		int n = read(fildes, &b[i],1);
		if(n <= 0)
			break;
		if(b[i] == '\n') {
			b[i] = '\n';
			i++;
			break;
		}
		i++;
	}
	return i;
}

int interpretador(){

	int fd1 = open("./files/artigos",O_RDONLY,0666);
	Artigo a;
  int file=open("logArtigos",O_CREAT |O_WRONLY,0666);
	while (read(fd1,&a,sizeof(Artigo))){
      char buf[200];
      int escrito=snprintf(buf,200,"Código= %d\n Posicao= %lld\n Preco= %d\n",a.codigo,a.posicao,a.preco);
      write(file,buf,escrito);
		}
	close(fd1);
  return 0;
}



int main (int argc, char* argv[]){

	if (iniciaFicheiros() != 0) perror("Falha a inicializar ficheiros");
	else {	iniciaCodigoGLobal();
			char buf[500];

			while(1) {
				int h = readln(0,buf,sizeof(buf));
				if(h <= 0) break;

				if (buf[0] == 'i'){

          char nome[100];
          char l[1];
          int preco;
          sscanf(buf,"%s %s %d\n",l,nome,&preco);
					insereArtigo(nome,preco);

				}

				if (buf[0] == 'p'){

          int c;
          char l[1];
          int preco;
          sscanf(buf,"%s %d %d\n",l,&c,&preco);
					alteraPreco(c,preco);

				}

				if (buf[0] == 'n'){

          int c;
          char nome[100];
          char l[1];
          sscanf(buf,"%s %d %s\n",l,&c,nome);
					alteraNome(c,nome);

				}
			}

			interpretador();
		}

	return 1;
}
