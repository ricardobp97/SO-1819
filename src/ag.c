  #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int size;
char* fileName;
char* output;

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

char* getTime(){
  time_t rawtime;
  struct tm * tf;

  time ( &rawtime );
  static char tempo[60];
  tf = localtime ( &rawtime );
  snprintf(tempo,60,"%d-%d-%dT%d-%d-%d",tf->tm_year+1900,tf->tm_mon,tf->tm_mday,tf->tm_hour,tf->tm_min,tf->tm_sec);
  return tempo;
}

int writeOutput(){
  int fd= open(fileName,O_RDONLY,0600);
  int n;
  char buf[200];
  int out=open(output,O_WRONLY);
  dup2(out,1);
  close(out);
  while ((n=readln(fd,&buf,200))) {
    write(1,buf,n);
  }
  close(fd);
  return 0;
}


void adiciona(int cod, int quantidade, int preco, char* line){
  int c,q;
  int p=0;
  c=q=0;
  sscanf(line,"%d %d %f\n",&c,&q,&p );
  q+=quantidade;
  p+=preco;
  char buf[200];
  int toWrite= snprintf(buf,200,"%d %d %f\n",cod, q, p);
  int fd= open(fileName, O_CREAT| O_WRONLY,0600);
  lseek(fd,50*cod,SEEK_SET);
  int n= write(fd,buf,toWrite);
  close(fd);
}


void atualiza(char* line){
  int codigo,quant;
  codigo=quant=0;
  int preco=0;
  char buf[50];

  int fd= open(fileName, O_CREAT| O_RDONLY,0600);
  sscanf(line,"%d %d %f\n", &codigo,&quant,&preco);
  int l=lseek(fd,50*codigo,SEEK_SET);
  int n=readln(fd,buf,50);
  close(fd);
  if(n==-1)
    perror("ERROR");
  if(n==0){
    open(fileName,O_WRONLY,0600);
    int l=lseek(fd,50*codigo,SEEK_SET);
    write(fd,line,size);
    close(fd);
  }
  else{
    adiciona(codigo,quant,preco,buf);
  }
}


int main(int argc, char const *argv[]) {
  fileName=strdup(argv[1]);
  output="porfavor";
  char * pipe=strdup(argv[2]);
  int fd=open(pipe,O_RDONLY);
  printf("%d\n",fd );
  dup2(fd,0);
  close(fd);
  char buf[200];
  while((size=readln(0,buf,200))>0){
  //  printf("%s\n",buf );
    atualiza(buf);
  }
  close(fd);
  unlink(pipe);
  writeOutput();
}
