#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "headers/stock.h"
#include "headers/artigo.h"
#include "headers/venda.h"

#define SIZELINE 256


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


char* getPid(){
    int pid=getpid();
    char buf[200];
    sprintf(buf,"%d",pid);
    return strdup(buf);
}
// Cliente de Vendas:
// Abre o pipe do servidor para lhe fornececer input
// Cria o seu pipe para receber output do servidor
// Eniva mensagem ao servidor para sinalizar existencia
// Mensagens iniciadas com "C:" -> nome do pipe
// Mensagens iniciadas com "mypipe:" -> instrucao
// relativa ao respetivo cliente que detem o pipe
int main () {
    int res,f,n;
    char inst[SIZELINE];
    char buffer[SIZELINE];
    char* tok;
    char *mypipe = getPid();

    int pipe = open("pipeServ", O_WRONLY);

    mkfifo(mypipe, 0666);

    /*if((f = fork()) == 0){
        int pp = open(mypipe, O_RDONLY, 0666);
        int n;
        char buf[100];
        while((n = read(pp, buf, 100)) > 0){
            printf("%s\n",buf );
        }
        close(pp);
        exit(0);
    }*/
    while((res = readln(0, buffer, SIZELINE)) > 0){
				char* safepointer;
        tok=strtok_r(buffer,"\n",&safepointer);
        n=sprintf(inst,"%s:%s\n",tok,mypipe);
        write(pipe,inst,n);
        printf("%s\n",mypipe);
        int me_pip = open(mypipe, O_RDONLY);
        printf("%d\n",me_pip);
        char buf[SIZELINE];
        puts("waiting for anwser");
        n = readln(me_pip,buf,SIZELINE);
        write(1,buf,n);
        close(me_pip);
    }
    printf("FINAL\n", );
  close(pipe);
  unlink(mypipe);
  return 0;
}
