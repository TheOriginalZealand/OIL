#include <assert.h>
#include "lexed.h"

#ifndef function_h
#define function_h

typedef struct {
    sToken name;
    size_t arguments;
} sTypeNode;

typedef struct {
    size_t len;
    sTypeNode *data;
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
    size_t arguments;
} sExprNode;

typedef struct {
    size_t len;
    sExprNode *data;
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
    size_t name_len;
    const char *name;

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

// returns true/false depending on whether or not this is a type (so we may backtrack and then parse variables)
static inline bool parse_type(sLexed lexed, sGlobals *globals, size_t *idx, sType *out) {
    assert(false);
}

// parsing commands requires previous command information, so no parse_command
static inline sFunction parse_function(sLexed lexed, sGlobals *globals, size_t *idx) {
    assert(false);
}

static inline sConst parse_constant(sLexed lexed, sGlobals *globals, size_t *idx) {
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
        if (token.type != TOKEN_IDENTIFIER) {
            // TODO: Proper error message handling
            // TODO: Error message line/file
            fprintf(stderr, "ERROR: Expected identifier");
            exit(1);
        }

        // token.type == TOKEN_IDENTIFIER

        if (strcmp(token.text, "function") == 0) {
            if (globals.functions_len + 1 > functions_cap) {
                functions_cap *= 2;
                void *data = realloc(globals.functions, sizeof(sFunction) * functions_cap);
                assert(data != NULL);
                globals.functions = data;
            }
            globals.functions[globals.functions_len++] = parse_function(lexed, &globals, &idx);
        } else if (strcmp(token.text, "const") == 0) {
            if (globals.constants_len + 1 > constants_cap) {
                functions_cap *= 2;
                void *data = realloc(globals.constants, sizeof(sConstant) * constants_cap);
                assert(data != NULL);
                globals.functions = data;
            }
            globals.constants[globals.constants_len++] = parse_function(lexed, &globals, &idx);
        } else {
            // TODO: error checking
            fprintf(stderr, "ERROR: Incorrect identifier");
            exit(1);
        }
    }

    return globals;
}

#endif
