#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "text.h"

#ifndef lexed_h
#define lexed_h

typedef enum {
    TOKEN_SYMBOL = 0,
    TOKEN_IDENTIFIER = 1,
    TOKEN_NUMBER = 2,
    TOKEN_STRING = 3,
    TOKEN_SPACE = 4,
} eTokenType;

typedef struct {
    eTokenType type;
    size_t len;
    const char *text;
} sToken;

typedef struct {
    size_t len;
    sToken *tokens;
} sLexed;

const char lexer_list_of_symbols[][4] = {
    "==\0\0",
    "=\0\0\0",
    "<<=\0",
    ">>=\0",
    "<<\0\0",
    ">>\0\0",
    "<=\0\0",
    ">=\0\0",
    "<\0\0\0",
    ">\0\0\0",
    "&&\0\0",
    "||\0\0",
    "&=\0\0",
    "|=\0\0",
    "!=\0\0",
    "^=\0\0",
    "&\0\0\0",
    "|\0\0\0",
    "!\0\0\0",
    "^\0\0\0",
    "+=\0\0",
    "-=\0\0",
    "*=\0\0",
    "/=\0\0",
    "+\0\0\0",
    "-\0\0\0",
    "*\0\0\0",
    "/\0\0\0",
    "(\0\0\0",
    ")\0\0\0",
    "[\0\0\0",
    "]\0\0\0",
    ",\0\0\0",
    ":\0\0\0",
    ";\0\0\0",
};
#define lexer_num_of_symbols (sizeof(lexer_list_of_symbols) / sizeof(lexer_list_of_symbols[0]))
const char *lexer_list_of_symbol_chars = "<>=&|!^+-*/()[],:;";

static inline sToken lexer_lex_token(const char *text, bool *ok) {
    *ok = true;
    if (isdigit(text[0])) {
        return (sToken) { .type = TOKEN_NUMBER, .len = string_find_end_of_digits(text), .text = text };
    }
    if (isalpha(text[0]) || text[0] == '_') {
        return (sToken) { .type = TOKEN_IDENTIFIER, .len = string_find_end_of_identifier(text), .text = text };
    }
    if (isspace(text[0])) {
        return (sToken) { .type = TOKEN_SPACE, .len = string_find_end_of_whitespace(text), .text = text };
    }
    if (text[0] == '\"') {
        size_t len = string_find_end_of_string_literal(text, ok);
        if (!*ok) return (sToken) { 0 };
        return (sToken) { .type = TOKEN_STRING, .len = len, .text = text };
    }
    if (strchr(lexer_list_of_symbol_chars, text[0]) == NULL) {
        *ok = false;
        return (sToken) { 0 }; // anything, really
    }
    for (size_t i = 0; i < lexer_num_of_symbols; ++i) {
        size_t len = strlen(lexer_list_of_symbols[i]);
        if (strncmp(lexer_list_of_symbols[i], text, len) == 0) {
            return (sToken) { .type = TOKEN_SYMBOL, .len = len, .text = text };
        }
    }
    assert(false); // Unreachable
}

static inline sLexed lexer_lex_text(const char *text, bool *ok) {
    // pass through twice
    *ok = true;
    size_t cap = 16;
    sLexed lexed = { .len = 0 };
    const char *textptr1 = text;
    lexed.tokens = malloc(sizeof(sToken) * cap);
    if (lexed.tokens == NULL) {
        *ok = false;
        return (sLexed) { 0 };
    }

    while (*textptr1 != '\0') {
        sToken dummy = lexer_lex_token(textptr1, ok);
        if (!*ok) return (sLexed) { 0 };
        textptr1 += sizeof(char) * dummy.len;
        if (lexed.len + 1 > cap) {
            cap *= 2;
            void *data = realloc(lexed.tokens, sizeof(sToken) * cap);
            if (data == NULL) {
                free(lexed.data);
                *ok = false;
                return (sLexed) { 0 };
            }
            lexed.tokens = data;
        }
        lexed.tokens[lexed.len++] = dummy;
    }

    return lexed;
}

#endif
