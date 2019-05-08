#include <sys/types.h>

typedef struct Artigo{
	int codigo;
	off_t posicao;
	int preco;
} Artigo;


// Api
Artigo novo_Artigo(int codigo, off_t posicao, int preco);
