#include <linux/limits.h>
#include <stdio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef const char* Token;

typedef struct {
    const Token token;
    void (*function)();
} Command;

typedef struct {
    const char *name;
    int value;
} Variable;

Variable variables[99];

void mov() { printf("Executing MOV\n"); }
void inc() { printf("Executing INC\n"); }
void dec() { printf("Executing DEC\n"); }
void add() { printf("Executing ADD\n"); }
void sub() { printf("Executing SUB\n"); }
void mul() { printf("Executing MUL\n"); }
void div_() { printf("Executing DIV\n"); }
void err(Token token) {printf("Command not found: %s\n", token);}

Command commands[] = {
    {"MOV", mov},
    {"INC", inc},
    {"DEC", dec},
    {"ADD", add},
    {"SUB", sub},
    {"MUL", mul},
    {"DIV", div_},
};

void run_line(char* line) {
    const char* delimiter = " ";
    Token token;
    token = strtok(strdup(line), delimiter);

    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i) {
        if (strcmp(token, commands[i].token) == 0) {
            commands[i].function();
            return;
        }
    }
    err(token);
}

int main() {
    run_line("adas b 1");
    return 0;
}
