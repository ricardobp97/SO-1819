typedef struct Venda {
	int codigoArt;
    int quantidade;
	int montanteTotal;
} *Venda;

Venda new_venda (int codigo, int quantidade, int total);
int getQuantidade (Venda v);
int getPreco (Venda v);
void setQuantidade(Venda v, int quantidade);
void setPreco(Venda v, int preco);


void insere_venda(int c, int q, int m);
int efetua_venda (int c, int q);
