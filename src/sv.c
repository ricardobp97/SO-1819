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

#define SIZELINE 256

void end(int signum){
  unlink("pipeServ");
  unlink("subagregacoes");
  kill(getpid(),SIGKILL);
}


char *token[2];
int lido = 0;
Artigo cache[10];
int posCache = 0;
int numElemCache = 0;

char* getTime(){
    time_t rawtime;
    struct tm * tf;

    time ( &rawtime );
    static char tempo[60];
    tf = localtime ( &rawtime );
    sprintf(tempo,"%d-%d-%dT%d-%d-%d",tf->tm_year+1900,tf->tm_mon,tf->tm_mday,tf->tm_hour,tf->tm_min,tf->tm_sec);
    return tempo;
}

Stock new_stock (int c, int q) {
    return(Stock){
		.codigoArt = c,
		.quantidade = q
	};
}


int getFromCache(int cod){
    int i;
    for(i=0; i<numElemCache; i++){
        if(cache[i].codigo == cod)
            return cache[i].preco;
    }
    return -1;
}

void atualizaCache(){
    int i, fd, cod;
    Artigo a;
    fd = open("./files/artigos",O_RDONLY, 0666);
    for(i=0; i<numElemCache; i++){
        cod = cache[i].codigo;
        lseek(fd,cod * sizeof(Artigo),SEEK_SET);
        read(fd,&a, sizeof(Artigo));
        cache[i] = a;
    }
    close(fd);
}

void adicionaCache(int cod){
    Artigo a;
    int fd = open("./files/artigos",O_RDONLY, 0666);
    lseek(fd,cod * sizeof(Artigo),SEEK_SET);
    read(fd,&a, sizeof(Artigo));
    close(fd);
    cache[posCache] = a;
    if(posCache == 9) posCache = 0;
    else posCache++;
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
    if(fd==-1){
      perror("abrir artigos");
      return 1;
    }
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
    if(fd==-1){
      perror("abrir stocks");
    }
    else{
      lseek(fd,c * sizeof(Stock),SEEK_SET);
      write(fd,&s,sizeof(Stock));
      close(fd);
    }
}

void insere_venda (int c, int q, int m) {
    char buf[50];
    int lido;
    lido=sprintf(buf,"%d %d %d\n",c,q,m);
    int fd = open("./files/vendas", O_CREAT| O_APPEND | O_WRONLY, 0666);
    if(fd==-1){
      perror("abrir vendas");
    }
    else{
	     write(fd,buf,lido);
       close(fd);
    }
}

// quantidade negativa -> alterar stocks,
// verificar preco do artigo e inserir venda
int efetua_venda (int c, int q) {
    int r = -1, res;
    Stock novo;
    int qvenda;
    int fdS = open("./files/stocks", O_CREAT| O_RDONLY, 0666);
    if(fdS == -1){
      perror("abrir stock");
      return -1;
    }
    lseek(fdS,c * sizeof(Stock),SEEK_SET);
    res = read(fdS,&novo, sizeof(Stock));
    close(fdS);
    //se existir stock
    if(res > 0){
      if(novo.quantidade >= abs(q)){
        qvenda = -q;
        r = novo.quantidade + q;
      }
      else{
          qvenda = novo.quantidade;
          r=0;
    }
    set_stock(c,r);
    int total=getFromCache(c);
    if(total==-1){
      total = qvenda * getPrecoArt(c);
      adicionaCache(c);
    }
    insere_venda(c,qvenda,total);
  }
    //se não existir stock
    else{
      set_stock(c,q);
      r=0;
    }
    return r;
}

// quantidade positiva -> alterar stocks
int update_stock (int c, int q) {
    int r, res;
    Stock antigo;
    int fd = open("./files/stocks", O_CREAT | O_RDONLY, 0666);
    if(fd==-1){
      perror("abrir stocki");
      return -1;
    }
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    res = read(fd,&antigo, sizeof(Stock));
    close(fd);
    if(res > 0){
        r = antigo.quantidade + q;
        set_stock(c,r);
    }
    else{
        r = q;
        set_stock(c,r);
    }
    return r;
}

int writeOutput(char* res,int lido,char* pipe){
  //pipe[strlen(pipe)-1]='\0';
  int fd=open(pipe,O_WRONLY);
  if(fd==-1) perror("output here");
  write(fd,res,lido);
  close(fd);
  return 0;
}


int processa_instrucao (char* buf) {
    int stock, preco;
    stock=preco=0;
    char res[300];
    int w=0;
    //char instrucao= strdup(buf);
    int cod,quantidade;
    char pipe[200];
    int preenchido=0;
    preenchido= sscanf(buf,"%d %d:%s",&cod ,&quantidade,pipe);



    // separar argumentos
    /*
    char * saveptr;

    char* tok= strtok_r(buf,":",&saveptr);
    char* pipe=strtok_r(NULL,":",&saveptr);
    cod=atoi(strtok_r(tok," ",&saveptr));
    quant=strtok_r(NULL," ",&saveptr);
    */
    if(artigo_existe(cod) > 0){
      //consulta de preço e stock
      //if(quant==NULL){
      if(preenchido!=3){
        preenchido= sscanf(buf,"%d:%s",&cod,pipe);
        if(preenchido==2){
          int preco = getPrecoArt(cod);
          stock = getStock(cod);
          w= sprintf(res,"Stock: %d || Preco: %d\n",stock,preco);
        }
        else{
          return 1;
        }
      }
      else {
        //int quantidade=atoi(quant);
        //adicionar stock
        if(quantidade>0){
            stock = update_stock(cod,quantidade);
            w = sprintf(res,"Novo Stock: %d\n",stock);
        }
        else{
          //venda
          stock = efetua_venda(cod,quantidade);
          w = sprintf(res,"Novo Stock: %d\n",stock);
        }
      }
    }
    else {
      perror("Código não existe");
      return -1;
    }
    writeOutput(res,w,pipe);
    return 0;
}


int readln(int fildes, char *buf, int maxBytes){
  char byte;
  int i = 0;
  int res;
  while (i < maxBytes && ((res = read(fildes,&byte,1)) > 0)){
    if (byte != '\n' && byte != EOF){
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
    int fd = open("files/vendas",O_RDONLY,0666);
    int size = lseek(fd,0,SEEK_END);
    close(fd);
    int numPipes=1;


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

    int numPipes=10;
    pid_t pid=getpid();
    //ficheiro com o pid do servidor
    int fd=open("pidServ", O_CREAT | O_WRONLY,0666);
    write(fd,&pid,sizeof(pid_t));
    close(fd);
    int d=open("pidServ",O_CREAT | O_RDONLY,0666);
    read(fd,&d,sizeof(d));
    close(fd);

    //criação pipe para comunicaçaõ com cv
    if(mkfifo("pipeServ",0666)==-1)
      perror("Criação do pipe central");

    //definição handler para pedido de agregação
    if( signal(SIGUSR1,agrega) == SIG_ERR){
      perror("Signal failed");
    }

    if( signal(SIGUSR2,atualizaCache) == SIG_ERR){
      perror("Signal failed");
    }

    if( signal(SIGINT,end) == SIG_ERR){
      perror("Signal failed");
    }

    //criação de 10 pipes anónimos para comunicação entre pai e filhos
    int pipeToChild[numPipes][2];
    for(int i = 0; i < numPipes; i++) {
        pipe(pipeToChild[i]);
    }

    int f=fork();
    if (f==0){
      for(int i=0;i<numPipes;i++){
        int p=fork();
        if(p==0){
          for(int j=0;j<numPipes;j++){
            close(pipeToChild[i][1]);
          }
          char buf[SIZELINE];
          while(1){
            while((readln(pipeToChild[i][0],buf,SIZELINE))>0){
            processa_instrucao(strdup(buf));
            }
          }
        }
      }
      _exit(0);
    }

    //pai dos filhos
    for(int j=0;j<numPipes;j++){
      close(pipeToChild[j][0]);
    }
    char buf[SIZELINE];
    int n=0;
    int pipe= open("pipeServ",O_RDONLY);
    while(1){
      while((n=readln(pipe,buf,SIZELINE))>0){
        char * tosend=strdup(buf);
        char * saveptr =NULL;
        char* tok=strtok_r(buf," ",&saveptr);
        int cod= atoi(tok);
        int res=cod%10;
        write(pipeToChild[res][1],tosend,n);
      }
    }

    return 0;
}
