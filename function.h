#include <assert.h>
#include "lexed.h"
#include "da.h"

#ifndef function_h
#define function_h

#define token_has_text(t_, tex_) (t_.text_len == strlen(tex_) && strncmp(t_.text, tex_, t_.text_len) == 0)

typedef struct {
    sToken name;
    size_t len;
    void *data;
} sType;

typedef struct {
    // nothing yet
    char nothing_yet_;
} sTypeNamesList;

typedef struct {
    sToken name;
    sType type;
} sVar;

typedef struct {
    size_t vars_len;
    sVar *vars;
} sVarList;

typedef enum {
    COMMAND_ASSIGN_VAR_EXPR,
    COMMAND_CALL_EXPR,
    COMMAND_WHILE,
    COMMAND_IF,
    COMMAND_ELSE,
    COMMAND_ELSEIF,
    COMMAND_BLOCK,
    COMMAND_END,
} eCommandType;

typedef struct {
    sToken name;
    size_t len;
    void *data;
} sExpr;

typedef struct {
    sToken name; // should be sExpr for lvalues, but those aren't handled yet
    sExpr value;
} sAssignCommand;

typedef struct {
    sExpr value_check;
    size_t index;
} sComCommand;

typedef union {
    sAssignCommand assign;
    sExpr call;
    sComCommand com_command; // if, while, elif
    size_t trivial_com_index; // end, else, block
} uCommandData;

typedef struct {
    eCommandType type;
    uCommandData data;
} sCommand;

typedef struct {
    sToken name;
    sVarList takes, uses;
    sType returns;
    sCommand commands;
} sFunction;

typedef struct {
    sVar type;
    sExpr value;
} sConstant;

typedef struct {
    // NOTE: removed storage of order of functions and constants.
    // TODO: When we need to append functions (macros) store capacity in here.
    size_t constants_len;
    sConstant *constants;
    size_t functions_len;
    sFunction *functions;
    sTypeNamesList *types_list;
} sGlobals;

static inline void free_type(sType type) {
    for (size_t i = 0; i < type.len; ++i) {
        free_type(((sType *) type.data)[i]);
    }
    free(type.data);
}

// returns true/false depending on whether or not this is a type (so we may backtrack and then parse variables)
static inline bool parse_type(sLexed lexed, sGlobals *globals, size_t *idx, sType *out) {
    size_t start_idx = 0;

    size_t out_cap = 1;
    out->len = 0;
    out->data = malloc(sizeof(TypeNode) * out_cap);
    assert(out->data != NULL);

    out->token = lexed.tokens[*idx];

    if (idx >= lexed.len || out->token.type != TOKEN_IDENTIFIER) {
        *idx = start_idx;
        free_type(out);
        return false;
    }

    if (token_has_text(out->token, "int") || token_has_text(out->token, "char") || token_has_text(out->token, "float")) {
        return true;
    } else if (token_has_text(out->token, "list") || token_has_text(out->token, "tuple")) {
        if ((*idx)++ >= lexed.len || lexed->data[*idx].type != TOKEN_SYMBOL || !token_has_text(lexed->data[*idx], "(")) {
            *idx = start_idx;
            free_type(out);
            return false;
        }
        while (1) {
            sType first;
            bool ok = parse_type(lexed, globals, *idx, &first);
            if (!ok) {
                *idx = start_idx;
                free_type(out);
                return false;
            }
            da_append(((sType *) out->data), out->len, out_cap, first);
            if (*idx >= lexed.len) {
                *idx = start_idx;
                free_type(out);
                return false;
            }
            sToken next = lexed->data[(*idx)++];
            if (next.type != TOKEN_SYMBOL) {
                *idx = start_idx;
                free_type(out);
                return false;
            }
            if (token_has_text(next, ")")) {
                break;
            } else if (token_has_text(next, ",")) {
                continue;
            }
            *idx = start_idx;
            free_type(out);
            return false;
        }
        return true;
    } else {
        *idx = start_idx;
        free_type(out);
        return false;
    }
}

static void free_var_list(sVarList vl) {
    for (size_t i = 0; i < vl->len; ++i) {
        free_type(vl->vars[i].type);
    }
    free(vl->data);
}

static inline sVar parse_var(sLexed lexed, sGlobals *globals, size_t *idx) {
    sVar var;
    assert(parse_type(lexed, globals, idx, &var->type));
    assert(*idx < lexed->len);
    var->name = lexed->tokens[(*idx)++];
    return var;
}

static inline bool parse_expr(sLexed lexed, sGlobals *globals, size_t *idx, sExpr *expr) {
    assert(false);
}

static inline sFunction parse_function(sLexed lexed, sGlobals *globals, size_t *idx) {
    // no going back
    sFunction function = { 0 };
    ++*idx; // skip "function"
    assert(lexed->len < *idx);
    function->name = lexed->tokens[*idx];
    ++*idx;
    assert(lexed->len < *idx);
    assert(lexed->tokens[*idx].type == TOKEN_IDENTIFIER && token_has_text(lexed->tokens[*idx], "takes"));
    ++*idx;
    assert(lexed->len < *idx);
    assert(lexed->tokens[*idx].type == TOKEN_SYMBOL && token_has_text(lexed->tokens[*idx], "("));
    ++*idx;
    size_t takes_cap = 16;
    function->takes.len = 0;
    function->takes.data = malloc(sizeof(sVar) * takes_cap);
    while (lexed->len < *idx && !(lexed->tokens[*idx].type == TOKEN_SYMBOL && token_has_text(lexed->tokens[*idx], ")"))) {
        sVar v = parse_var(lexed, globals, idx);
        da_append(function->takes.data, function->takes.len, takes_cap, v);
    }
    assert(lexed->len < *idx);
    assert(lexed->tokens[*idx].type == TOKEN_IDENTIFIER && token_has_text(lexed->tokens[*idx], "returns"));
    ++*idx;
    assert(parse_type(lexed, globals, idx, &function->returns));
    assert(lexed->len < *idx);
    assert(lexed->tokens[*idx].type == TOKEN_IDENTIFIER && token_has_text(lexed->tokens[*idx], "uses"));
    ++*idx;
    assert(lexed->len < *idx);
    assert(lexed->tokens[*idx].type == TOKEN_SYMBOL && token_has_text(lexed->tokens[*idx], "("));
    ++*idx;
    size_t uses_cap = 16;
    function->uses.len = 0;
    function->uses.data = malloc(sizeof(sVar) * uses_cap);
    while (lexed->len < *idx && !(lexed->tokens[*idx].type == TOKEN_SYMBOL && token_has_text(lexed->tokens[*idx], ")"))) {
        sVar v = parse_var(lexed, globals, idx);
        da_append(function->uses.data, function->uses.len, uses_cap, v);
    }

    size_t commands_cap = 16;
    function->commands_len = 0;
    function->commands = malloc(sizeof(sCommand) * commands_cap);
    assert(function->commands != NULL);
    size_t block_idx_len = 0;
    size_t block_idx_cap = 0;
    size_t *block_indices = malloc(sizeof(size_t) * block_idx_cap);
    assert(block_indices != NULL);

    while (!(lexed->data[*idx].type == TOKEN_IDENTIFIER && token_has_text(lexed->tokens[*idx], "end"))) {
    }

    free(block_indices); // NO MEMORY LEAK
}

static inline sConstant parse_constant(sLexed lexed, sGlobals *globals, size_t *idx) {
    assert(false);
}

static inline sGlobals parse_globals(sLexed lexed) {
    size_t constants_cap = 16, functions_cap = 16;
    sGlobals globals = {
        .constants = malloc(sizeof(sConstant) * constants_cap)
        .functions = malloc(sizeof(sFunction) * functions_cap)
    };
    assert(globals.constants != NULL);
    assert(globals.functions != NULL);

    size_t idx = 0;
    while (idx < lexed.len) {
        sToken token = lexed.tokens[idx];
        // TODO: Proper error message handling
        // TODO: Error message line/file
        assert(token.type == TOKEN_IDENTIFIER);

        // token.type == TOKEN_IDENTIFIER

        if (token_has_text(token, "function")) {
            sFunction function = parse_function(lexed, &globals, &idx);
            da_append(globals.functions, globals.functions_len, functions_cap, constant);
        } else if (token_has_text(token, "const")) {
            sConstant constant = parse_constant(lexed, &globals, &idx);
            da_append(globals.constants, globals.constants_len, constants_cap, constant);
        } else {
            // TODO: error checking
            assert(false);
        }
    }

    return globals;
}

#endif
