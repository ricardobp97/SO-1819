Artigo cache[10];
int posCache = 0;
int numElemCache = 0;

int getFromCache(int cod){
    int i;
    for(i=0; i<numElemCache; i++){
        if(cache[i].codigo == cod)
            return cache[i].preco;
    }
    return -1;
}

void adicionaCache(int cod){
    Artigo a;
    int fd = open("./files/artigos",O_RDONLY, 0666);
    lseek(fd,cod * sizeof(Artigo),SEEK_SET);
    read(fd,&a, sizeof(Artigo));
    close(fd);
    cache[posCache] = a;
    if(posCache == 9) posCache = 0;
    else posCache++;
}

void atualizaCache(){
    int i, fd, cod;
    Artigo a;
    fd = open("./files/artigos",O_RDONLY, 0666);
    for(i=0; i<numElemCache; i++){
        cod = cache[i].codigo;
        lseek(fd,cod * sizeof(Artigo),SEEK_SET);
        read(fd,&a, sizeof(Artigo));
        cache[i] = a;
    }
    close(fd);
}