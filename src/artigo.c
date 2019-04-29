#include "headers/artigo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Artigo new_artigo(int codigo, char* nome, float preco){
	char* destino = strdup(nome);
	return (Artigo){
		.codigo = c,
		.preco = p,

	};
}

void delete_artigo(Artigo *a){
	free(a -> nome);
	// acho que a int's e floats nao se da free..
}

void rePrice(Artigo *a , float np){
	a->preco = np;
}

int main(int argc, char const *argv[]) {

  char* comand= argv
  return 0;
}
