#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

typedef struct Artigo{
	int codigo;
	off_t posicao;
	int preco;
} Artigo;

typedef struct Stock{
	int codigoArt;
	int quantidade;
} Stock;


ssize_t readln(int fildes, void *buf, size_t nbyte) {

	char* b = buf;
	int i = 0;

	while(i < nbyte) {
		int n = read(fildes, &b[i],1);
		if(n <= 0)
			break;
		if(b[i] == '\n') {
			b[i] = '\n';
			i++;
			break;
		}
		i++;
	}
	return i;
}

int main (int argc, char * argv[]) {
    int res;
    char buffer[1024];
    Artigo a;
    Stock s;
    
    printf("--- Ficheiro Strings ---\n");
    printf("\n");
    int fd = open("./strings",O_RDONLY,0666);
    while((res = readln(fd, buffer, 1024)) > 0){
        write(1,buffer,res);
    }
    close(fd);
    printf("\n");
    printf("\n");

    printf("--- Ficheiro Artigos ---\n");
    printf("\n");
    fd = open("./artigos",O_RDONLY,0666);
    while((res = read(fd, &a, sizeof(Artigo))) > 0){
        res = snprintf(buffer,1024,"Código = %d\nPosicao = %lld\nPreco = %f\n",a.codigo,a.posicao,a.preco);
        write(1,buffer,res);
    }
    close(fd);
    printf("\n");

    printf("--- Ficheiro Stocks ---\n");
    printf("\n");
    fd = open("./stocks",O_RDONLY,0666);
    while((res = read(fd, &s, sizeof(Stock))) > 0){
        res = snprintf(buffer,1024,"Código = %d\nQuantidade = %d\n",s.codigoArt,s.quantidade);
        write(1,buffer,res);
    }
    close(fd);
    printf("\n");

    printf("--- Ficheiro Vendas ---\n");
    printf("\n");
    fd = open("./vendas",O_RDONLY,0666);
    while((res = readln(fd, buffer, 1024)) > 0){
        write(1,buffer,res);
    }
    close(fd);
    printf("\n");

    return 0;
}