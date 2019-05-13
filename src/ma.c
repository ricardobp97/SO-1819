#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <ctype.h>
#include "headers/artigo.h"

int codigoGLOBAL = -1 ;
size_t tamanhoDesperdicado = 0;
size_t tamanhoTotal = 0;

void verificaCodigoGLobal(){
	off_t tamanho;
	int fd1 = open("./files/artigos", O_CREAT | O_RDONLY, 0666);
    tamanho = lseek(fd1,0,SEEK_END);
    int tamanhoAux = (int)tamanho;
    ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
    codigoGLOBAL = tamanhoAux/tamanhoArtigoAux;
}


int insereArtigo(char* nome, int preco){
	char c = ' ';
	int fd1 = open("./files/artigos",O_CREAT | O_WRONLY | O_RDONLY, 0666);
	int fd2 = open("./files/strings",O_CREAT | O_WRONLY | O_RDONLY, 0666);
	lseek(fd1,0,SEEK_END);
	off_t posAux = lseek(fd2,0,SEEK_END);
	Artigo aux = novo_Artigo(codigoGLOBAL,posAux,preco);
	write(fd1,&aux,sizeof(Artigo));
	size_t t = strlen(nome);
	write(fd2,nome,t);
	write(fd2,&c,1);
	close(fd1);
	close(fd2);
	codigoGLOBAL++;
	tamanhoTotal += (t+1);
	return (codigoGLOBAL - 1);
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


void alteraPreco(int cod,int pre){
	if(cod >= codigoGLOBAL) write(1,"Artigo inexistente!\n",21);
	else{
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
}

void carregarTamanhos(){
	int fd = open("./files/tamanho",O_CREAT | O_RDONLY,0666);
	read(fd,&tamanhoDesperdicado, sizeof(size_t));
	read(fd,&tamanhoTotal, sizeof(size_t));
	close(fd);
	unlink("./files/tamanho");
}

void guardarTamanhos(){
	int fd = open("./files/tamanho",O_CREAT | O_APPEND | O_WRONLY ,0666);
	write(fd,&tamanhoDesperdicado,sizeof(size_t));
	write(fd,&tamanhoTotal,sizeof(size_t));
	close(fd);
}

void atualizaTamDes(off_t p){
	size_t t = 0;
	char a;
	int fd = open("./files/strings",O_RDONLY ,0666);
	lseek(fd,p,SEEK_SET);
	while(a!= ' '){
		t += read(fd, &a, sizeof(char));
	}
	close(fd);
	tamanhoDesperdicado += t;
}

Artigo alteraNomeAux(int cod){
	int fd1 = open("./files/artigos",O_RDONLY ,0666);
	Artigo a;
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
	lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
	read(fd1,&a,sizeof(Artigo));
	atualizaTamDes(a.posicao);
	//calculaDesperdicio(cod);
	close(fd1);
	return a;
}

void alteraNome(int cod, char* n) {
	if(cod >= codigoGLOBAL) write(1,"Artigo inexistente!\n",21);
	else{
		Artigo a;
		char c = ' ';
		int fd1 = open("./files/artigos",O_WRONLY, 0666);
		int fd2 = open("./files/strings",O_WRONLY| O_RDONLY, 0666);
		a = alteraNomeAux(cod);
		off_t posNova = lseek(fd2,0,SEEK_END);
		size_t t = strlen(n);
		write(fd2,n,t);
		write(fd2,&c,1);
		tamanhoTotal += (t+1);
		a.posicao = posNova;
		ssize_t tamanhoArtigo = sizeof(Artigo);
    	int tamanhoArtigoAux = (int)tamanhoArtigo;
    	lseek(fd1,cod*tamanhoArtigoAux,SEEK_SET);
		write(fd1,&a,sizeof(Artigo));
		close(fd1);
		close(fd2);
	}
}

int necessitaComp(){
	double r = ((double)(tamanhoDesperdicado)) / ((double)(tamanhoTotal));
	if(r >= 0.2 ) return 1;
	else return 0;
}

ssize_t readln(int fildes, void *buf, size_t nbyte) {
	char* b = buf;
	int i = 0; 

	while(i < nbyte) {
		int n = read(fildes, &b[i],1);
		if(n <= 0) break;
		if(b[i] == '\n'){
			b[i] = '\0';
			i++;
			break;
		}
		i++;
	}
	return i;
}

void getString(off_t p, char **pt){
	char* nome = malloc(200 * sizeof(char));
	int res, i = 0;
	int fd = open("./files/strings",O_RDONLY ,0666);
	lseek(fd,p,SEEK_SET);
	while(1){
		res = read(fd,&nome[i],1);
		if(res <= 0){
			close(fd);
			break;
		}
		if(nome[i] == ' '){
			i++;
			nome[i] = '\0';
			close(fd);
			*pt = nome;
			break;
		}
		i++;
	}
}

void atualizaArtigo(int cod, off_t pos, int pre){
	Artigo a = novo_Artigo(cod,pos,pre);
	int fd = fd = open("./files/artigos",O_WRONLY ,0666);
	lseek(fd,cod * sizeof(Artigo),SEEK_SET);
	write(fd,&a,sizeof(Artigo));
	close(fd);
}

void compactador(){
	tamanhoTotal = 0;
	tamanhoDesperdicado = 0;
	int i, fd, fd2;
	off_t pos;
	size_t t;
	Artigo a;
	char* nome = malloc(200 * sizeof(char));

	for(i = 0; i < codigoGLOBAL; i++){
		// Ir buscar o artigo i
		fd = open("./files/artigos",O_RDONLY ,0666);
		lseek(fd,i * sizeof(Artigo),SEEK_SET);
		read(fd, &a, sizeof(Artigo));
		close(fd);
		// Ir buscar o nome do artigo
		getString(a.posicao,&nome);
		// Criar um novo ficheiro strings
		// Escrever no novo apenas as strings atualizadas
		fd2 = open("./files/stringsv2",O_CREAT | O_WRONLY ,0666);
		pos = lseek(fd2,0,SEEK_END);
		t = strlen(nome);
		write(fd2,nome,t);
		close(fd2);
		//write(fd2,&c,1);
		tamanhoTotal += t;
		// Atualizar posicao no artigo
		atualizaArtigo(a.codigo,pos,a.preco);
	}
	unlink("./files/strings");
	rename("./files/stringsv2","./files/strings");
}

int isDidigt(char * s) {
    int i;
    for(i=0; s[i]!='\0'; i++){
        if(!isdigit(s[i]))
            return 0;
    }
    return 1;
}

int main (){
	int fd;
	pid_t x;
	char buf[1000];
	char* out = malloc(25 * sizeof(char));
	carregarTamanhos();

	while(1){
		size_t h = readln(0,buf,sizeof(buf));
		if(h <= 0) break;
		verificaCodigoGLobal();
		if((buf[0] == 'a') && (buf[1] == ' ')){
			fd = open("pidServ",O_RDONLY,0666);
            x = read(fd,&x,sizeof(pid_t));
			kill(x,SIGUSR1);
			close(fd);
		}
		if((buf[0] == 'i') && (buf[1] == ' ')){
			char* token_nome;
			char* token_preco;
			token_nome = strtok(buf," ");
			token_nome = strtok(NULL," ");
			token_preco = strtok(NULL," ");
			if(isDidigt(token_preco)){
				int preco = atoi(token_preco);
				int c = insereArtigo(token_nome,preco);
				snprintf(out,25,"Código: %d\n",c);
				write(1,out,strlen(out));
			}
		}
		if((buf[0] == 'p') && (buf[1] == ' ')){
			char* token_cod;
			char* token_preco;
			token_cod = strtok(buf," ");
			token_cod = strtok(NULL," ");
			token_preco = strtok(NULL," ");
			if(isDidigt(token_cod) && isDidigt(token_preco)){
				int cod = atoi(token_cod);		
				int preco = atoi(token_preco);
				alteraPreco(cod,preco);
			}
		}
		if((buf[0] == 'n') && (buf[1] == ' ')){
			char* token_cod;
			char* token_nome;
			token_cod = strtok(buf," ");
			token_cod = strtok(NULL," ");
			token_nome = strtok(NULL," ");
			if(isDidigt(token_cod)){
				int cod = atoi(token_cod);
				alteraNome(cod,token_nome);
				if(necessitaComp()){
					compactador();
				}
			}
		}
	}
	free(out);
	guardarTamanhos();
	fd = open("pidServ",O_RDONLY,0666);
	if(fd >= 0){
    	pid_t x = read(fd,&x,sizeof(pid_t));
		kill(x,SIGUSR2);
		close(fd);
	}
	return 0;
}