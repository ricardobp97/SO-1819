typedef struct Venda {
	int codigoArt;
    int quantidade;
	float montanteTotal;
} *Venda;

Venda new_venda (int codigo, int quantidade, float total);
int getQuantidade (Venda v);
float getPreco (Venda v);
void setQuantidade(Venda v, int quantidade);
void setPreco(Venda v, float preco);


void insere_venda(int c, int q, float m);
void efetua_venda (int c, int q);
