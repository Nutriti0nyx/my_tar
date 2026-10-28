#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>


#include <stdio.h>

typedef enum{
    FALSE,
    TRUE
} bool;

int power(int n, int exp){
    if (exp == 0){
        return 1;
    }
    for (; exp>1;exp--){
        n *=n;
    }
    return n;
}

char * typeFlagConvert(int type){
    if (type/4096 == 8){
        return "0";
    } if (type/4096 == 4){
        return "5";
    } if (type/4096 == 2){
        return "4";
    } if (type/4096 == 3){
        return "2";
    } if (type/4096 == 6){
        return "4";
    } if (type/4096 == 1){
        return "6";
    }
    return 0;
    // note : gérer le cas de return '1'
}

char * base(int n, int l, int base){
    char * str = malloc(l+1);
    for (int i = l-1; i>=0; i--){
        str[i] = n%base+'0';
        n/=base;
    }
    str[l] = '\0';
    return str;
}

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

void fileWriteLink(int file, char * name){
    struct stat st;
    lstat(name, &st);
    if (typeFlagConvert(st.st_mode)[0] == '2'){
        char * buf = malloc(st.st_size);
        readlink(name,buf,st.st_size);
        fileWrite(file,buf,st.st_size,100);
    }
    fileWrite(file, "", 0, 100);
}

char * concat(int n, int l, ...){
    va_list args;
    va_start(args,l);
    char * str = malloc(l);
    char * arg;
    int i = 0;
    while (n-- > 0){
        arg = va_arg(args,char*);
        for (int i2 = 0; arg[i2] != '\0'; i2++){
            str[i] = arg[i2];
            i++;
        }
    }
    va_end(args);
    return str;
}

void swap(char * str1, char * str2, int l){
    int i = 0;
    while (i<l){
        str1[i] ^= str2[i];
        str2[i] ^= str1[i];
        str1[i] ^= str2[i]; 
        i++;
    }
}

char ** fileFullName(char * fullName){
    char * prefix = calloc(100, sizeof(char));
    prefix[0] = '.';
    prefix[1] = '/';
    char * name = calloc(100, sizeof(char));
    int i = 2;
    for (; fullName[i-2] != '\0' && i<99; i++){
        prefix[i] = fullName[i-2];
    }
    if (fullName[i-2] == '\0'){
        swap(prefix,name,100);
    } else {
        i--;
        while (prefix[i] != '/'){
            i--;
        }
        prefix[i++] = 0;
        int i2 = 0;
        while (i<99){
            name[i2] = prefix[i];
            prefix[i] = 0;
            i++;
            i2++;
        }
        for (; fullName[i-2] != '\0'; i++){
            name[i2] = fullName[i-2];
            i2++;

        }
    }
    char ** prefix_name = malloc(2*sizeof(char *));
    prefix_name[0] = prefix;
    prefix_name[1] = name;
    return prefix_name;
}

void chksum(int file){
    int sum = 0;
    int i = 0;
    char header[512];
    lseek(file, 0, 0);
    read(file, header, 512);
    while (i<512){
        sum += header[i];
        i++;
    }
    lseek(file, 148, 0);
    fileWrite(file, base(sum,6,8),6,7);
    fileWrite(file, " ",1,1);
}

void header(int file, char * name, int block){
    struct stat st;
    lseek(file, 0+512*block, 0);
    char ** prefix_name = fileFullName(name);
    stat(name,&st);
    fileWrite(file, prefix_name[1],-1,100);                                     // name + nulls = 100 | pos : 0
    fileWrite(file, concat(3, 7, "00", base(st.st_mode, 4, 8), " "), 7, 8);     // perms + null = 8 | pos : 100
    fileWrite(file, concat(2, 7, base(st.st_uid, 6, 8), " "), 7, 8);            // uid + space + null = 8 | pos : 108
    fileWrite(file, concat(2, 7, base(st.st_gid, 6, 8), " "), 7, 8);            // gid + space + null = 8 | pos : 116
    fileWrite(file, concat(2, 12, base(st.st_size, 11, 8), " "), 12, 12);       // size + space = 12 | pos : 124
    fileWrite(file, concat(2, 12, base(st.st_mtime, 11, 8), " "), 12, 12);      // mod-time + space = 12 | pos : 136
    fileWrite(file, "        ", 8, 8);;                                           // 8 spaces = 8 | pos : 148
    fileWrite(file, typeFlagConvert(st.st_mode),1,1);                           // flag = 1 | pos : 156 
    fileWriteLink(file, name);                                                  // link + nulls = 100 | pos : 157
    fileWrite(file, "ustar\0", 6, 6);                                           // ustar + null = 6 | pos : 257
    fileWrite(file, "00", 2, 2);                                                // 00 | pos : 263
    fileWrite(file, getpwuid(st.st_uid)->pw_name,-1,32);                        // user + nulls = 32 | pos : 265
    fileWrite(file, getgrgid(st.st_gid)->gr_name,-1,32);                        // guest + nulls = 32 | pos : 297 
    fileWrite(file, concat(2, 7, base(minor(st.st_rdev),6,8)," "),7,8);         // minor + space + null = 8 | pos : 329
    fileWrite(file, concat(2, 7, base(major(st.st_rdev),6,8)," "),7,8);         // major + space + null = 8 | pos : 337
    fileWrite(file, prefix_name[0],-1,155);                                     // prefix + nulls = 155 | pos : 345
    fileWrite(file, "\0\0\0\0\0\0\0\0\0\0\0\0", 12, 12);                        // nulls = 12 | pos = 500
    chksum(file);                                                               // chksum + null + space = 8 | pos = 148
}

int fileWriteContent(int fileArc, char * name, int block){
    int fileSrc = open(name, O_RDONLY);
    int count = 0;
    lseek(fileArc, 0+512*block, 0);
    char c;
    while (read(fileSrc, &c, 1) > 0){
        write(fileArc, &c, 1);
        count++;
    }
    while (count++%512 != 0){
        write(fileArc, "\0",1);
    }
    return count/512;
}

int create(char * arc, char ** names, int l){
    int file = open(arc, O_RDWR | O_CREAT, 777);
    int block = 0;
    for (int i = 0; i<l;i++){
        header(file,names[i],block++);
        block += fileWriteContent(file, names[i], block);
    }
    return 0;
}

int main(int ac, char** av){
    bool f_flag = FALSE;
    int * flags = calloc(5,sizeof(bool));
    char ** fileNames = malloc(sizeof(char *)*ac);
    int fileCount = 0;
    int archiveIndex;
    for (int i = 1; i<ac;i++){
        if (av[i][0]=='-'){
            for (int j = 1;av[i][j];j++){
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
    create(av[archiveIndex], fileNames, fileCount);
    return 0;
}