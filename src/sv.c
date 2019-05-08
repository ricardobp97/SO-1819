#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "headers/stock.h"
#include "headers/artigo.h"
#include "headers/venda.h"

int cliente;
char * token[2];

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
    Stock s = malloc(sizeof(Stock));
    s->codigoArt = c;
    s->quantidade = q;
    return s;
}

void insere_venda (int c, int q, float m) {
    char buf[50];
    int lido;
    lido=snprintf(buf,50,"%d %d %f\n",c,q,m);
    int fd = open("./files/vendas", O_CREAT | O_APPEND | O_WRONLY, 0600);
	write(fd,buf,lido);
    close(fd);
}

// quantidade negativa -> alterar stocks,
// verificar preco do artigo e inserir venda
int efetua_venda (int c, int q) {
    // Verificar Stock
    int r = -1, res;
    Stock novo;
    int fdS = open("./files/stocks", O_APPEND | O_WRONLY | O_RDONLY, 0600);
    lseek(fdS,c * sizeof(Stock),SEEK_SET);
    res = read(fdS,&novo, sizeof(Stock));
    if(res > 0){
        if(novo->quantidade > 0){
            int qvenda;
            if(novo->quantidade >= q){
                qvenda = q;
                novo->quantidade -= q;
                r = novo->quantidade;
            }
            else{
                qvenda = novo->quantidade;
                novo->quantidade = 0;
            }
            // Alterar Stock
            write(fdS,&novo, sizeof(Stock));
            // Verificar preco do artigo
            Artigo a;
            int fdA = open("./files/artigos", O_RDONLY, 0600);
            lseek(fdA,c * sizeof(Artigo),SEEK_SET);
            read(fdS,&a, sizeof(Artigo));
            int total = qvenda * a.preco;
            close(fdA);
            //free(a);
            // Registar venda
            insere_venda(c,qvenda,total);
        }
    }
    close(fdS);
    return r;
}

// quantidade positiva -> alterar stocks
int update_stock (int c, int q) {
    int r, res;
    Stock novo;
    int fd = open("./files/stocks", O_APPEND | O_WRONLY | O_RDONLY, 0600);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    res = read(fd,&novo, sizeof(Stock));
    if(res > 0){
        novo->quantidade += q;
        r = novo->quantidade;
        write(fd,&novo, sizeof(Stock));
        close(fd);
    }
    else{
        r = q;
        novo = new_stock(c,q);
        write(fd,&novo, sizeof(Stock));
        close(fd);
    }
    return r;
}

int getStock(int c){
    int r;
    Stock s;
    int fd = open("./files/stocks",O_RDONLY,0666);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    if(read(fd,&s,sizeof(Stock)) > 0)
        r = s->quantidade;
    else
        r = 0;
    close(fd);
    return r;
}

int getPrecoArt (int c) {
    int p;
    Artigo a;
    int fd = open("./files/artigos",O_RDONLY,0666);
    lseek(fd,c * sizeof(Artigo),SEEK_SET);
    read(fd,&a,sizeof(Artigo));
    p = a.preco;
    close(fd);
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

void processa_instrucao (char* s, char ** r) {
    int stock = 0;
    char * tok = strtok(s," ");
    if(tok != NULL){
        int c = atoi(tok);
        if(artigo_existe(c) > 0){
            tok = strtok(NULL," ");
            if(tok != NULL){
                int q = atoi(tok);
                if(q>0){
                    stock = update_stock(c,q);
                    snprintf(*r,16,"Novo Stock: %d\n",stock);
                }
                else{
                    stock = efetua_venda(c,-q);
                    if(stock < 0)
                        snprintf(*r,26,"Nao ha stock disponivel!\n");
                    else
                        snprintf(*r,16,"Novo Stock: %d\n",stock);
                }
            }
            else{
                // mostra no stdout stock e preco
                int preco = getPrecoArt(c);
                stock = getStock(c);
                snprintf(*r,21,"Stock: %d || Preco: %d\n",stock,preco);
            }
        }
        else{
            snprintf(*r,21,"Artigo não existe!\n");
        }
    }
    else{
        snprintf(*r,20,"Código Inválido!\n");
    }
    printf("R -> %s\n",*r);
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

    int res;
    char buffer[200];
    char *resposta;

    if(mkfifo("pipe", 0666) == -1){
        perror("pipe");
    }
    int pipe = open("pipe", O_RDONLY, 0666);

    while((res = read(pipe, buffer, 200)) > 0){
        token[0] = strtok(buffer, ":");
        token[1] = strtok(NULL, ":");
            if(*token[0] == 'C'){
                // Entrar no pipe do cliente
                cliente = open(token[1],O_WRONLY, 0666);
                if(cliente == -1) perror("erro abrir pipe");
                write(cliente,"Ligacao Estabelecida!\n",23);
            }
            else{
                if(token[0] != NULL){
                    // Seg Fault processa_instrucao!
                    processa_instrucao(token[1],&resposta);
                    printf("Resposta  %s", resposta);
                    write(cliente,resposta,26);
                }
            }
    }
    printf("Acabei\n");
    close(cliente);
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
