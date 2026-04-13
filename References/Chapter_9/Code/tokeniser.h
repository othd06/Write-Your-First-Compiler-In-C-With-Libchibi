
/************************
* File: tokeniser.h
************************/

#ifndef TOKENISER_H
#define TOKENISER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "helpers.h"

typedef enum {
    TOK_STRING_LITERAL,
    TOK_BOOL_LITERAL,
    TOK_INTEGER_LITERAL,
    TOK_DECIMAL_LITERAL,
    TOK_IDENTIFIER,
    TOK_KEYWORD_STRING,
    TOK_KEYWORD_F80,
    TOK_KEYWORD_F64,
    TOK_KEYWORD_F32,
    TOK_KEYWORD_U64,
    TOK_KEYWORD_U32,
    TOK_KEYWORD_U16,
    TOK_KEYWORD_U8,
    TOK_KEYWORD_I64,
    TOK_KEYWORD_I32,
    TOK_KEYWORD_I16,
    TOK_KEYWORD_I8,
    TOK_KEYWORD_BOOL,
    TOK_KEYWORD_BODY,
    TOK_KEYWORD_PROC,
    TOK_KEYWORD_DEFINE,
    TOK_KEYWORD_RETURN,
    TOK_KEYWORD_EXTERN,
    TOK_KEYWORD_VAR,
    TOK_KEYWORD_ASSIGN,
    TOK_KEYWORD_IF,
    TOK_KEYWORD_ELIF,
    TOK_KEYWORD_ELSE,
    TOK_KEYWORD_WHILE,
    TOK_KEYWORD_BREAK,
    TOK_KEYWORD_CONTINUE,
    TOK_KEYWORD_LOG,
    TOK_KEYWORD_AND,
    TOK_KEYWORD_OR,
    TOK_KEYWORD_NOT,
    TOK_KEYWORD_XOR,
    TOK_KEYWORD_BIT,
    TOK_KEYWORD_SHL,
    TOK_KEYWORD_SHR,
    TOK_KEYWORD_VOID,
    TOK_KEYWORD_PTR,
    TOK_KEYWORD_TO,
    TOK_KEYWORD_FROM,
    TOK_KEYWORD_ALLOCATE,
    TOK_KEYWORD_OF,
    TOK_KEYWORD_FREE,
    TOK_KEYWORD_SIZEOF,
    TOK_KEYWORD_CAST,
    TOK_L_PAREN,
    TOK_R_PAREN,
    TOK_SYMBOL_PLUS,
    TOK_SYMBOL_MINUS,
    TOK_SYMBOL_TIMES,
    TOK_SYMBOL_DIVIDE,
    TOK_SYMBOL_MODULO,
    TOK_SYMBOL_EQ,
    TOK_SYMBOL_NEQ,
    TOK_SYMBOL_GT,
    TOK_SYMBOL_GE,
    TOK_SYMBOL_LT,
    TOK_SYMBOL_LE,
    TOK_EOF,
} TokenKind;

typedef struct {
    TokenKind kind;
    union {
        char* string_literal;
        bool bool_literal;
        long integer_literal;
        long double decimal_literal;
        char* identifier;
    } data;
} Token;

DeclareSeq(Token, Token_Seq);
DeclareSeq(char, char_Seq);

Token_Seq tokenise(char* plaintext);


#endif

#ifdef TOKENISER_C
#undef TOKENISER_C

Token_Seq tokenise(char* plaintext) {
    int index = 0;
    Token_Seq tokens = {0};
    char* keyword_strings[] = {"string", "f80", "f64",
        "f32", "u64", "u32", "u16", "u8", "i64", "i32",
        "i16", "i8", "bool", "body", "proc", "define",
        "return", "extern", "var", "assign", "if", "elif",
        "else", "while", "break", "continue", "log", "and",
        "or", "not", "xor", "bit", "shl", "shr", "void",
        "ptr", "to", "from", "allocate", "of", "free",
        "sizeof", "cast"};
    TokenKind keyword_kinds[] = {TOK_KEYWORD_STRING,
        TOK_KEYWORD_F80, TOK_KEYWORD_F64,
        TOK_KEYWORD_F32, TOK_KEYWORD_U64,
        TOK_KEYWORD_U32, TOK_KEYWORD_U16,
        TOK_KEYWORD_U8, TOK_KEYWORD_I64,
        TOK_KEYWORD_I32, TOK_KEYWORD_I16,
        TOK_KEYWORD_I8, TOK_KEYWORD_BOOL,
        TOK_KEYWORD_BODY, TOK_KEYWORD_PROC,
        TOK_KEYWORD_DEFINE, TOK_KEYWORD_RETURN,
        TOK_KEYWORD_EXTERN, TOK_KEYWORD_VAR,
        TOK_KEYWORD_ASSIGN, TOK_KEYWORD_IF,
        TOK_KEYWORD_ELIF, TOK_KEYWORD_ELSE,
        TOK_KEYWORD_WHILE, TOK_KEYWORD_BREAK,
        TOK_KEYWORD_CONTINUE, TOK_KEYWORD_LOG,
        TOK_KEYWORD_AND, TOK_KEYWORD_OR,
        TOK_KEYWORD_NOT, TOK_KEYWORD_XOR,
        TOK_KEYWORD_BIT, TOK_KEYWORD_SHL,
        TOK_KEYWORD_SHR, TOK_KEYWORD_VOID,
        TOK_KEYWORD_PTR, TOK_KEYWORD_TO,
        TOK_KEYWORD_FROM, TOK_KEYWORD_ALLOCATE,
        TOK_KEYWORD_OF, TOK_KEYWORD_FREE,
        TOK_KEYWORD_SIZEOF, TOK_KEYWORD_CAST};
    
    while (plaintext[index] != '\0') {
        if (isalpha(plaintext[index])) {
            char_Seq token_string = {0};
            while (isalpha(plaintext[index]) || isdigit(plaintext[index]) || plaintext[index] == '_') {
                append(char_Seq, token_string, plaintext[index++]);
            }
            append(char_Seq, token_string, '\0');
            int keyword_index = string_in_array(token_string.data, keyword_strings, sizeof(keyword_strings)/sizeof(char*));
            if (strcmp(token_string.data, "true") == 0) {
                Token new_token;
                new_token.kind = TOK_BOOL_LITERAL;
                new_token.data.bool_literal = true;
                append(Token_Seq, tokens, new_token);
            } else if (strcmp(token_string.data, "false") == 0) { 
                Token new_token;
                new_token.kind = TOK_BOOL_LITERAL;
                new_token.data.bool_literal = false;
                append(Token_Seq, tokens, new_token);
            }else if (keyword_index == -1) {
                Token new_token;
                new_token.kind = TOK_IDENTIFIER;
                new_token.data.identifier = token_string.data;
                append(Token_Seq, tokens, new_token);
            } else {
                Token new_token;
                new_token.kind = keyword_kinds[keyword_index];
                append(Token_Seq, tokens, new_token);
            }
        } else if (isdigit(plaintext[index])) {
            long token_integer_1 = 0;
            while (isdigit(plaintext[index])) {
                token_integer_1 *= 10;
                token_integer_1 += (long)plaintext[index++]-48;
            }
            if (plaintext[index] != '.') {
                Token new_token;
                new_token.kind = TOK_INTEGER_LITERAL;
                new_token.data.integer_literal = token_integer_1;
                append(Token_Seq, tokens, new_token);
            } else {
                index++;
                
                long token_integer_2 = 0;
                while (isdigit(plaintext[index])) {
                    token_integer_2 *= 10;
                    token_integer_2 += (long)plaintext[index++]-48;
                }

                long double token_float = (long double)token_integer_2;
                while(token_float >= 1.0) token_float /= 10;
                token_float += (long double)token_integer_1;
                
                Token new_token;
                new_token.kind = TOK_DECIMAL_LITERAL;
                new_token.data.decimal_literal = token_float;
                append(Token_Seq, tokens, new_token);
            }
        } else if (isspace(plaintext[index])) {
            index++;
        } else if (plaintext[index] == '(') {
            index++;
            Token new_token;
            new_token.kind = TOK_L_PAREN;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == ')') {
            index++;
            Token new_token;
            new_token.kind = TOK_R_PAREN;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '+') {
            index++;
            Token new_token;
            new_token.kind = TOK_SYMBOL_PLUS;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '-') {
            index++;
            Token new_token;
            new_token.kind = TOK_SYMBOL_MINUS;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '*') {
            index++;
            Token new_token;
            new_token.kind = TOK_SYMBOL_TIMES;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '/') {
            index++;
            Token new_token;
            if (plaintext[index] == '=') {
                index++;
                new_token.kind = TOK_SYMBOL_NEQ;
            } else {
                new_token.kind = TOK_SYMBOL_DIVIDE;
            }
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '%') {
            index++;
            Token new_token;
            new_token.kind = TOK_SYMBOL_MODULO;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '=') {
            index++;
            Token new_token;
            new_token.kind = TOK_SYMBOL_EQ;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '>') {
            index++;
            Token new_token;
            if (plaintext[index] == '=') {
                index++;
                new_token.kind = TOK_SYMBOL_GE;
            } else {
                new_token.kind = TOK_SYMBOL_GT;
            }
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '<') {
            index++;
            Token new_token;
            if (plaintext[index] == '=') {
                index++;
                new_token.kind = TOK_SYMBOL_LE;
            } else {
                new_token.kind = TOK_SYMBOL_LT;
            }
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == '"') {
            index++; //consume the opening quote
            char_Seq token_string = {0};
            while ((token_string.len > 0 && token_string.data[token_string.len-1] == '\\') || plaintext[index] != '"') {
                append(char_Seq, token_string, plaintext[index++]);
            }
            append(char_Seq, token_string, '\0');
            index++; // consume the closing quote
            Token new_token;
            new_token.kind = TOK_STRING_LITERAL;
            new_token.data.string_literal = token_string.data;
            append(Token_Seq, tokens, new_token);
        } else if (plaintext[index] == ';') {
            int strength = 0;
            while (plaintext[index] == ';') {
                strength++;
                index++;
            }
            not_strong_enough:
            while (plaintext[index] != ';') {
                if (plaintext[index] == '\0') {
                    fprintf(stderr, "comment hit eof. Did you forget to terminate it?\n");
                    exit(1);
                }
                index++;
            }
            for (int i = 0; i<strength; i++) {
                if (plaintext[index] != ';') goto not_strong_enough;
                index++;
            }
        } else {
            fprintf(stderr, "Error: leading token character not recognised\n");
            exit(1);
        }
    }

    Token eof_token;
    eof_token.kind = TOK_EOF;
    append(Token_Seq, tokens, eof_token);
    return tokens;
}

#endif

