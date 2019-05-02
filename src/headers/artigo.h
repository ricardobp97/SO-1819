#include <sys/types.h>

typedef struct Artigo{
	int codigo;
  float preco;
  off_t posicao;
} *Artigo;


Artigo new_artigoN();
Artigo new_artigo(int codigo, off_t posicao, float preco);
void setPrice(Artigo a , float np);
