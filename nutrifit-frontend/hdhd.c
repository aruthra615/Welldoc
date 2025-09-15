#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct rec {
    int ino;
    char lexeme[20];
    char token[20];
};

struct rec symtab[100] = {
    {1, "for", "FOR"},
    {2, "if", "IF"},
    {3, "else", "ELSE"},
    {4, "while", "WHILE"},
    {5, "int", "INT"},
    {6, "then", "THEN"},
    {7, "return", "RETURN"},
    {8, "bool", "BOOL"},
    {9, "true", "TRUE"},
    {10, "false", "FALSE"},
    {11, "break", "BREAK"},
    {12, "cin", "CIN"},
    {13, "cout", "COUT"},
    {14, "void", "VOID"},
    {15, "include", "INCLUDE"},
    {16, "using", "USING"},
    {17, "namespace", "NAMESPACE"},
    {18, "std", "STD"},
    {19, "sizeof", "SIZEOF"}
};

int symlen = 19;

int isOperator(char *ch) {
    char ops[10][3] = {"==", "!=", "<=", ">=", "&&", "||", "++", "--", "<<", ">>"};
    for (int i = 0; i < 10; i++) {
        if (strncmp(ch, ops[i], 2) == 0)
            return 2;
    }
    return 0;
}

int isKeyword(char *str) {
    for (int x = 0; x < symlen; x++) {
        if(strcmp(symtab[x].lexeme, str) == 0)
            return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.cpp>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    char literal[10000];
    size_t len = fread(literal, 1, sizeof(literal) - 1, fp);
    literal[len] = '\0';
    fclose(fp);

    char *ch = literal;

    while (*ch != '\0') {
        if (isspace(*ch)) {
            ch++;
            continue;
        }

        if (*ch == '/' && *(ch + 1) == '/') {
            ch += 2;
            while (*ch != '\0' && *ch != '\n') ch++;
            continue;
        }

        if (*ch == '#') {
            char str[100];
            int i = 0;
            str[i++] = *ch++;
            while (*ch != '\n' && *ch != '\0' && *ch != '\r') {
                str[i++] = *ch++;
            }
            str[i] = '\0';
            printf("%s | Preprocessor | -\n", str);
            continue;
        }

        if (isalpha(*ch) || *ch == '_') {
            char str[50];
            int i = 0;
            while (isalnum(*ch) || *ch == '_')
                str[i++] = *ch++;
            str[i] = '\0';

            char *lookahead = ch;
            while (isspace(*lookahead)) lookahead++;
            if (*lookahead == '(') {
                if (!isKeyword(str)) {
                    printf("%s | Function | -\n", str);
                } else {
                    for (int x = 0; x < symlen; x++) {
                        if(strcmp(symtab[x].lexeme, str) == 0) {
                            printf("%s | %s | %d\n", str, symtab[x].token, x);
                            break;
                        }
                    }
                }
                continue;
            }

            int flag = 0;
            for (int x = 0; x < symlen; x++) {
                if(strcmp(symtab[x].lexeme, str) == 0) {
                    flag = 1;
                    printf("%s | %s | %d\n", symtab[x].lexeme, symtab[x].token, x);
                    break;
                }
            }
            if (!flag) {
                strcpy(symtab[symlen].lexeme, str);
                strcpy(symtab[symlen].token, "id");
                symtab[symlen].ino = symlen++;
                printf("%s | id | %d\n", str, symlen - 1);
            }
        }

        else if (isdigit(*ch)) {
            char str[20];
            int i = 0;
            while(isdigit(*ch) || *ch == '.')
                str[i++] = *ch++;
            str[i] = '\0';
            printf("%s | Number | -\n", str);
        }

        else if (*ch == '"') {
            char str[100];
            int i = 0;
            str[i++] = *ch++;
            while(*ch != '"' && *ch != '\0')
                str[i++] = *ch++;
            if (*ch == '"') str[i++] = *ch++;
            str[i] = '\0';
            printf("%s | String | -\n", str);
        }

        else if (*ch == ':' && *(ch+1) == ':') {
            printf(":: | Scope Resolution | -\n");
            ch += 2;
        }

        else if (isOperator(ch) == 2) {
            printf("%.2s | Operator | -\n", ch);
            ch += 2;
        }

        else if (ch == '=' || *ch == '+' || *ch == '-' || *ch == '' ||
                 *ch == '/' || *ch == '<' || *ch == '>' || *ch == '%' ||
                 *ch == '&' || *ch == '|') {
            printf("%c | Operator | -\n", *ch);
            ch++;
        }

        else if (*ch == ';' || *ch == ',' || *ch == '(' || *ch == ')' ||
                 *ch == '{' || *ch == '}' || *ch == '[' || *ch == ']') {
            printf("%c | Punctuation | -\n", *ch);
            ch++;
        }

        else {
            ch++;
        }
    }

    return 0;
}