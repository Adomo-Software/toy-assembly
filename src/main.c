#include <asm-generic/errno.h>
#include <linux/limits.h>
#include <stdio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAX_VARIABLES 99
#define MAX_INSTRUCTIONS 99
#define MAX_LABELS 99
#define MAX_ARGS 2

typedef const char* Token;

typedef Token Args[MAX_ARGS];

typedef struct {
	Token token;
	void (*function)(Args args);
	int argc;
} Command;

typedef struct {
	Token name;
	// not needed until we support referances / pointers
	// union {
	//	 int intValue;
	//	 struct Variable *varValue;
	// } value;
	int value;
} Var;

typedef struct {
	void (*function)(Args);
	Args args;
} Instruction;

typedef struct {
	Token label_name;
	int program_address;
} Label;

typedef struct {
	int instruction;
	int line;
	int program;
	int label;
	int variable;
} G_counter;

G_counter global_counter = {0};

typedef struct {
	Instruction instructions[MAX_INSTRUCTIONS];
	Var variables[MAX_VARIABLES];
	Label labels[MAX_LABELS];
} G_array;

G_array global_array;

#define P global_counter.program

int find_variable(Token token);
int find_label(Token token);
Instruction* parse_line(char* line);
int atoi_1 (const char* str, int *res);
Var* get_var(Token var);
void print_env();

void mov(Args args);
void add(Args args);
void sub(Args args);
void mul(Args args);
void jez(Args args);
void jnz(Args args);
void jgz(Args args);
void jlz(Args args);

void mov(Args args) {
	int parse_res;
	int this_var = -1;
	if (global_counter.variable >= MAX_VARIABLES) {
		fprintf(stderr, "Error: Reached MAX_VARIABLES cap\n");
		exit(1);
		return;
	}

	this_var = find_variable(args[0]);
	if (this_var == -1) {
		this_var = global_counter.variable;
	} else {
		global_counter.variable--;
	}

	if (atoi_1(args[1], &parse_res)) {
		// copy variable value
		global_array.variables[this_var].name = args[0];
		global_array.variables[this_var].value = get_var(args[1])->value;
	} else {
		// it's a numer
		global_array.variables[this_var].name = args[0];
		global_array.variables[this_var].value = parse_res;
	}
	global_counter.variable++;
}
void add(Args args) {
	int parse_res;
	int this_var = -1;
	this_var = find_variable(args[0]);
	if (this_var == -1) {
		fprintf(stderr, "Error: Variable '%s' not declared.\n", args[0]);
		return;
	}

	if (atoi_1(args[1], &parse_res)) {
		global_array.variables[this_var].value += get_var(args[1])->value;
	} else {
		global_array.variables[this_var].value += parse_res;
	}
}
void sub(Args args) {
	int parse_res;
	int this_var = -1;
	this_var = find_variable(args[0]);
	if (this_var == -1) {
		fprintf(stderr, "Error: Variable '%s' not declared.\n", args[0]);
		return;
	}

	if (atoi_1(args[1], &parse_res)) {
		global_array.variables[this_var].value -= get_var(args[1])->value;
	} else {
		global_array.variables[this_var].value -= parse_res;
	}
}
void mul(Args args) {
	int parse_res;
	int this_var = -1;

	this_var = find_variable(args[0]);
	if (this_var == -1) {
		fprintf(stderr, "Error: Variable '%s' not declared.\n", args[0]);
		return;
	}

	if (atoi_1(args[1], &parse_res)) {
		global_array.variables[this_var].value *= get_var(args[1])->value;
	} else {
		global_array.variables[this_var].value *= parse_res;
	}
}
void jump(Args args) {
	global_counter.program = find_label(args[0]) - 1;
}
void jez(Args args) {
	int parse_res;
	int test_num;

	if (atoi_1(args[0], &parse_res)) {
		Var* var = get_var(args[0]);
		if (var == NULL) {
			fprintf(stderr, "Error: Variable '%s' not declared.\n", args[0]);
			return;
		}
		test_num = var->value;
	} else {
		test_num = parse_res;
	}

	if (test_num == 0) {
		global_counter.program = find_label(args[1]) - 1;
	}
}
void jnz(Args args) {
	int parse_res;
	int test_num;

	if (atoi_1(args[0], &parse_res)) {
		Var* var = get_var(args[0]);
		if (var == NULL) {
			fprintf(stderr, "Error: Variable '%s' not declared.\n", args[0]);
			return;
		}
		test_num = var->value;
	} else {
		test_num = parse_res;
	}

	if (test_num != 0) {
		global_counter.program = find_label(args[1]) - 1;
	}
}
void jgz(Args args) {
	int parse_res;
	int test_num;

	if (atoi_1(args[0], &parse_res)) {
		Var* var = get_var(args[0]);
		if (var == NULL) {
			fprintf(stderr, "Error: Variable '%s' not declared.\n", args[0]);
			return;
		}
		test_num = var->value;
	} else {
		test_num = parse_res;
	}

	if (test_num > 0) {
		global_counter.program = find_label(args[1]) - 1;
	}
}
void jlz(Args args) {
	int parse_res;
	int test_num;

	if (atoi_1(args[0], &parse_res)) {
		Var* var = get_var(args[0]);
		if (var == NULL) {
			fprintf(stderr, "Error: Variable '%s' not declared.\n", args[0]);
			return;
		}
		test_num = var->value;
	} else {
		test_num = parse_res;
	}

	if (test_num < 0) {
		global_counter.program = find_label(args[1]) - 1;
	}
}
void err(Token token) {printf("Command not found: %s\n", token);}

Command commands[] = {
	{"MOV", mov, 2},
	{"ADD", add, 2},
	{"SUB", sub, 2},
	{"MUL", add, 2},
	{"JUMP", jump, 1},
	{"JEZ", jez, 2},
	{"JNZ", jnz, 2},
	{"JGZ", jgz, 2},
	{"JLZ", jlz, 2},
};

int find_label(Token token) {
	for (int i = 0; i < global_counter.label; i++) {
		if (strcmp(global_array.labels[i].label_name, token) == 0) {
			return global_array.labels[i].program_address;
		}
	}
	return -1;
}

int find_variable(Token token) {
	for (int i = 0; i < global_counter.variable; i++) {
		if (strcmp(global_array.variables[i].name, token) == 0) {
			return i;
		}
	}
	return -1;
}

int atoi_1 (const char* str, int *res) { 
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

#define MAX_LINE_LENGTH 1024
void parse_code(char* code) {
	char *line, *next_line;

	line = code;
	while ((next_line = strchr(line, '\n')) != NULL) {
		*next_line = '\0';
		parse_line(line);
		line = next_line + 1;
		global_counter.line++;
	}
}

Instruction* parse_line(char* line) {
	{
		size_t len = strlen(line);
		if (line[len - 1] == ':') {
			line[len - 1] = '\0';
			global_array.labels[global_counter.label].label_name = line;
			global_array.labels[global_counter.label].program_address = global_counter.instruction;
			global_counter.label++;
			return NULL;
		}
	}
	Args args;
	int i;
	const char* delim = " ,";
	Token token;
	token = strtok(strdup(line), delim);

	for (i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i) {
		if (strcmp(token, commands[i].token) == 0) {
			goto ok;
		}
	}
	goto err;
 ok:
	{
		int j = 0;
		while (true) {
			token = strtok(NULL, delim);
			if (token != NULL) {
				if (j >= commands[i].argc) {
					fprintf(stderr, "%d | %s <= "
							"%s expects %d arguments\n",
							global_counter.line, line, commands[i].token, commands[i].argc);
					exit(1);
				}
				args[j] = token;
				j++;
			} else break;
		}
		if (j < commands[i].argc) {
			fprintf(stderr, "%d | %s <= "
					"%s expects %d arguments\n",
					global_counter.line + 1, line, commands[i].token, commands[i].argc);
			exit(1);
		}
	}
	global_array.instructions[global_counter.instruction].function = commands[i].function;
	global_array.instructions[global_counter.instruction].args[0] = args[0];
	global_array.instructions[global_counter.instruction].args[1] = args[1];
	return &global_array.instructions[global_counter.instruction++];
 err:
	err(token);
	return NULL;
}

void run_line(char* line) {
	Instruction* instruction = parse_line(line);
	if (instruction != NULL)
		instruction->function(instruction->args);
}

void run_code() {
	while (P < global_counter.instruction) {
		global_array.instructions[P].function(global_array.instructions[P].args);
		P++;
	}
}

Var* get_var(Token var) {
	int i;
	i = find_variable(var);
	if (i == -1) {
		fprintf(stderr, "Error: Variable '%s' not declared.\n", var);
		return NULL;
	}
	return &global_array.variables[i];
}

void print_var(Token var) {
	printf("%d\n", get_var(var)->value); 
}

void print_env() {
	for (int i = 0; i < global_counter.variable; i++) {
		printf("%s = %d\n", global_array.variables[i].name, global_array.variables[i].value);
	}
}

int main() {
	char code[] =
		// factorial program
		"MOV R1, 10\n"
		"MOV R2, 0\n"
		"LOOP:\n"
		"ADD R2, R1\n"
		"SUB R1, 1\n"
		"JEZ R1, END\n"
		"JUMP LOOP\n"
		"END:\n";

	parse_code(code);
	run_code();
	print_var("R2");

	return 0;
}
