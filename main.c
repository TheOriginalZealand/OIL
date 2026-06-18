#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "lexed.h"

int main(int argc, char **argv) {
    assert(argc == 3);

    int text_len;
    char *file_text;

    char *in = argv[1], *out = argv[2];

    {
        FILE *file = fopen(in, "r");
        assert(file != NULL);
        fseek(file, 0, SEEK_END);
        text_len = ftell(file);
        fseek(file, 0, SEEK_SET);

        assert((file_text = malloc(sizeof(char) * (text_len + 1))) != NULL);
        fread(file_text, text_len, 1, file);
        file_text[text_len] = '\0';

        fclose(file);
    }

    bool ok;

    sLexed lexed = lexer_lex_text(file_text, &ok);

    if (!ok) {
        fprintf(stderr, "Failed to lex text (invalid token or something)!\n");
        // No memory cleanup here because:
        // a. it's done automatically after exiting
        // b. this program is not that advanced yet
        exit(1);
    }

    for (size_t i = 0; i < lexed.len; ++i) {
        fprintf(stderr, "%.*s\n", (int) lexed.tokens[i].len, lexed.tokens[i].text);
    }

    free(lexed.tokens);

    free(file_text);
    return 0;
}
