#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct Artigo{
	int codigo;
	off_t posicao;
	double preco;
} Artigo;


//API
Artigo new_artigo(int codigo,double preco);
