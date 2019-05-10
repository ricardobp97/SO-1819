#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "headers/artigo.h"

int codigoGLOBAL = -1 ;
L listaPosAntigas = NULL;
off_t tamanhoDesperdicado;
off_t auxAlteraPos_x = 0;

off_t tamanhoString(){
	int fd1 = open("./files/strings",O_RDONLY ,0666);
	off_t r = lseek(fd1,0,SEEK_END);
	return r;
}

L insereL(L l, off_t x){
	L aux = malloc(sizeof(L));
	aux->valor = x;
	aux->prox = l;
	
	return aux;

}

int existeL(L l, off_t x){
	L aux = malloc(sizeof(L));
	aux = l;
	int r;
	if (l == NULL) return 0;
	else {
			 while(l!=NULL){
			 	if (l->valor == x){
			 		l= aux;
			 		return 1;

			 	} 
			 	else {
			 		  l = l->prox;
			 		 }
			}
	}
	l = aux;
}

L removeL(L l, off_t x){
	printf("removeL: entrei\n");
	L aux = malloc(sizeof(L));
	L ant = malloc(sizeof(L));
	aux = l;
	ant = l;
	int flag = 0;
	while(l && flag != 1){
		if (l->valor == x){
			ant->prox = l->prox;
			free(l);
			flag = 1;
			printf("removeL: removi\n");
			}
	else { ant = l;
		   l = l->prox;
		 }
	}
	ant = aux;
	return aux;
}

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
	return 1;
}

void calculaDesperdicio(int cod){
	int fd1 = open("./files/artigos",O_RDONLY ,0666);
	int fd2 = open("./files/strings",O_RDONLY ,0666);
	Artigo a;
	off_t aux;
	char c = 'r';
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
	lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
	read(fd1,&a,sizeof(Artigo));
	aux = a.posicao;
	lseek(fd2,aux,SEEK_SET);
	while((read(fd2,&c,1))&&(c != ' ')){
									tamanhoDesperdicado++;
									   }
	close(fd1);
	close(fd2);

}

Artigo alteraNomeAux(int cod){
	int fd1 = open("./files/artigos",O_RDONLY ,0666);
	Artigo a;
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
	lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
	read(fd1,&a,sizeof(Artigo));
	listaPosAntigas = insereL(listaPosAntigas,a.posicao);
	calculaDesperdicio(cod);
	close(fd1);
	return a;
		
}

int alteraNome(int cod, char* n) {
	if (cod > codigoGLOBAL) perror("Artigo inexistente");
	else {
		Artigo a;
		char c = ' ';
		int fd1 = open("./files/artigos",O_WRONLY, 0666);
		int fd2 = open("./files/strings",O_WRONLY| O_RDONLY, 0666);
		a = alteraNomeAux(cod);
		off_t posNova = lseek(fd2,0,SEEK_END);
		write(fd2,n,strlen(n));
		write(fd2,&c,1);
		a.posicao = posNova ;
		ssize_t tamanhoArtigo = sizeof(Artigo);
    	int tamanhoArtigoAux = (int)tamanhoArtigo;
    	lseek(fd1,cod*tamanhoArtigoAux,SEEK_SET);
		write(fd1,&a,sizeof(Artigo));
		close(fd1);
		close(fd2);
		}

	return 1;
}


Artigo alteraPosAux(int cod){
	Artigo a;
	if (cod > (codigoGLOBAL-1)){
							a.preco = -1;
							return a;
						    }
	else {	
			int fd1 = open("./files/artigos",O_RDONLY ,0666);
			ssize_t tamanhoArtigo = sizeof(Artigo);
    		int tamanhoArtigoAux = (int)tamanhoArtigo;
			lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
			read(fd1,&a,sizeof(Artigo));
			close(fd1);
			return a;
		 }
}

int alteraPos(off_t pos, off_t decremento) {
	int fd1 = open("./files/artigos",O_WRONLY, 0666);
	int codigo = 0;
	Artigo a;
	a = alteraPosAux(codigo);
	while(a.preco != -1 && a.posicao<pos) {
		codigo++;
		a = alteraPosAux(codigo);
	}
	a.posicao = a.posicao - (decremento + auxAlteraPos_x);
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
    lseek(fd1,codigo*tamanhoArtigoAux,SEEK_SET);
	write(fd1,&a,sizeof(Artigo));
	
	
	auxAlteraPos_x = auxAlteraPos_x + decremento;
	close(fd1);

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
			printf("Preco = %d \n ",a.preco);
			printf("\n");
		}
	close(fd1);
}


void compactador(){
	int fd1 = open("./files/strings",O_RDONLY ,0666);
	int fd2 = open("./files/stringsv2",O_CREAT | O_WRONLY ,0666);

	char c;
	off_t x = 0;

	while((read(fd1,&c,1))>0){
		if(existeL(listaPosAntigas,x)==1){
			off_t aux = 0;
			char aux2 = 'r';
			off_t aux3 = x;
			while((read(fd1,&aux2,1)>0) && (aux2 != ' ')){aux++;}
			listaPosAntigas = removeL(listaPosAntigas,x);
			x = x + aux + 2; 
			alteraPos(aux3,(aux+2));
		}
		
		else {
			  write(fd2,&c,1);
			  x++;
		}
	}

	close(fd1);
	close(fd2);
	rename("./files/stringsv2","./files/strings");
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
		
		
			compactador();
			interpretador();
			printf("tamanhoDesperdicado = %ld\n",tamanhoDesperdicado);


		}

	return 1;
}