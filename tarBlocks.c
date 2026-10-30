#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

//----------------------------------

char * typeFlagConvert(int type);

char * base(int n, int l, int base);

char * concat(int n, int l, ...);

char ** fileFullName(char * fullName);

//----------------------------------

void fileWrite(int file, char* str, int strl, int l);

void fileWriteLink(int file, char * name);

void chksum(int file, int block);

//----------------------------------

void fHeader(int file, char * name, int block){
    struct stat st;
    lseek(file, 0+512*block, 0);
    char ** prefix_name = fileFullName(name);
    lstat(name,&st);
    fileWrite(file, prefix_name[1],-1,100);                                                                                     // name + nulls = 100 | pos : 0
    fileWrite(file, concat(3, 7, "00", base(st.st_mode, 4, 8), " "), 7, 8);                                                     // perms + null = 8 | pos : 100
    fileWrite(file, concat(2, 7, base(st.st_uid, 6, 8), " "), 7, 8);                                                            // uid + space + null = 8 | pos : 108
    fileWrite(file, concat(2, 7, base(st.st_gid, 6, 8), " "), 7, 8);                                                            // gid + space + null = 8 | pos : 116
    fileWrite(file, *typeFlagConvert(st.st_mode) == '2' ? "00000000000 " : concat(2, 12, base(st.st_size, 11, 8)," "),12,12);   // size + space = 12 | pos : 124
    fileWrite(file, concat(2, 12, base(st.st_mtime, 11, 8), " "), 12, 12);                                                      // mod-time + space = 12 | pos : 136
    fileWrite(file, "        ", 8, 8);                                                                                          // 8 spaces = 8 | pos : 148
    fileWrite(file, typeFlagConvert(st.st_mode),1,1);                                                                           // flag = 1 | pos : 156 
    fileWriteLink(file, name);                                                                                                  // link + nulls = 100 | pos : 157
    fileWrite(file, "ustar\0", 6, 6);                                                                                           // ustar + null = 6 | pos : 257
    fileWrite(file, "00", 2, 2);                                                                                                // 00 | pos : 263
    fileWrite(file, getpwuid(st.st_uid)->pw_name,-1,32);                                                                        // user + nulls = 32 | pos : 265
    fileWrite(file, getgrgid(st.st_gid)->gr_name,-1,32);                                                                        // guest + nulls = 32 | pos : 297 
    fileWrite(file, concat(2, 7, base(minor(st.st_rdev),6,8)," "),7,8);                                                         // minor + space + null = 8 | pos : 329
    fileWrite(file, concat(2, 7, base(major(st.st_rdev),6,8)," "),7,8);                                                         // major + space + null = 8 | pos : 337
    fileWrite(file, prefix_name[0],-1,155);                                                                                     // prefix + nulls = 155 | pos : 345
    fileWrite(file, "\0\0\0\0\0\0\0\0\0\0\0\0", 12, 12);                                                                        // nulls = 12 | pos = 500
    chksum(file,block);                                                                                                         // chksum + null + space = 8 | pos = 148
}

int fContent(int fileArc, char * name, int block){
    int fileSrc = open(name, O_RDONLY);
    int count = 0;
    lseek(fileArc, 0+512*block, 0);
    struct stat st;
    lstat(name,&st);
    char c;
    if (typeFlagConvert(st.st_mode)[0] != '2'){
        while (read(fileSrc, &c, 1) > 0){
            write(fileArc, &c, 1);
            count++;
        }
    }
    while (count++%512 != 0){
        write(fileArc, "\0",1);
    }
    return (count-1)/512;
}


void voidBlock(char * name){
    int i = 0;
    int file = open(name, O_WRONLY | O_APPEND);
    while (i++<1024){
        write(file,"\0",1);
    }
    close(file);
}

int blockcount(int file){
    int count = 0;
    char _[512];
    while(read(file,&_,512)>0){
        count++;
    }
    return count;
}