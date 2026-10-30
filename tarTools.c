//?------------------[HEADERS]------------------

#include <stdarg.h>
#include <stdlib.h>

//?------------------[TOOLS]------------------

char * typeFlagConvert(int type){
    if (type/4096 == 8){
        return "0";
    } if (type/4096 == 4){
        return "5";
    } if (type/4096 == 2){
        return "4";
    } if (type/4096 == 10){
        return "2";
    } if (type/4096 == 6){
        return "4";
    } if (type/4096 == 1){
        return "6";
    }
    return 0;
    //? note : gérer le cas de return '1'
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
    char * name = calloc(100, sizeof(char));
    int i = 0;
    for (; fullName[i] != '\0' && i<99; i++){
        prefix[i] = fullName[i];
    }
    if (fullName[i] == '\0'){
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
        for (; fullName[i] != '\0'; i++){
            name[i2] = fullName[i];
            i2++;

        }
    }
    char ** prefix_name = malloc(2*sizeof(char *));
    prefix_name[0] = prefix;
    prefix_name[1] = name;
    return prefix_name;
}