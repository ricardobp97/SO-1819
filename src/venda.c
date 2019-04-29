#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/venda.h"


Venda new_venda(int quantidade, double preco){
  Venda v=malloc(sizeof(struct Venda));
  v->quant= quantidade;
  v->preco=preco;
  return v;
}

int getQuantidade(Venda v){
  return v->quant;
}

double getPreco (Venda v){
  return v->preco;
}


void addQuantidade(Venda v, int quantidade){
    v->quant=(v->quant)+quantidade;
}

void addPreco(Venda v, double preco){
    double ant= v->preco;
    double s= ant+preco;
    v->preco=ant+preco;

}
