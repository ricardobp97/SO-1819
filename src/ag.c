#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/venda.h"

Venda vendas[1024];
int in[1024];


void atualiza(char* argus[]){
  int cod=atoi(argus[0]);
  int quant= atoi(argus[1]);
  double preco=atof(argus[2]);

  if(in[cod]==0){
    in[cod]=1;
    vendas[cod]=new_venda(quant,preco);
  }
  else{
    addQuantidade(vendas[cod],quant);
    addPreco(vendas[cod],preco);

  }
}

void separa(char * line, char* argus[]){
  char* token;

  token=strtok(line," ");
  for(int i=0;token!=NULL;i++){
    argus[i]=token;
    token = strtok(NULL," ");
  }
}

void tratamento(char *line){

  char* argus[2];
  separa(line,argus);
  atualiza(argus);
}


int main(int argc, char const *argv[]) {
  for(int i=0;i<1024;i++){
    in[i]=0;
  }
  char* line1=strdup(argv[1]);
  tratamento(line1);
  return 0;
}
