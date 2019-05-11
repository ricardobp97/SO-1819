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

typedef struct lligada {
	off_t valor;
	struct lligada *prox;
} *L;

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

int main () {
    int res;
    char buffer[1024];
    Artigo a;
    Stock s;
    off_t p;
    size_t t;
    
    printf("--- Ficheiro Strings ---\n");
    int fd = open("./strings",O_RDONLY,0666);
    while((res = readln(fd, buffer, 1024)) > 0){
        write(1,buffer,res);
    }
    close(fd);
    printf("\n");

    printf("--- Ficheiro Lista ---\n");
    fd = open("./lista",O_RDONLY,0666);
    while((res = read(fd, &p, sizeof(off_t))) > 0){
        res = snprintf(buffer,1024,"Posicao = %ld\n",p);
        write(1,buffer,res);
    }
    close(fd);
    printf("\n");

    printf("--- Ficheiro Tamanho ---\n");
    fd = open("./tamanho",O_RDONLY,0666);
    while((res = read(fd,&t,sizeof(size_t))) > 0){
        res = snprintf(buffer,1024,"Tamanho = %zd\n",t);
        write(1,buffer,res);
    }
    close(fd);
    printf("\n");

    printf("--- Ficheiro Artigos ---\n");
    fd = open("./artigos",O_RDONLY,0666);
    while((res = read(fd, &a, sizeof(Artigo))) > 0){
        res = snprintf(buffer,1024,"Código = %d\nPosicao = %ld\nPreco = %d\n",a.codigo,a.posicao,a.preco);
        write(1,buffer,res);
        printf("\n");
    }
    close(fd);

    printf("--- Ficheiro Stocks ---\n");
    fd = open("./stocks",O_RDONLY,0666);
    while((res = read(fd, &s, sizeof(Stock))) > 0){
        res = snprintf(buffer,1024,"Código = %d\nQuantidade = %d\n",s.codigoArt,s.quantidade);
        write(1,buffer,res);
        printf("\n");
    }
    close(fd);

    printf("--- Ficheiro Vendas ---\n");
    fd = open("./vendas",O_RDONLY,0666);
    while((res = readln(fd, buffer, 1024)) > 0){
        write(1,buffer,res);
    }
    close(fd);
    return 0;
}