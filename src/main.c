#include <linux/limits.h>
#include <stdio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_VARIABLES 99

typedef const char* Token;

typedef struct {
    const char *left;
    const char *right;
} Args;

typedef struct {
    const Token token;
    void (*function)(Args args);
} Command;

typedef struct {
    const char *name;
    // not needed until we support referances / pointers
    // union {
    //     int intValue;
    //     struct Variable *varValue;
    // } value;
    int value;
} Var;

Var variables[MAX_VARIABLES];
int variable_count = 0;

int find_variable(Token token);
void run_line(char* line);
int atoi_1 (const char* str, int *res);
Var* get_var(Token var);

void mov(Args args);
void inc(Args args);
void dec(Args args);
void add(Args args);
void sub(Args args);
void mul(Args args);
void div_(Args args);

void mov(Args args) {
    int parse_res;
    if (variable_count >= MAX_VARIABLES) {
        fprintf(stderr, "Error: Reached MAX_VARIABLES cap\n");
        exit(1);
        return;
    }

    if (find_variable(args.left) != -1) {
        fprintf(stderr, "Error: Variable '%s' already declared.\n", args.left);
        exit(1);
        return;
    }

    if (atoi_1(args.right, &parse_res)) {
        variables[variable_count].name = args.left;

        variables[variable_count].value = get_var(args.right)->value;
    } else {
        variables[variable_count].name = args.left;
        variables[variable_count].value = parse_res;
    }
    variable_count++;
}
void inc(Args args) { printf("Executing INC\n"); }
void dec(Args args) { printf("Executing DEC\n"); }
void add(Args args) { printf("Executing ADD\n"); }
void sub(Args args) { printf("Executing SUB\n"); }
void mul(Args args) { printf("Executing MUL\n"); }
void div_(Args args) { printf("Executing DIV\n"); }
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

int find_variable(Token token) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, token) == 0) {
            return i;
        }
    }
    return -1;
}

int atoi_1 (const char* str, int *res) 
{ 
    *res = 0;
    int i = 0; 

    while (str[i] != '\0') { 
        if (str[i] - '0' > 9) {
            return 1;
        }
        *res = *res * 10 + (str[i] - '0'); 
        i++; 
    }
    return 0;
}

Var* get_var(Token var);

void run_line(char* line) {
    Args args;
    int i;
    const char* delimiter = " ";
    Token token;
    token = strtok(strdup(line), delimiter);

    for (i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i) {
        if (strcmp(token, commands[i].token) == 0) {
            goto ok;
        }
    }
    goto err;
ok:
    args.left = strtok(NULL, delimiter);
    args.right = strtok(NULL, delimiter);
    commands[i].function(args);
    return;
err:
    err(token);
}

Var* get_var(Token var) {
    int i;
    i = find_variable(var);
    if (i == -1) {
        fprintf(stderr, "Error: Variable '%s' not declared.\n", var);
        return NULL;
        // exit(1);
    }
    return &variables[i];
}

void print_var(Token var) {
    printf("%d\n", get_var(var)->value); 
}

int main() {
    run_line("MOV b 21");
    print_var("b");

    run_line("MOV x 420");
    print_var("x");

    run_line("MOV a b");
    print_var("a");

    run_line("MOV c a");
    print_var("c");


    return 0;
}
