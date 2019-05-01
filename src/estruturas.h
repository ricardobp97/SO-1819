typedef struct Artigo{
	int codigo;
    double preco;
    // Posicao Nome;
    // Tamanho Nome;
}Artigo;

typedef struct Venda{
	int codigoArt;
	int quantidade;
	float montanteTotal;
}Venda;

typedef struct Stock{
	int codigoArt;
	int quantidade;
}Stock;

//API

Artigo new_artigo(int c,double p);
int insereArtigo(char* codigo,double preco);
Venda new_venda(int c, int q, float m);
void insere_venda(Venda *v);
/*
void delete_artigo(Artigo *a);
void reName(Artigo a);
void rePrice(Artigo a);

void update_stock(int c, int q);
void show_artigo(int c);
*/