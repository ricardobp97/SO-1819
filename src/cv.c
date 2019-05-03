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

int main () {
    int res;
    char *buffer[50];
    int pipe = open("pipe", O_WRONLY, 0666);
    // Criar pipe proprio
    char * mypipe = getTime();
    if(mkfifo(mypipe, 0666) == -1){
        perror("pipe cliente");
    }
    // Enviar mensagem ao servidor a informar qual o seu pipe
    write(pipe,strcat("C:",mypipe),50);
    // abre o proprio pipe
    int pp = open("pipe", O_RDONLY, 0666);

    while((res = read(0, &buffer, 50)) > 0){
        write(pipe,buffer,res);
    }

    close(pipe);
}