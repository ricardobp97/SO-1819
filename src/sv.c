#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "estruturas.h"

Venda new_venda (int c, int q, float m) {
    return (Venda){
        .codigoArt = c,
        .quantidade = q,
        .montanteTotal = m
	};
}

void insere_venda (Venda *v) {
	int fd = open("./files/vendas", O_CREAT | O_APPEND | O_WRONLY, 0600);
	write(fd,v,sizeof(Venda));
    close(fd);
}

int main () {
    
    /* Testes!!
    Venda v2;
    Venda v = new_venda(1,2,2.5);
    Venda v1 = new_venda(2,7,12.5);
    insere_venda(&v);
    insere_venda(&v1);
    int fd = open("./files/vendas", O_RDONLY);
    lseek(fd,0,SEEK_SET);
    read(fd,&v2, sizeof(Venda));
    printf("Codigo: %d\n", v2.codigoArt);
    printf("Quantidade: %d\n", v2.quantidade);
    printf("Montante Total: %f\n", v2.montanteTotal);
    close(fd);
    */

    return 0;
}