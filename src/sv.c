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
  struct tm * tf;

  time ( &rawtime );
  static char tempo[60];
  tf = localtime ( &rawtime );
  snprintf(tempo,60,"%d-%d-%dT%d-%d-%d",tf->tm_year+1900,tf->tm_mon,tf->tm_mday,tf->tm_hour,tf->tm_min,tf->tm_sec);
  return tempo;
}

void insere_venda (int c, int q, float m) {
    printf("Insere\n");
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
    printf("Venda\n");
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
    printf("Update Stock\n");
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
    printf("Processa Instrucao\n");
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
                printf("token 0 = %s\ntoken 1 = %s\n",token[0], token[1]);
                // Entrar no pipe do cliente
                cliente = open(token[1],O_WRONLY, 0666);
                write(cliente,"Ligacao Estabelecida!\n",23);
            }
            else{
                if(token[0] != NULL){
                    printf("token 0 = %s\ntoken 1 = %s\n",token[0], token[1]);
                    resposta = processa_instrucao(token[1]);
                    write(cliente,resposta,16);
                }
            }
    }
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
