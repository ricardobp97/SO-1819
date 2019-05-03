#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>s
#include "headers/stock.h"
#include "headers/artigo.h"
#include "headers/venda.h"

void insere_venda (int c, int q, float m) {
    char buf[16];
    snprintf(buf,16,"%d %d %f\n",c,q,m);
    int fd = open("./files/vendas", O_CREAT | O_APPEND | O_WRONLY, 0600);
	write(fd,buf,16);
    close(fd);
}

// quantidade negativa -> alterar stocks,
// verificar preco do artigo e inserir venda
void efetua_venda (int c, int q) {
    // Verificar Stock
    Stock novo;
    int fdS = open("./files/stocks", O_CREAT | O_APPEND | O_WRONLY | O_RDONLY, 0600);
    lseek(fdS,c * sizeof(Stock),SEEK_SET);
    read(fdS,&novo, sizeof(Stock));
    if(novo->quantidade > 0){
        int qvenda;
        if(novo->quantidade >= q){
            qvenda = q;
            novo->quantidade -= q;
        }
        else{
            qvenda = novo->quantidade;
            novo->quantidade = 0;
        }
        // Alterar Stock
        write(fdS,&novo, sizeof(Stock));
        // Verificar preco do artigo
        Artigo a;
        int fdA = open("./files/artigos", O_RDONLY, 0600);
        lseek(fdA,c * sizeof(Artigo),SEEK_SET);
        read(fdS,&a, sizeof(Artigo));
        int total = qvenda * a->preco;
        close(fdA);
        free(a);
        // Registar venda
        insere_venda(c,qvenda,total);
    }
    close(fdS);
    free(novo);
}

// quantidade positiva -> alterar stocks
void update_stock (int c, int q) {
    Stock novo;
    int fd = open("./files/stocks", O_CREAT | O_APPEND | O_WRONLY | O_RDONLY, 0600);
    lseek(fd,c * sizeof(Stock),SEEK_SET);
    read(fd,&novo, sizeof(Stock));
    novo->quantidade += q;
    write(fd,&novo, sizeof(Stock));
    close(fd);
    free(novo);
}

void processa_instrucao (char* s) {
    // ler a string s e redirecionar o pedido para a funcao certa.
}

int main () {
    return 0;
}
