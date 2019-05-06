#include <sys/types.h>

typedef struct Artigo{
	int codigo;
	off_t posicao;
	double preco;
} Artigo;


// Api
Artigo novo_Artigo(int codigo, off_t posicao, double preco);
