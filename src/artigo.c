#include "headers/artigo.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

Artigo novo_Artigo(int cod, off_t pos, int pre){
	return(Artigo){
		.codigo = cod,
		.posicao = pos,
		.preco = pre
	};
}

