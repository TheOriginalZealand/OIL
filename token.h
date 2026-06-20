#include "lexed.h"
#include <string.h>
#include <stdbool.h>

#ifndef token_h
#define token_h

static inline bool token_has_text(sToken t, const char *tex) {
    return t.len == strlen(tex) && strncmp(t.text, tex, t.len) == 0;
}

static inline bool token_is(sToken token, const char *text, eTokenType type) {
    return token.type == type && token_has_text(token, text);
}

#endif
