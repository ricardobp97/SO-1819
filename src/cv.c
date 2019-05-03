#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "headers/stock.h"
#include "headers/artigo.h"
#include "headers/venda.h"

char* getTime(){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    return asctime(timeinfo);
}

// Cliente de Vendas:
// Abre o pipe do servidor para lhe fornececer input
// Cria o seu pipe para receber output do servidor
// Eniva mensagem ao servidor para sinalizar existencia
// Mensagens iniciadas com "C:" -> nome do pipe
// Mensagens iniciadas com "mypipe:" -> instrucao
// relativa ao respetivo cliente que detem o pipe
int main () {
    int res, size;
    char buffer[50];
    int pipe = open("pipe", O_WRONLY, 0666);
    // Criar pipe proprio
    char * mypipe = getTime();
    if(mkfifo(mypipe, 0666) == -1){
        perror("pipe cliente");
    }
    // Enviar mensagem ao servidor
    write(pipe,strcat("C:",mypipe),50);
    // abre o proprio pipe
    int pp = open(mypipe, O_RDONLY, 0666);
    char * m = strcat(mypipe,":");
    size = strlen(m);
    
    while((res = read(0, &buffer, 50)) > 0){
        write(pipe,strcat(m,buffer),res+size);
    }

    close(pipe);
    close(pp);
    unlink(strcat("./",mypipe));
}