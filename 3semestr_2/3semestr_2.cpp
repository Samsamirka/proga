#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream>

void addToken(char*, char* );
void nextToken();
int isType(char*);
int isValue(char*);
int isBaseType();
int structMembers();
int structDeclaration();
void lexer(char*);
char* readLinesFromFile(const char*);

// Структура для хранения информации о токене
typedef struct Token {
    char* type;
    char* value;
} Token;

// Глобальный указатель на текущий токен
Token* currentToken;

// Массив токенов
Token* tokens;
int tokenCount = 0;
int tokenCapacity = 100;

size_t size; // размер файла

// Функция для добавления токена в массив
void addToken(char* type, char* value) {
    if (tokenCount >= tokenCapacity) {
        tokenCapacity *= 2;
        tokens = (Token*)realloc(tokens, sizeof(Token) * tokenCapacity);
        if (tokens == NULL) {
            printf("Error!\n");
            free(tokens); // oсвобождаем выделенную ранее память
            exit(1);
        }
    }
    tokens[tokenCount].type = (char*)malloc(100);
    strcpy(tokens[tokenCount].type, type);
    tokens[tokenCount].value = (char*)malloc(strlen(value) + 1);
    strcpy(tokens[tokenCount].value, value);

    tokenCount++;
}

// Функция для получения следующего токена
void nextToken() {
    if (tokenCount > 0) {
        currentToken = &tokens[0];
        for (int i = 0; i < (tokenCount - 1); i++) {
            tokens[i] = tokens[i + 1];
        }
        tokenCount--;
    }
    else {
        // Достигнут конец массива токенов
        currentToken = NULL;
    }
}

// Проверка на тип токена
int isType(char* type) {
    if (currentToken != NULL && strcmp(currentToken->type, type) == 0) {
        return 1;
    }
    return 0;
}

// Проверка на значение токена
int isValue(char* value) {
    if (currentToken != NULL && strcmp(currentToken->value, value) == 0) {
        return 1;
    }
    return 0;
}

// Проверка на корректность типа данных
int isBaseType() {
    if (strcmp(currentToken->value, "int") == 0 || strcmp(currentToken->value, "float") == 0 || strcmp(currentToken->value, "char") == 0) return 1;
    return 0;
}

// Проверка на корректность членов структуры
int structMembers() {
    char* t = new char[5];
    memcpy(t, "TYPE", sizeof(char) * 5);
    char* i = new char[11];
    memcpy(i, "IDENTIFIER", sizeof(char) * 11);
    char* z = new char[2];
    memcpy(z, ";", sizeof(char) * 2);
    char* f = new char[2];
    memcpy(f, "}", sizeof(char) * 2);
    if (isType(t)) {
        if (isBaseType()) {
            nextToken();
            if (isType(i)) {
                nextToken();
                if (isValue(z)) {
                    nextToken();
                    if (isValue(f)) {
                        return 1;
                    }
                    if (structMembers()) {
                        return 1;
                    }
                }
            }
        }
        return 0;
    }
    else if (isValue(f)) {
        return 1;
    }
    else {
        return 0;
    }
}


// Проверка на корректность описания структуры
int structDeclaration() {
    char* i = new char[11];
    memcpy(i, "IDENTIFIER", sizeof(char) * 11);
    char* s = new char[7];
    memcpy(s, "struct", sizeof(char) * 7);
    char* o = new char[2];
    memcpy(o, "{", sizeof(char) * 2);
    char* z = new char[2];
    memcpy(z, "}", sizeof(char) * 2);
    currentToken = &tokens[0];
    if (isType(s)) {
        nextToken();
        if (isType(i)) {
            nextToken();
            if (isValue(o)) {
                nextToken();
                // Рекурсивный вызов для проверки членов структуры
                if (structMembers()) {
                    if (isValue(z)) {
                        nextToken();
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

// Функция лексера для разделения входного кода на токены
void lexer(char* input) {
    int size_bef = 0;
    char* token = strtok(input, " \n\t");
    char* s = new char[7];
    memcpy(s, "struct", sizeof(char) * 7);
    char* i = new char[11];
    memcpy(i, "IDENTIFIER", sizeof(char) * 11);
    char* t = new char[5];
    memcpy(t, "TYPE", sizeof(char) * 5);
    char* open = new char[10];
    memcpy(open, "BRACEOPEN", sizeof(char) * 10);
    char* close = new char[11];
    memcpy(close, "BRACECLOSE", sizeof(char) * 11);
    char* com = new char[6];
    memcpy(com, "COMMA", sizeof(char) * 6);
    char* eq = new char[6];
    memcpy(eq, "EQUAL", sizeof(char) * 6);
    while (token != NULL) {
        // Проверка на ключевые слова
        if (strcmp(token, "struct") == 0) {
            addToken(s, token);
        } else if (strcmp(token, "int") == 0 || strcmp(token, "float") == 0 || strcmp(token, "char") == 0)
            addToken(t, token);
        else {
            // Проверка на идентификаторы
            const char* braceopen = "{";
            const char* braceclose = "}";
            const char* comma = ";";
            const char* equal = "=";
            if (isalpha(token[0])) {
                size_bef = strlen(token);
                char* token_end = &(token[size_bef - 1]);
                char* token_copy = (char*)malloc(size_bef + 1); // +1 для нулевого символа
                if (token_copy == NULL) {
                    fprintf(stderr, "Ошибка выделения памяти\n");
                }
                strncpy(token_copy, token, size_bef - 1);
                token_copy[size_bef - 1] = '\0'; // Добавляем нулевой символ

                if (* braceopen == * token_end) {
                    addToken(i, token_copy);
                    strncpy(token_copy, token + size_bef - 1, 1);
                    token_copy[1] = '\0';
                    addToken(open, token_copy);
                    free(token_copy);
                }
                else if (*comma == *token_end) {
                    addToken(i, token_copy);
                    strncpy(token_copy, token + size_bef - 1, 1);
                    token_copy[1] = '\0';
                    addToken(com, token_copy);
                    free(token_copy);
                }
                else if (*braceclose == *token_end) {
                    addToken(i, token_copy);
                    strncpy(token_copy, token + size_bef - 1, 1);
                    token_copy[1] = '\0';
                    addToken(close, token_copy);
                    free(token_copy);
                }
                else if (*equal == *token_end) {
                    addToken(i, token_copy);
                    strncpy(token_copy, token + size_bef - 1, 1);
                    token_copy[1] = '\0';
                    addToken(eq, token_copy);
                    free(token_copy);
                }
                else addToken(i, token);
            } 
            else{
                // Проверка на символы
                if (*braceopen == *token) addToken(open, token);
                if (*braceclose == *token) addToken(close, token);
                if (*comma == *token) addToken(com, token);
                if (*equal == *token) addToken(eq, token);
            }
        }
        token = strtok(NULL, " \n\t");
    }
}

char* readLinesFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Ошибка открытия файла");
        return NULL;
    }

    size_t capacity = sizeof(char) * tokenCapacity;
    char* input = (char*)malloc(capacity); // Изначально выделяем 1024 байт
    if (input == NULL) {
        printf("Ошибка выделения памяти");
        fclose(fp);
        return NULL;
    }

    char line[1024]; // Временный буфер для чтения строки

    while (fgets(line, sizeof(line), fp) != NULL) {
        size_t len = strlen(line);
        // Увеличиваем размер выделенной памяти, если необходимо
        if ((size + len + 1) > capacity) {
            capacity *= 2; // Удваиваем емкость
            input = (char*)realloc(input, capacity);
            if (input == NULL) {
                perror("Ошибка выделения памяти");
                fclose(fp);
                return NULL;
            }
        }
        // Копируем строку в выделенную память
        memcpy(input + size, line, len);
        size += len;
    }

    if (ferror(fp)) {
        perror("Ошибка чтения из файла");
        fclose(fp);
        free(input);
        return NULL;
    }

    fclose(fp);

    // Уменьшаем выделенную память до фактического размера
    size++;
    input = (char*)realloc(input, size + 1); // +1 для завершающего '\0'
    input[size - 1] = '\0'; // Добавляем завершающий '\0'

    return input;
}

int main() {
    setlocale(LC_ALL, "Russian");
    char* input = new char[73];
    input = readLinesFromFile("input.txt");
    /*char* input = new char[73];
    memcpy(input, "struct Person { int age; char name; float height; }", sizeof(char) * 73);*/

    // Выделение памяти для массива токенов
    tokens = (Token*)malloc(sizeof(Token) * tokenCapacity);
    if (tokens == NULL) {
        fprintf(stderr, "Ошибка выделения памяти!\n");
        exit(1);
    }

    // Лексический анализ
    lexer(input);

    // Вывод токенов
    for (int i = 0; i < tokenCount; ++i) {
        printf("Token %d: Type = %s, Value = %s\n", i, tokens[i].type, tokens[i].value);
    }

    // Синтаксический анализ
    if (structDeclaration()) {
        printf("Описание структуры корректное.\n");
    } else {
        printf("Описание структуры некорректное.\n");
    }

    free(tokens);

    return 0;
}
