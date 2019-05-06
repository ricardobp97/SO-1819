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

char *inst;

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
    int res,f;
    inst = malloc(50 * sizeof(char));
    strcat(inst,"C-");
    char buffer[50];
    char *mypipe = strtok(getTime(),"\n");
    int pipe = open("pipe", O_WRONLY, 0666);

    // Enviar mensagem ao servidor
    strcat(inst,mypipe);
    write(pipe,inst,50);

    if(mkfifo(mypipe, 0666) == -1){
        perror("pipe cliente");
    }
    if((f = fork()) == 0){
        // abre o proprio pipe
        int pp = open(mypipe, O_RDONLY, 0666);
        int n;
        char buf[50];
        while((n = read(pp, &buf, 50)) > 0){
            write(1,buf,n);
        }
        close(pp);
        exit(0);
    }
    
    while((res = read(0, &buffer, 50)) > 0){
        // é preciso dar refresh no inst...
        inst = mypipe;
        strcat(inst,"-");
        strcat(inst,buffer);
        write(pipe,inst,50);
    }
    close(pipe);
    inst = "./";
    strcat(inst,mypipe);
    unlink(inst);
}