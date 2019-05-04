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


char* getTime(){
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  return asctime(timeinfo);
}

void insere_venda (int c, int q, float m) {
    char buf[200];
    int lido;
    lido=snprintf(buf,200,"%d %d %f\n",c,q,m);
    int fd = open("./files/vendas", O_CREAT | O_APPEND | O_WRONLY, 0600);
	  write(fd,buf,lido);
    close(fd);
}

// quantidade negativa -> alterar stocks,
// verificar preco do artigo e inserir venda
int efetua_venda (int c, int q) {
    // Verificar Stock
    int r = 0;
    Stock novo;
    int fdS = open("./files/stocks", O_CREAT | O_APPEND | O_WRONLY | O_RDONLY, 0600);
    lseek(fdS,c * sizeof(Stock),SEEK_SET);
    read(fdS,&novo, sizeof(Stock));
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
        int total = qvenda * a->preco;
        close(fdA);
        free(a);
        // Registar venda
        insere_venda(c,qvenda,total);
    }
    close(fdS);
    free(novo);
    return r;
}

// quantidade positiva -> alterar stocks
int update_stock (int c, int q) {
    int r;
    Stock novo;
    int fd = open("./files/stocks", O_CREAT | O_APPEND | O_WRONLY | O_RDONLY, 0600);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    read(fd,&novo, sizeof(Stock));
    novo->quantidade += q;
    r = novo->quantidade;
    write(fd,&novo, sizeof(Stock));
    close(fd);
    free(novo);
    return r;
}

char * processa_instrucao (char* s) {
    char r[16];
    int stock;
    char * token = strtok(s," ");
    if(token != NULL){
        int c = atoi(token);
        token = strtok(NULL," ");
        if(token != NULL){
            int q = atoi(token);
            if(q>0){
                stock = update_stock(c,q);
            }
            else{
                stock = efetua_venda(c,-q);
            }
            snprintf(r,16,"Novo Stock: %d\n",stock);
        }
        else{
            // mostra no stdout stock e preco
        }
    }

    return "Funciona!!";
    //return r;
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
  int rest= mkfifo("res",0666);
  char buf[200];
  int n;
/*
  pid_t pid, pid2;
  int status,n;
  */

/*
    pid=fork();
    if (pid==0){
      pid2=fork();

      if(pid2==0){
        execlp("./ag.c",getTime(),"./res",NULL);
        _exit(-1);
      }
      */
      int fd= open("./files/vendas",O_RDONLY,0600);
      int res=open("./res",O_WRONLY);
      while((n=readln(fd,buf,200))>0){
          write(res,buf,n);
      }
      close(res);
    /*
      close(res);

      _exit(0);
  }
  else{
    wait(&status);
    printf("%d\n",status );
  }
*/
}

int main(int argc, char const *argv[]) {

    int res, cliente;
    char buffer[200];
    char *token[2];
    char *resposta;

    if(mkfifo("pipe", 0666) == -1){
        perror("pipe");
    }
    int pipe = open("pipe", O_RDONLY, 0666);

    while((res = read(pipe, &buffer, 200)) > 0){
        token[0] = strtok(buffer, "-");
        token[1] = strtok(NULL, "-");
            if(*token[0] == 'C'){
                // Entrar no pipe do cliente
                cliente = open(token[1],O_WRONLY, 0666);
                write(cliente,"Ligacao Estabelecida!\n",23);
            }
            else{
                if(token[0] != NULL){
                    resposta = processa_instrucao(token[1]);
                    write(cliente,resposta,16);
                }
            }
    }

    close(pipe);
    unlink("./pipe");

    return 0;
}
