//?------------------[HEADERS]------------------

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

//?------------------[TOOLS]------------------

char * typeFlagConvert(int type);

char * base(int n, int l, int base);

//?------------------[WRITE]------------------

void fileWrite(int file, char* str, int strl, int l){
    if (strl>0){
        write(file, str, strl);
    } else {
        strl = 0;
        for (int i = 0; str[i]!='\0';i++){
            write(file, &str[i], 1);
            strl++;
        }
    }
    while (strl<l){
        write(file, "\0",1);
    strl++;
 }
}

void chksum(int file, int block){
    int sum = 0;
    int i = 0;
    char header[512];
    lseek(file, 512*block, 0);
    read(file, header, 512);
    while (i<512){
        sum += header[i];
        i++;
    }
    lseek(file, 512*block+148, 0);
    fileWrite(file, base(sum,6,8),6,7);
    fileWrite(file, " ",1,1);
}

void fileWriteLink(int file, char * name){
    struct stat st;
    lstat(name, &st);
    if (typeFlagConvert(st.st_mode)[0] == '2'){
        char * buf = malloc(st.st_size);
        readlink(name,buf,st.st_size);
        fileWrite(file,buf,st.st_size,100);
    } else {
        fileWrite(file,"\0",1,100);
    }
}