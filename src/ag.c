#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

typedef struct par{
  int quant;
  int preco;
} *Par;

#define SIZEARRAY 16000
#define SIZELINE 200
int size;
char fileName[10];
Par agregados[SIZEARRAY];


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


int writeOutput(){

  for(int i=0;i<SIZEARRAY;i++){
    if(agregados[i]!=NULL){
      char buf[200];
      int n=sprintf(buf,"%d %d %d\n",i,agregados[i]->quant,agregados[i]->preco );
      write(1,buf,n);
    }
  }
return 0;
}


void adiciona(int cod, Par p){

  Par lido=agregados[cod];
  lido->quant+=p->quant;
  lido->preco+=p->preco;
  free(p);
}


void atualiza(char* line){
  Par p= malloc(sizeof(struct par));
  int cod;
  sscanf(line,"%d %d %d\n", &cod,&(p->quant),&(p->preco));
   if(p->quant!=0 && p->preco!=0){
    if (agregados[cod]==NULL){
      agregados[cod]=p;
    }
    else{
      adiciona(cod,p);
    }
  }
}


int main(int argc, char const *argv[]) {
  sprintf(fileName,"%d",getpid());
  char buf[SIZELINE];
  for(int i=0;i<SIZEARRAY;i++){
    agregados[i]=NULL;
  }
  while((size=readln(0,buf,SIZELINE))){
      atualiza(buf);
  }
  writeOutput();
  return 0;
}
