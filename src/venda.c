#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/venda.h"


Venda new_venda(int codigo, int quantidade, int total){
  Venda v=malloc(sizeof(Venda));
  v->codigoArt = codigo;
  v->quantidade = quantidade;
  v->montanteTotal = total;
  return v;
}


int getQuantidade(Venda v){
  if(v)
    return v->quantidade;
  else
    return -1;
}

int getPreco(Venda v){
  if(v)
    return v->montanteTotal;
  else
    return -1;
}


void setQuantidade(Venda v, int quantidade){
  if(v)
    v->quantidade=(v->quantidade)+quantidade;
}

void setPreco(Venda v, int total){
  if(v){
    v->montanteTotal+=total;
  }
}
