typedef struct Stock{
	int codigoArt;
	int quantidade;
} *Stock;

Stock new_stock(int c, int q);
void update_stock(int c, int q);
