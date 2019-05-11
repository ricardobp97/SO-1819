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
//Lt listaCods = NULL;
size_t tamanhoDesperdicado = 0;
size_t tamanhoTotal = 0;
//off_t auxAlteraPos_x = 0;

/*
off_t tamanhoString(){
	int fd1 = open("./files/strings",O_RDONLY ,0666);
	off_t r = lseek(fd1,0,SEEK_END);
	return r;
}

Lt insereLt(Lt l, int cod){
	Lt aux = malloc(sizeof(Lt));
	aux->codigo = cod;
	aux->prox = l;
	
	return aux;
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
			}
	else { ant = l;
		   l = l->prox;
		 }
	}
	ant = aux;
	return aux;
}
*/

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
	tamanhoTotal += t;
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

/*
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
*/

void inserePos(off_t p){
	L l = malloc(sizeof(L));
	l->valor = p;
	l->prox = listaPosAntigas;
	listaPosAntigas = l;
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

void carregarLista(){
	int res;
	off_t p;
	int fd = open("./files/lista",O_CREAT | O_RDONLY,0666);
	while((res = read(fd, &p, sizeof(off_t))) > 0){
        inserePos(p);
    }
	close(fd);
	unlink("./files/lista");
}

void guardarLista(){
	int fd = open("./files/lista",O_CREAT | O_APPEND | O_WRONLY ,0666);
	L aux;
	off_t p;
	while(listaPosAntigas){
		p = listaPosAntigas->valor;
		write(fd,&p,sizeof(off_t));
		aux = listaPosAntigas;
		listaPosAntigas = listaPosAntigas->prox;
		free(aux);
	}
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
	inserePos(a.posicao);
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
		tamanhoTotal += t;
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

/*
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
	while(a.preco != -1 && (a.posicao < (pos))) {
		codigo++;
		a = alteraPosAux(codigo);
	}
	if (a.posicao == -1);
	else{
	a.posicao = a.posicao - (decremento + auxAlteraPos_x);
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
    lseek(fd1,codigo*tamanhoArtigoAux,SEEK_SET);
	write(fd1,&a,sizeof(Artigo));
	
	
	auxAlteraPos_x = auxAlteraPos_x + decremento;
	}
	close(fd1);

	return 1;
}
*/

/* versao nova:
void alteraPosAux(off_t pos){
	int fd1 = open("./files/artigos",O_RDONLY, 0666);
	Artigo a;
	int cod = 0;

	while(cod<codigoGLOBAL){
		read(fd1,&a,sizeof(Artigo));
		if (a.posicao>(pos)) insereLt(listaCods, a.codigo);
		cod++;
	}
	close(fd1);
	printf("sai");
}

Artigo alteraPost(int cod){

	int fd1 = open("./files/artigos",O_RDONLY ,0666);
	Artigo a;
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
	lseek(fd1,cod * tamanhoArtigoAux,SEEK_SET);
	read(fd1,&a,sizeof(Artigo));
	close(fd1);
	return a;


}

int alteraPos(off_t pos,off_t decremento) {
	int fd1 = open("./files/artigos",O_WRONLY, 0666);
	printf("entrei");
	Artigo a;
	alteraPosAux(pos);
	for(listaCods; listaCods!=NULL;listaCods = listaCods-> prox){

		printf("Lvalues : %d\n",listaCods->codigo);
	}
	while(listaCods != NULL){
		a = alteraPost(listaCods->codigo);
		printf("Sou a pos antiga %ld \n",a.posicao );
		a.posicao = a.posicao - decremento;
		printf("Sou a pos nova %ld \n",a.posicao );
	    ssize_t tamanhoArtigo = (a.codigo) * sizeof(Artigo);
    	int tamanhoArtigoAux = (int)tamanhoArtigo;
    	lseek(fd1,tamanhoArtigoAux,SEEK_SET);
		write(fd1,&a,sizeof(Artigo));
		listaCods = listaCods-> prox;
	}
		
	close(fd1);
	return 1;

}

*/ 


/*
int alteraPos(off_t pos, off_t decremento) {
	int fd1 = open("./files/artigos",O_WRONLY, 0666);
	Artigo a;
	a = alteraPosAux(pos);
	int codigo = a.codigo;
	a.posicao = a.posicao - (decremento + auxAlteraPos_x);
	ssize_t tamanhoArtigo = sizeof(Artigo);
    int tamanhoArtigoAux = (int)tamanhoArtigo;
    lseek(fd1,codigo*tamanhoArtigoAux,SEEK_SET);
	write(fd1,&a,sizeof(Artigo));
	auxAlteraPos_x = auxAlteraPos_x + decremento;
	close(fd1);

	return 1;
}
*/

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
/*
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

	for(listaPosAntigas; listaPosAntigas!=NULL;listaPosAntigas = listaPosAntigas-> prox){

		printf("Lvalues : %d\n",listaPosAntigas->valor);
	}

	while((read(fd1,&c,1))>0){
		if(existeL(listaPosAntigas,x)==1){
			off_t aux = 0;
			char aux2 = 'r';
			off_t aux3 = x;
			while((read(fd1,&aux2,1)>0) && (aux2 != ' ')){aux++;}
			listaPosAntigas = removeL(listaPosAntigas,x);
			x = x + aux + 2; 
			alteraPos(aux3,aux+2);
		}
		
		else {
			  write(fd2,&c,1);
			  x++;
		}
	}
    listaPosAntigas = NULL;

	close(fd1);
	close(fd2);
	//rename("./files/stringsv2","./files/strings");
}
*/
/*
void getString(off_t p, char** n){
	char c;
	int i = 0;
	int fd = open("./files/strings",O_RDONLY ,0666);
	lseek(fd,p,SEEK_SET);
	while( read(fd,&c,1) > 0 ){
		if(c == ' '){
			close(fd);
			break;
		}
		else{
			*n[i] = c;
			i++;
		}
	}
	close(fd);
}

void compactador(){
	int i;
	Artigo a;
	char* nome = malloc(1000 * sizeof(char));
	size_t t;
	for(i = 0; i < codigoGLOBAL; i++){
		int fd1 = open("./files/artigos",O_RDONLY ,0666);
		lseek(fd1,i * sizeof(Artigo),SEEK_SET);
		read(fd1, &a, sizeof(Artigo)));
		t = getStringTam(a.posicao,&nome);
    }
    close(fd);
}
*/
int main (){

	char buf[1000];
	char* out = malloc(25 * sizeof(char));
	carregarLista();
	carregarTamanhos();

	while(1){
		size_t h = readln(0,buf,sizeof(buf));
		if(h <= 0) break;
		verificaCodigoGLobal();
		/*
		if(buf[0] == 'a'){
			int fd = open("pidServ",O_CREAT | O_WRONLY,0666);
            pid_t x = read(fd,&x,sizeof(pid_t));
			kill(x,SIGUSR1);
			close(fd);
		}
		*/
		if(buf[0] == 'i'){
			char* token_nome;
			char* token_preco;
			token_nome = strtok(buf," ");
			token_nome = strtok(NULL," ");
			token_preco = strtok(NULL," ");
			int preco = atoi(token_preco);
			int c = insereArtigo(token_nome,preco);
			snprintf(out,25,"Código: %d\n",c);
			write(1,out,strlen(out));
		}
		if(buf[0] == 'p'){
			char* token_cod;
			char* token_preco;
			token_cod = strtok(buf," ");
			token_cod = strtok(NULL," ");
			token_preco = strtok(NULL," ");
			int cod = atoi(token_cod);		
			int preco = atoi(token_preco);
			alteraPreco(cod,preco);
		}
		if(buf[0] == 'n'){
			char* token_cod;
			char* token_nome;
			token_cod = strtok(buf," ");
			token_cod = strtok(NULL," ");
			token_nome = strtok(NULL," ");
			int cod = atoi(token_cod);		
			alteraNome(cod,token_nome);
			if(necessitaComp()){
				printf("Precisa!!!\n");
				//compactador();
			}
		}
	}
	free(out);
	guardarTamanhos();
	guardarLista();
	return 0;
}