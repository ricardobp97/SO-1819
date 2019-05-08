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
    struct tm * tf;
  
    time ( &rawtime );
    static char tempo[60];
    tf = localtime ( &rawtime );
    snprintf(tempo,60,"%d-%d-%dT%d-%d-%d",tf->tm_year+1900,tf->tm_mon,tf->tm_mday,tf->tm_hour,tf->tm_min,tf->tm_sec);
    return tempo;
}

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

int length(char * s) {
    int i;
    for(i=0 ; s[i]!='\0'; i++);
    return i;
}

// Cliente de Vendas:
// Abre o pipe do servidor para lhe fornececer input
// Cria o seu pipe para receber output do servidor
// Eniva mensagem ao servidor para sinalizar existencia
// Mensagens iniciadas com "C:" -> nome do pipe
// Mensagens iniciadas com "mypipe:" -> instrucao
// relativa ao respetivo cliente que detem o pipe
int main () {
    int res,f,i;
    char * inst = malloc(100 * sizeof(char));
    strcat(inst,"C:");
    char buffer[100];
    char *mypipe = strtok(getTime(),"\n");
    int pipe = open("pipe", O_WRONLY, 0666);

    if(mkfifo(mypipe, 0666) == -1){
        perror("pipe cliente");
    }
    // Enviar mensagem ao servidor
    strcat(inst,mypipe);
    write(pipe,inst,50);

    if((f = fork()) == 0){
        // abre o proprio pipe
        int pp = open(mypipe, O_RDONLY, 0666);
        int n;
        char buf[100];
        while((n = read(pp, buf, 100)) > 0){
            write(1,buf,n);
            write(1,"\n",2);
        }
        close(pp);
        exit(0);
    }
    
    while((res = readln(0, buffer, 100)) > 0){
        // é preciso dar refresh no inst...
        //char * inst = malloc(100 * sizeof(char));
        i = length(mypipe) + res;
        snprintf(inst,i+1,"%s:%s\n",mypipe,buffer);
        write(pipe,inst,i+1);
    }
    
    close(pipe);
    snprintf(inst,100,"./%s",mypipe);
    unlink(inst);
}