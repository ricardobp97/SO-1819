typedef struct Stock{
	int codigoArt;
	int quantidade;
} Stock;

Stock new_stock(int c, int q);
int update_stock(int c, int q);
