#include <assert.h>
#include "lexed.h"
#include "token.h"
#include "da.h"

#ifndef function_h
#define function_h

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
    size_t len;
    sVar *data;
} sVarList;

typedef enum {
    COMMAND_ASSIGN_VAR_EXPR = 0,
    COMMAND_ASSIGN_INDEX_EXPR = 5, // TODO: Deprecate this, replace with lvalues in assign_var_expr
    COMMAND_CALL_EXPR = 10,
    COMMAND_WHILE = 20,
    COMMAND_IF = 30,
    COMMAND_ELSE = 40,
    COMMAND_ELIF = 50,
    COMMAND_BLOCK = 60,
    COMMAND_END = 70,
    COMMAND_RETURN = 80,
} eCommandType;

typedef struct {
    sToken name;
    size_t len;
    void *data;
} sExpr;

typedef struct {
    sExpr name, // should be sExpr for lvalues, but those aren't handled yet
          value;
} sAssignCommand;

typedef struct {
    sExpr value_check;
    size_t index;
} sComCommand;

typedef union {
    sAssignCommand assign;
    sExpr call, returns;

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
    size_t commands_len;
    sCommand *commands;
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
    out->data = malloc(sizeof(sType) * out_cap);
    assert(out->data != NULL);

    out->name = lexed.tokens[*idx];

    if (*idx >= lexed.len || out->name.type != TOKEN_IDENTIFIER) {
        *idx = start_idx;
        free_type(*out);
        return false;
    }

    if (token_has_text(out->name, "int") || token_has_text(out->name, "char") || token_has_text(out->name, "float")) {
        return true;
    } else if (token_has_text(out->name, "list") || token_has_text(out->name, "tuple")) {
        if ((*idx)++ >= lexed.len || lexed.tokens[*idx].type != TOKEN_SYMBOL || !token_has_text(lexed.tokens[*idx], "(")) {
            *idx = start_idx;
            free_type(*out);
            return false;
        }
        while (1) {
            sType first;
            bool ok = parse_type(lexed, globals, idx, &first);
            if (!ok) {
                *idx = start_idx;
                free_type(*out);
                return false;
            }
            da_append(out->data, out->len, out_cap, first);
            if (*idx >= lexed.len) {
                *idx = start_idx;
                free_type(*out);
                return false;
            }
            sToken next = lexed.tokens[(*idx)++];
            if (next.type != TOKEN_SYMBOL) {
                *idx = start_idx;
                free_type(*out);
                return false;
            }
            if (token_has_text(next, ")")) {
                break;
            } else if (token_has_text(next, ",")) {
                continue;
            }
            *idx = start_idx;
            free_type(*out);
            return false;
        }
        return true;
    } else {
        *idx = start_idx;
        free_type(*out);
        return false;
    }
}

static void free_var_list(sVarList vl) {
    for (size_t i = 0; i < vl.len; ++i) {
        free_type(vl.data[i].type);
    }
    free(vl.data);
}

static inline sVar parse_var(sLexed lexed, sGlobals *globals, size_t *idx) {
    sVar var;
    assert(parse_type(lexed, globals, idx, &var.type));
    assert(*idx < lexed.len);
    var.name = lexed.tokens[(*idx)++];
    return var;
}

static inline void free_expr(sExpr expr) {
    for (size_t i = 0; i < expr.len; ++i) {
        free_expr(((sExpr *) expr.data)[i]);
    }
    free(expr.data);
}

// precedence

static bool parse_expr_index(sLexed lexed, sGlobals *globals, size_t *idx, sExpr *expr) {
}

static bool parse_expr_prefix(sLexed lexed, sGlobals *globals, size_t *idx, sExpr *expr) {
}

static bool parse_expr_infix(sLexed lexed, sGlobals *globals, size_t *idx, sExpr *expr) {
}

static bool parse_expr_muldiv(sLexed lexed, sGlobals *globals, size_t *idx, sExpr *expr) {
}

static bool parse_expr_addsub(sLexed lexed, sGlobals *globals, size_t *idx, sExpr *expr) {
}

// end precedence

static inline bool parse_expr(sLexed lexed, sGlobals *globals, size_t *idx, sExpr *expr) {
    assert(false);
}

static inline sFunction parse_function(sLexed lexed, sGlobals *globals, size_t *idx) {
    // no going back
    sFunction function = { 0 };
    ++*idx; // skip "function"

    assert(lexed.len > *idx);
    function.name = lexed.tokens[(*idx)++];

    assert(lexed.len > *idx);
    assert(token_is(lexed.tokens[(*idx)++], "takes", TOKEN_IDENTIFIER));

    assert(lexed.len > *idx);
    assert(token_is(lexed.tokens[(*idx)++], "(", TOKEN_SYMBOL));
    ++*idx;
    size_t takes_cap = 16;
    function.takes.len = 0;
    function.takes.data = malloc(sizeof(sVar) * takes_cap);
    while (lexed.len > *idx && !token_is(lexed.tokens[*idx], ")", TOKEN_SYMBOL)) {
        sVar v = parse_var(lexed, globals, idx);
        da_append(function.takes.data, function.takes.len, takes_cap, v);
    }
    ++*idx;

    assert(lexed.len > *idx);
    assert(token_is(lexed.tokens[(*idx)++], "returns", TOKEN_IDENTIFIER));
    assert(parse_type(lexed, globals, idx, &function.returns));
    assert(lexed.len > *idx);

    assert(token_is(lexed.tokens[(*idx)++], "uses", TOKEN_IDENTIFIER));
    assert(lexed.len > *idx);
    assert(token_is(lexed.tokens[(*idx)++], "(", TOKEN_SYMBOL));
    size_t uses_cap = 16;
    function.uses.len = 0;
    function.uses.data = malloc(sizeof(sVar) * uses_cap);
    while (lexed.len > *idx && !token_is(lexed.tokens[*idx], ")", TOKEN_SYMBOL)) {
        sVar v = parse_var(lexed, globals, idx);
        da_append(function.uses.data, function.uses.len, uses_cap, v);
    }

    size_t commands_cap = 16;
    function.commands_len = 0;
    // function.commands stores the list of commands used by the function
    function.commands = malloc(sizeof(sCommand) * commands_cap);
    assert(function.commands != NULL);

    size_t block_idx_idx = 0;
    size_t block_idx_len = 0;

    size_t block_idx_cap = 16;
    size_t block_commands_cap = 16;
    // block_command_types stores the list of "types" per block beginning
    eCommandType *block_command_types = malloc(sizeof(eCommandType) * block_idx_cap); 
    // and block_indices stores the index of each block relative to the function
    size_t *block_indices = malloc(sizeof(size_t) * block_idx_cap);
    assert(block_indices != NULL);

    while (*idx < lexed.len) {
        size_t saved_before_command = *idx;
        sToken token = lexed.tokens[(*idx)++];
        if (token_is(token, ";", TOKEN_SYMBOL)) continue;
        bool ident = (token.type == TOKEN_IDENTIFIER);
        if (ident && token_has_text(token, "end")) {
            if (block_idx_len == 0) break; // end of function
            sCommand com = {
                .type = COMMAND_END,
                .data = { .trivial_com_index = block_indices[--block_idx_len] }
            };
            da_append(function.commands, function.commands_len, commands_cap, com);
        } else if (ident && token_has_text(token, "return")) {
            sExpr expr;
            assert(parse_expr(lexed, globals, idx, &expr));
            assert(lexed.len > *idx);
            assert(token_is(lexed.tokens[(*idx)++], ";", TOKEN_SYMBOL));
            assert(lexed.len > *idx);
            sCommand com = {
                .type = COMMAND_RETURN,
                .data = { .returns = expr }
            };
            da_append(function.commands, function.commands_len, commands_cap, com);
        } else if (ident && token_has_text(token, "if")) {
            da_append(block_command_types, block_idx_len, block_idx_cap, COMMAND_IF);
            da2_append(block_indices, block_idx_len, block_commands_cap, block_idx_idx);
            ++block_idx_idx;
            // no parenthesis needed
            sExpr expr;
            assert(parse_expr(lexed, globals, idx, &expr));
            assert(lexed.len > *idx);
            sCommand com = {
                .type = COMMAND_IF,
                .data = { .com_command = { .value_check = expr, .index = block_idx_idx - 1 } },
            };

            da_append(function.commands, function.commands_len, commands_cap, com);
        } else if (ident && token_has_text(token, "block")) {
            da_append(block_command_types, block_idx_len, block_idx_cap, COMMAND_BLOCK);
            da2_append(block_indices, block_idx_len, block_commands_cap, block_idx_idx);
            ++block_idx_idx;
            sCommand com = {
                .type = COMMAND_BLOCK,
                .data = { .trivial_com_index = block_idx_idx - 1 },
            };

            da_append(function.commands, function.commands_len, commands_cap, com);
        } else if (ident && token_has_text(token, "else")) {
            assert(block_idx_len > 0);
            assert(block_command_types[block_idx_len - 1] == COMMAND_IF || block_command_types[block_idx_len - 1] == COMMAND_ELIF);
            block_command_types[block_idx_len - 1] = COMMAND_ELSE;
            sCommand com = {
                .type = COMMAND_ELSE,
                .data = { .trivial_com_index = block_indices[block_idx_len - 1] }
            };

            da_append(function.commands, function.commands_len, commands_cap, com);
        } else if (ident && token_has_text(token, "elif")) {
            assert(block_idx_len > 0);
            assert(block_command_types[block_idx_len - 1] == COMMAND_IF || block_command_types[block_idx_len - 1] == COMMAND_ELIF);
            block_command_types[block_idx_len - 1] = COMMAND_ELIF;

            sExpr expr;
            assert(parse_expr(lexed, globals, idx, &expr));
            assert(lexed.len > *idx);
            sCommand com = {
                .type = COMMAND_ELIF,
                .data = { .com_command = { .value_check = expr, .index = block_indices[block_idx_len - 1] } },
            };

            da_append(function.commands, function.commands_len, commands_cap, com);
        } else if (ident && token_has_text(token, "while")) {
            da_append(block_command_types, block_idx_len, block_idx_cap, COMMAND_WHILE);
            da2_append(block_indices, block_idx_len, block_commands_cap, block_idx_idx);
            ++block_idx_idx;
            // no parenthesis needed
            sExpr expr;
            assert(parse_expr(lexed, globals, idx, &expr));
            assert(lexed.len > *idx);
            sCommand com = {
                .type = COMMAND_WHILE,
                .data = { .com_command = { .value_check = expr, .index = block_idx_idx - 1 } },
            };

            da_append(function.commands, function.commands_len, commands_cap, com);
        } else {
            // expression or assign
            sExpr expr;
            assert(parse_expr(lexed, globals, idx, &expr));
            assert(lexed.len > *idx);
            if (token_is(lexed.tokens[*idx], "=", TOKEN_SYMBOL)) {
                ++*idx;
                sExpr right_hand_side;
                assert(parse_expr(lexed, globals, idx, &right_hand_side));
                assert(lexed.len > *idx);
                // pattern match (token identifier) : (expression)
                if (token_is(expr.name, ":", TOKEN_SYMBOL)
                    && expr.len == 2
                    && ((sExpr *) expr.data)[0].name.type == TOKEN_IDENTIFIER
                    && ((sExpr *) expr.data)[0].len == 0) {
                    sCommand com = {
                        .type = COMMAND_ASSIGN_INDEX_EXPR,
                        .data = { .assign = { .name = expr, .value = right_hand_side } }
                    };
                    da_append(function.commands, function.commands_len, commands_cap, com);
                } else if (expr.name.type == TOKEN_IDENTIFIER && expr.len == 0) {
                    sCommand com = {
                        .type = COMMAND_ASSIGN_VAR_EXPR,
                        .data = { .assign = { .name = expr, .value = right_hand_side } }
                    };
                    da_append(function.commands, function.commands_len, commands_cap, com);
                } else {
                    assert(false);
                }
            } else {
                sCommand com = {
                    .type = COMMAND_CALL_EXPR,
                    .data = { .call = expr }
                };

                da_append(function.commands, function.commands_len, commands_cap, com);
            }

            assert(token_is(lexed.tokens[(*idx)++], ";", TOKEN_SYMBOL));
            assert(lexed.len > *idx);
        }
    }
    assert(block_idx_len == 0); // this is a genuine assertion, not error handling

    assert(lexed.len > *idx);
    // NO MEMORY LEAK
    free(block_command_types);
    free(block_indices);

    return function;
}

static inline sConstant parse_constant(sLexed lexed, sGlobals *globals, size_t *idx) {
    (*idx)++; // skip "const" term

    sConstant constant;
    constant.type = parse_var(lexed, globals, idx);
    assert(lexed.len > *idx);
    assert(token_is(lexed.tokens[(*idx)++], "=", TOKEN_SYMBOL));
    assert(lexed.len > *idx);
    assert(parse_expr(lexed, globals, idx, &constant.value));
    assert(lexed.len > *idx);

    return constant;
}

static inline sGlobals parse_globals(sLexed lexed) {
    size_t constants_cap = 16, functions_cap = 16;
    sGlobals globals = {
        .constants = malloc(sizeof(sConstant) * constants_cap),
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
            da_append(globals.functions, globals.functions_len, functions_cap, function);
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
