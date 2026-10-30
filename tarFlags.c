#include <unistd.h>
#include <fcntl.h>

void fHeader(int file, char * name, int block);

int fContent(int fileArc, char * name, int block);

int blockcount(int file);

void createTar(char * arc, char ** names, int l){
    int file = open(arc, O_RDWR | O_CREAT, 0777);
    int block = 0;
    for (int i = 0; i<l;i++){
        fHeader(file,names[i],block++);
        block += fContent(file, names[i], block);
    }
    close(file);
}

void appendTar(char * arc, char ** names, int l){
    int file = open(arc, O_RDWR);
    int block = blockcount(file)-2;
    for (int i = 0; i<l;i++){
        fHeader(file,names[i],block++);
        block += fContent(file, names[i], block);
    }
    close(file);
}

