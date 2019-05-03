#include "headers/artigo.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

Artigo new_artigoN(){
	Artigo a = malloc(sizeof(Artigo));
	a->codigo = 0;
	a->posicao = 0;
	a->preco = 0;
	return a;
}

Artigo new_artigo(int codigo, off_t posicao, float preco){
	Artigo a = malloc(sizeof(Artigo));
	a->codigo = codigo;
	a->posicao = posicao;
	a->preco = preco;
	return a;
}

void setPrice(Artigo a , float np){
	if(a)
		a->preco = np;
}
