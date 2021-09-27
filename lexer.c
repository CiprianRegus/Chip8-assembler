#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h> 

typedef unsigned short ushort;

enum symbol{
    CLS, RET, SYS, JP, CALL, SE, SNE, LD, ADD, AND, OR, XOR, SUB,
    SHR, SHL, SUBN, RND, DRW, SKP, SKNP, V, VAL
};

unsigned int line_n = 1;

/*
typedef struct Token{
    enum symbol s;
    const char *value
};
*/

int lenHelper(unsigned x) {
    if (x > 0xffff)      return -1;
    if (x > 0xfff)       return 4;
    if (x > 0xff)        return 3;
    if (x > 0xf)         return 2;
    return 1;
}

int symbolcmp(const char *str, enum symbol s){
    printf("%d\n", s);
    return 0;
}

char *subn(char *line){
    return "";
}

ushort parse_jp(char *line){
    char *addr = strtok(line, " \t");
    int addr_num = 0;

    addr = strtok(NULL, " \t");
    if(addr == NULL){
        fprintf(stderr, "Line %d: Syntax error\n", line_n);
        exit(EXIT_FAILURE);
    }
    sscanf(addr, "%x", &addr_num);
    if(strlen(addr) - 1 != lenHelper(addr_num)){
        printf("Line %d: invalid jump address (%s)\n", line_n, addr);
        //exit(EXIT_FAILURE);
    }
    if(strlen(addr) - 1 > 3 ){
        
        fprintf(stderr, "Line %d: jump address has to be a 12 bit field (found %s)\n", line_n, addr);
        exit(EXIT_FAILURE);
    }
    addr = strtok(NULL, " \t");
    if(addr != NULL){
        fprintf(stderr, "Line %d: Syntax error (JP instruction only takes 1 address)\n", line_n);
        exit(EXIT_FAILURE);
    }

    return 0x1000 | (ushort)addr_num;
}   

ushort parse_call(char *line){
    char *addr = strtok(line, " \t");
    int addr_num = 0;

    addr = strtok(NULL, " \t");
    if(addr == NULL){
        fprintf(stderr, "Line %d: Syntax error\n", line_n);
        exit(EXIT_FAILURE);
    }
    sscanf(addr, "%x", &addr_num);
    if(strlen(addr) - 1 != lenHelper(addr_num)){
        printf("Line %d: invalid call address (%s)\n", line_n, addr);
        //exit(EXIT_FAILURE);
    }
    if(strlen(addr) - 1 > 3 ){
        fprintf(stderr, "Line %d: call address has to be a 12 bit field (found %s)\n", line_n, addr);
        exit(EXIT_FAILURE);
    }
    addr = strtok(NULL, " \t");
    if(addr != NULL){
        fprintf(stderr, "Line %d: Syntax error (CALL instruction only takes 1 address)\n", line_n);
        exit(EXIT_FAILURE);
    }

    return 0x2000 | (ushort)addr_num;
}

ushort parse_se(char *line){
    char *addr = strtok(line, " \t");
    unsigned int byte = 0;
    int register_num = 0;

    addr = strtok(NULL, " \t");
    if(addr == NULL){
        fprintf(stderr, "Line %d: Syntax error\n", line_n);
        exit(EXIT_FAILURE);
    }

    if(addr[0] != 'V' || addr[2] != ','){
        fprintf(stderr, "Line %d: Syntax error\n", line_n);
        exit(EXIT_FAILURE);
    }
    if(strchr("1234567890abcdefABCDEF", addr[1]) == NULL){
        fprintf(stderr, "Line %d: Invalid register\n", line_n);
        exit(EXIT_FAILURE);
    }
    sscanf(&addr[1], "%x", &register_num);
    addr = strtok(NULL, " ");   // byte value
    sscanf(addr, "%x", &byte);

    return 0x3000 | register_num << 8 | byte; 
}

void tokenize(FILE *f){
    
    char *line = NULL;
    size_t read = 0;
    size_t len = 0;

    const char output_file_name[] = "output.ch8";   
    FILE *output_file = fopen(output_file_name, "wb+");
    if(output_file == NULL){
        fprintf(stderr, "Error opening output file\n");
        exit(EXIT_FAILURE);
    }

    while((read = getline(&line, &len, f)) != -1){
        
        char *line_cpy = (char *)malloc(strlen(line) * sizeof(char));    
        strcpy(line_cpy, line);
        char *instruction = strtok(line_cpy, " ");
        printf("%s\n", instruction); 

        if(strcmp(instruction, "CLS") == 0){
            ushort mc_instr = 0x00e0;
            fwrite(&mc_instr, 1, sizeof(ushort), output_file);
        }
        else if(strcmp(instruction, "RET") == 0){
            ushort mc_instr = 0x00ee;
            fwrite(&mc_instr, 1, sizeof(ushort), output_file);
        }
        else if(strcmp(instruction, "JP") == 0){
            ushort mc_instr = parse_jp(line);
            fwrite(&mc_instr, 1, sizeof(ushort), output_file);
        }
        else if(strcmp(instruction, "CALL") == 0){
            ushort mc_instr = parse_call(line);
            fwrite(&mc_instr, 1, sizeof(ushort), output_file);
        }
        else if(strcmp(instruction, "SE") == 0){
            ushort mc_instr = parse_se(line);
            fwrite(&mc_instr, 1, sizeof(ushort), output_file);
        }
        line_n++;
        free(line_cpy);
    }
    free(line);
}

int main(int argc, char **argv){
    if(argc < 2){
        fprintf(stderr, "No input file\n");
        exit(EXIT_FAILURE);
    }

    const char *file_name = (const char *)argv[1];
    FILE *input_file = fopen(file_name, "r");
    
    if(input_file == NULL){
        fprintf(stderr, "Couldn't open file\n");
        exit(EXIT_FAILURE);
    }
    tokenize(input_file);
    fclose(input_file);

    return 0;
}