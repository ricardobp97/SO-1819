typedef struct Venda {
    int quant;
    double preco;
} *Venda;

Venda new_venda (int quantidade, double preco);
int getQuantidade (Venda v);
double getPreco (Venda v);
void addQuantidade(Venda v, int quantidade);
void addPreco(Venda v, double preco);
