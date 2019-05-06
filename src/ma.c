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
}

int iniciaFicheiros(){
	open("./files/artigos",O_CREAT,0666);
	open("./files/strings",O_CREAT,0666);
	return 1;
}

int insereArtigo(char* nome, double preco){
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
	return 1;
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


int alteraPreco(int cod,double pre){
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
	return 1;
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
	return 1;
}

ssize_t readln(int fildes, void *buf, size_t nbyte) {

	char* b = buf;

	int i = 0; 

	while(i < nbyte) {

		int n = read(fildes, &b[i],1);

		if(n <= 0) 
			break;

		if(b[i] == '\n') {
			b[i] = '\0';
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
	while (read(fd1,&a,sizeof(Artigo))){
			printf("Codigo = %d \n ",a.codigo);
			printf("Posicao = %jd \n ",a.posicao);
			printf("Preco = %f \n ",a.preco);
			printf("\n");
		}
	close(fd1);
}



int main (int argc, char* argv[]){

	if (iniciaFicheiros() != 1) perror("Falha a inicializar ficheiros");
	else {	iniciaCodigoGLobal(); 
			int fd = open(argv[1],O_RDONLY,0666);
			char buf[1000];

			while(1) {
				size_t h = readln(fd,buf,sizeof(buf));
				if(h <= 0) break;

				if (buf[0] == 'i'){
					
				    char* token_inst;
					char* token_nome;
					char* token_preco;
					char* token_lixo;

					token_inst = strtok(buf," ");
					token_nome = strtok(NULL," ");
					token_preco = strtok(NULL," ");
					token_lixo = strtok(NULL," ");

									
					double preco = strtod(token_preco,NULL);
					insereArtigo(token_nome,preco);
					
				}

				if (buf[0] == 'p'){
					
				    char* token_inst;
					char* token_cod;
					char* token_preco;
					char* token_lixo;

					token_inst = strtok(buf," ");
					token_cod = strtok(NULL," ");
					token_preco = strtok(NULL," ");
					token_lixo = strtok(NULL," ");

					int cod = atoi(token_cod);		
					double preco = strtod(token_preco,NULL);
					alteraPreco(cod,preco);
					
				}

				if (buf[0] == 'n'){
					
				    char* token_inst;
					char* token_cod;
					char* token_nome;
					char* token_lixo;

					token_inst = strtok(buf," ");
					token_cod = strtok(NULL," ");
					token_nome = strtok(NULL," ");
					token_lixo = strtok(NULL," ");

					int cod = atoi(token_cod);		
					alteraNome(cod,token_nome);
					
				}




			}
		
			
			
		
			interpretador();
		}

	return 1;
}