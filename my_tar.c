//?------------------[HEADERS]------------------

#include <stdlib.h>

//?------------------[TYPES]------------------

typedef enum{
    FALSE,
    TRUE
} bool;

//?------------------[BLOCKS]------------------

void voidBlock(char * name);

void createTar(char * arc, char ** names, int l);

void appendTar(char * arc, char ** names, int l);

//?------------------[MAIN]------------------

int main(int ac, char** av){
    bool f_flag = FALSE;
    bool * flags = calloc(5,sizeof(bool));
    char ** fileNames = malloc(sizeof(char *)*ac);
    int fileCount = 0;
    int archiveIndex = -1;
    for (int i = 1; i<ac; i++){
        if (av[i][0]=='-'){
            for (int j = 1; av[i][j]!='\0'; j++){
                if (av[i][j]=='f'){
                    f_flag = TRUE;
                } else if (av[i][j]=='c'){
                    flags[0] = TRUE;
                } else if (av[i][j]=='r'){
                    flags[1] = TRUE;
                } else if (av[i][j]=='t'){
                    flags[2] = TRUE;
                } else if (av[i][j]=='u'){
                    flags[3] = TRUE;
                } else if (av[i][j]=='x'){
                    flags[4] = TRUE;
                }
            }
        } else if (f_flag){
            archiveIndex = i;
            f_flag = FALSE;
        } else {
            fileNames[fileCount++] = av[i];
        } 
    }
    if (flags[0]){
        if (archiveIndex>0){
            createTar(av[archiveIndex], fileNames, fileCount);
            voidBlock(av[archiveIndex]);
        }
    } else if (flags[1]){
        if (archiveIndex>0){
            appendTar(av[archiveIndex], fileNames, fileCount);
            voidBlock(av[archiveIndex]);
        }
    }
    return 0;
}