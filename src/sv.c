#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "headers/stock.h"
#include "headers/artigo.h"
#include "headers/venda.h"

char* getTime(){
    time_t rawtime;
    struct tm * tf;
  
    time ( &rawtime );
    static char tempo[60];
    tf = localtime ( &rawtime );
    snprintf(tempo,60,"%d-%d-%dT%d-%d-%d",tf->tm_year+1900,tf->tm_mon,tf->tm_mday,tf->tm_hour,tf->tm_min,tf->tm_sec);
    return tempo;
}

Stock new_stock (int c, int q) {
    return(Stock){
		.codigoArt = c,
		.quantidade = q
	};
}

int getStock(int c){
    int r, res;
    Stock s;
    int fd = open("./files/stocks", O_RDONLY, 0600);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    res = read(fd,&s, sizeof(Stock));
    close(fd);
    if(res > 0){
        r = s.quantidade;
    }
    else{
        r = 0;
    }
    return r;
}

int getPrecoArt (int c) {
    int p;
    Artigo a;
    int fd = open("./files/artigos",O_RDONLY,0666);
    lseek(fd,c * sizeof(Artigo),SEEK_SET);
    read(fd,&a,sizeof(Artigo));
    close(fd);
    p = a.preco;
    return p;
}

int artigo_existe(int c) {
    int r;
    Artigo a;
    int fd = open("./files/artigos",O_RDONLY,0666);
    lseek(fd,c * sizeof(Artigo),SEEK_SET);
    r = read(fd,&a,sizeof(Artigo));
    close(fd);
    return r;
}

int isDidigt(char * s) {
    int i;
    for(i=0; s[i]!='\0'; i++){
        if(!isdigit(s[i]))
            return 0;
    }
    return 1;
}

void set_stock(int c, int q){
    Stock s = new_stock(c,q);
    int fd = open("./files/stocks", O_WRONLY, 0600);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    write(fd,&s,sizeof(Stock));
    close(fd);
}

void insere_venda (int c, int q, int m) {
    char buf[50];
    int lido;
    lido=snprintf(buf,50,"%d %d %d\n",c,q,m);
    int fd = open("./files/vendas", O_APPEND | O_WRONLY, 0600);
	write(fd,buf,lido);
    close(fd);
}

// quantidade negativa -> alterar stocks,
// verificar preco do artigo e inserir venda
int efetua_venda (int c, int q) {
    int r = -1, res;
    Stock novo;
    int fdS = open("./files/stocks", O_RDONLY, 0600);
    lseek(fdS,c * sizeof(Stock),SEEK_SET);
    res = read(fdS,&novo, sizeof(Stock));
    close(fdS);
    if(res > 0){
        if(novo.quantidade > 0){
            int qvenda;
            if(novo.quantidade >= q){
                qvenda = q;
                r = novo.quantidade - q;
            }
            else{
                qvenda = novo.quantidade;
                r = 0;
            }
            set_stock(c,r);
            int total = qvenda * getPrecoArt(c);
            insere_venda(c,qvenda,total);
        }
    }
    return r;
}

// quantidade positiva -> alterar stocks
int update_stock (int c, int q) {
    int r, res;
    Stock novo;
    int fd = open("./files/stocks", O_RDONLY, 0600);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    res = read(fd,&novo, sizeof(Stock));
    close(fd);
    if(res > 0){
        r = novo.quantidade + q;
        set_stock(c,r);
    }
    else{
        r = q;
        set_stock(c,r);
    }
    return r;
}

void processa_instrucao (char* s, char** pt) {
    int stock = 0;
    char * tok = strtok(s," ");
    if(tok != NULL && isDidigt(tok)){
        int c = atoi(tok);
        if(artigo_existe(c) > 0){
            tok = strtok(NULL," ");
            if(tok != NULL){
                int q = atoi(tok);
                if(q>0){
                    stock = update_stock(c,q);
                    snprintf(*pt,25,"Novo Stock: %d\n",stock);
                }
                else{
                    printf("q  %d\n", q);
                    stock = efetua_venda(c,-q);
                    if(stock < 0)
                        snprintf(*pt,26,"Nao ha stock disponivel!\n");
                    else
                        snprintf(*pt,25,"Novo Stock: %d\n",stock);
                }
            }
            else{
                // mostra no stdout stock e preco
                int preco = getPrecoArt(c);
                stock = getStock(c);
                snprintf(*pt,30,"Stock: %d || Preco: %d\n",stock,preco);
            }
        }
        else{
            snprintf(*pt,21,"Artigo não existe!\n");
        }
    }
    else{
        snprintf(*pt,20,"Código Inválido!\n");
    }
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

void agrega(){

  char* lista[4]={"test1","test2","test3","test4"};
  int fd= open("./files/vendas",O_RDONLY,0600);
  int size=lseek(fd,0,SEEK_END);
  int v=size/4;
  close(fd);


  char buf[50];
  int n;
  pid_t pid1, pid2;
  mkfifo("porfavor",0666);


      for(int i=0;i<4;i++){
        pid1=fork();

        if(pid1==0){
          char nameOfFile[4];
          snprintf(nameOfFile,4,"%d",i);
          int rest= mkfifo(nameOfFile,0666);
          printf("%d\n",rest );

          pid2=fork();
          if(pid2==0){
            execlp("./ag","./ag",lista[i],nameOfFile,NULL);
            _exit(-1);
          }

          int res=open(nameOfFile,O_WRONLY);

          int lido=0;
          fd= open("./files/vendas",O_RDONLY,0600);
          lseek(fd,i*v,SEEK_SET);
          while(lido<v){
              n=readln(fd,buf,50);
              lido+=n;
              write(res,buf,n);
          }
            close(res);
            _exit(0);
          }
      }
      int op=open("porfavor",O_RDONLY);
      char baba[200];
      int nn;
      while((nn=readln(op,baba,200))>0){
        write(1,baba,nn);
      }
}

int length(char * s) {
    int i;
    for(i=0 ; s[i]!='\0'; i++);
    return i;
}

int main() {
    int i, f, c, status;
    int res;
    char * token[2];
    char *nomes[11];
    char buffer[200];

    for(i=0; i<11; i++){
        // 1 pipe por cada filho!
        snprintf(nomes[i],4,"%d",i);
        if(mkfifo(nomes[i], 0666) == -1){
            perror("pipes filhos");
        }
        f = fork();
        // codigo de cada filho!!
        if(f == 0){
            int n, cliente;
            int pp = open(nomes[i], O_RDONLY, 0666);
            char buf[100];
            char *tok[2];
            char *resposta = malloc(50 * sizeof(char));
            char ** pt = &resposta;
            while((n = read(pp, buf, 100)) > 0){
                tok[0] = strtok(buffer, ":");
                tok[1] = strtok(NULL, ":");
                cliente = open(tok[0],O_WRONLY, 0666);
                if(cliente == -1) perror("erro abrir pipe do cliente");
                processa_instrucao(tok[1],pt);
                write(cliente,resposta,length(resposta));
                close(cliente);
            }
            close(pp);
            free(pt);
            _exit(i);
        }
    }
    // Codigo pai
    if(mkfifo("pipe", 0666) == -1){
        perror("pipe");
    }
    int pipe = open("pipe", O_RDONLY, 0666);
    
    int filho;
    while((res = read(pipe, buffer, 200)) > 0){
        token[0] = strtok(buffer, ":");
        token[1] = strtok(NULL, ":");
            if(token[0] && token[1]){
                c = atoi(strtok(token[1]," "));
                if(0 <= c && c < 10) filho = open("0", O_APPEND || O_WRONLY, 0666);
                if(11 <= c && c < 20) filho = open("1", O_APPEND || O_WRONLY, 0666);
                if(21 <= c && c < 30) filho = open("2", O_APPEND || O_WRONLY, 0666);
                if(31 <= c && c < 40) filho = open("3", O_APPEND || O_WRONLY, 0666);
                if(41 <= c && c < 50) filho = open("4", O_APPEND || O_WRONLY, 0666);
                if(51 <= c && c < 60) filho = open("5", O_APPEND || O_WRONLY, 0666);
                if(61 <= c && c < 70) filho = open("6", O_APPEND || O_WRONLY, 0666);
                if(71 <= c && c < 80) filho = open("7", O_APPEND || O_WRONLY, 0666);
                if(81 <= c && c < 90) filho = open("8", O_APPEND || O_WRONLY, 0666);
                if(91 <= c && c < 100) filho = open("9", O_APPEND || O_WRONLY, 0666);
                if(c >= 100) filho = open("10", O_APPEND || O_WRONLY, 0666);
                write(filho,buffer,res);
                close(filho);
            }
    }

    for(i=0; i<11; i++){
        wait(&status);
    }
    close(pipe);
    unlink("./pipe");

/*
  insere_venda(2,2,10.1);
  insere_venda(2,2,10.1);
  insere_venda(2,2,10.1);
  insere_venda(2,2,10.1);
  insere_venda(2,2,10.1);
  insere_venda(2,2,10.1);
  insere_venda(2,2,10.1);
  insere_venda(2,2,10.1);

  agrega();
*/
    return 0;
}
