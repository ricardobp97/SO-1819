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
#include <signal.h>
#include <sys/types.h>
#include "headers/stock.h"
#include "headers/artigo.h"
#include "headers/venda.h"


char *token[2];
int lido;

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
    int fd = open("./files/stocks", O_RDONLY, 0666);
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
    int fd = open("./files/stocks", O_WRONLY, 0666);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    write(fd,&s,sizeof(Stock));
    close(fd);
}

void insere_venda (int c, int q, int m) {
    char buf[50];
    int lido;
    lido=snprintf(buf,50,"%d %d %d\n",c,q,m);
    int fd = open("./files/vendas", O_CREAT| O_APPEND | O_WRONLY, 0666);
	  write(fd,buf,lido);
    close(fd);
}

// quantidade negativa -> alterar stocks,
// verificar preco do artigo e inserir venda
int efetua_venda (int c, int q) {
    int r = -1, res;
    Stock novo;
    int fdS = open("./files/stocks", O_CREAT| O_RDONLY, 0666);
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
    int fd = open("./files/stocks", O_RDONLY, 0666);
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

int readln(int fildes, char *buf, int maxBytes){
  char byte;
  int i = 0;
  int res;
  while (i < maxBytes && (res = read(fildes,&byte,1)) != 0){
    if (byte != '\n'){
      buf[i] = byte;
      i += res;
    }
    else {
        buf[i++] = '\n';
        return i;
    }
  }
  return i;
}

void agrega(int signum){

  pid_t pidW=0;

  char* data = getTime();
    int fd = open("./files/vendas",O_RDONLY,0666);
    int size = lseek(fd,0,SEEK_END);
    close(fd);
    int numPipes=4;


    // sub agregadores escrevem para aqui
    // agregador final lê de aqui
    int fifo = mkfifo("subagregacoes",0666);
    if (fifo < 0)
      perror("Could not create a FIFO!");


    // cria agregador de agregadores
    // lê do named pipe e escreve no ficheiro com a data
    int ff=fork();
    if(ff==0){
      pidW=getpid();
      int std= open("subagregacoes",O_RDONLY);
      dup2(std,0);
      close(std);

      int stdOut = open(data,O_CREAT | O_WRONLY, 0666);
      dup2(stdOut,1);
      close(stdOut);
      execlp("./ag","./ag",NULL);
      _exit(-1);
    }


    int pipeToChild[numPipes][2];
    for(int i = 0; i < numPipes; i++) {
      pipe(pipeToChild[i]);
    }


  // CRIAR OS AGREGADORES INTERMÉDIOS
   int pid = fork();
    // filho cria sub-agregadores
    if (pid == 0) {
      for(int i = 0; i < numPipes; i++) {
        int agregador = fork();
        if (agregador == 0) {
          // redirecionamento do pipe que liga ao pai
          dup2(pipeToChild[i][0],0);
          for(int j=0;j<numPipes;j++){
            close(pipeToChild[j][0]);
            close(pipeToChild[j][1]);
          }

         int stout = open("subagregacoes", O_WRONLY);
          if (stout == -1)
              perror("open subagregacoes");
          dup2(stout,1);
          close(stout);
          if (execlp("./ag","./ag",NULL) == -1)
            perror("execlp");
          _exit(-1);
        }
      }
    }

    // PAI ESCREVE PARA OS AGREGADORES INTERMEDIOS
    else {
      // unused read of pipe
      for(int i = 0; i < numPipes; i++) {
        close(pipeToChild[i][0]);
      }
      int vendas_fd = open("./files/vendas",O_RDONLY,0666);
      lseek(vendas_fd,lido,SEEK_SET);
      char buf[500];
      int currentPipe=0;
      int n;

      while((n=readln(vendas_fd,buf,500))>0){
          write(pipeToChild[currentPipe][1],buf,n);
          currentPipe=(currentPipe+1)%(numPipes);
      }
      close(vendas_fd);
      for(int i = 0; i < numPipes; i++) {
        close(pipeToChild[i][1]);
      }
      lido=size;
    }
  }


int length(char * s) {
    int i;
    for(i=0 ; s[i]!='\0'; i++);
    return i;
}

int main() {

    pid_t pid=getpid();
    int d;
    int fd=open("pidServ",O_CREAT | O_WRONLY,0666);
    write(fd,&pid,sizeof(pid));
    close(fd);
    fd=open("pidServ",O_CREAT | O_RDONLY,0666);
    read(fd,&d,sizeof(d));
    printf("%d\n",d );
    close(fd);

    if( signal(SIGUSR1,agrega) == SIG_ERR){
      perror("Signal failed");
    }



  /*
    int i, f, c, status;
    int res;
    char *nome = malloc(12 * sizeof(char));
    char **pt;
    char buffer[200];
    for(i=0; i<11; i++){
        // 1 pipe por cada filho!
        //snprintf(nomes[i],12,"%d",i);
        pt = &nome;
        snprintf(*pt,12,"%d",i);
        if(mkfifo(nome, 0666) == -1){
            perror("pipes filhos");
        }
        f = fork();
        // codigo de cada filho!!
        if(f == 0){
            int n, cliente;
            int pp = open(nome, O_RDONLY, 0666);
            char buf[100];
            char *tok[2];
            char *resposta = malloc(50 * sizeof(char));
            char **ptr = &resposta;
            while((n = read(pp, buf, 100)) > 0){
                tok[0] = strtok(buf, ":");
                tok[1] = strtok(NULL, ":");
                printf("2 -> %s:%s\n",tok[0],tok[1]);
                cliente = open(tok[0],O_WRONLY, 0666);
                if(cliente == -1) perror("erro abrir pipe do cliente");
                processa_instrucao(tok[1],ptr);
                write(cliente,resposta,length(resposta));
                close(cliente);
            }
            close(pp);
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
        snprintf(*pt,res,"%s:%s",token[0],token[1]);
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
                write(filho,*pt,res);
                close(filho);
            }
    }

    for(i=0; i<11; i++){
        wait(&status);
    }
    close(pipe);
    unlink("./pipe");

*/
/*
  insere_venda(1000,1000,1000);
  insere_venda(1000,1000,1000);
  insere_venda(1000,1000,1000);
  insere_venda(1000,1000,1000);
  insere_venda(1000,1000,1000);
  insere_venda(1000,1000,1000);
  insere_venda(1000,1000,1000);

  lido=0;
  agrega();
  */

    return 0;
}
