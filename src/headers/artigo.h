#include <sys/types.h>

typedef struct Artigo{
	int codigo;
	off_t posicao;
	int preco;
} Artigo;

typedef struct lligada {
	off_t valor;
	struct lligada *prox;
} *L;


// Api
Artigo novo_Artigo(int codigo, off_t posicao, int preco);