#include "tokens.h"

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
    COMMAND_ENDWHILE,
    COMMAND_IF,
    COMMAND_ELSE,
    COMMAND_ELSEIF,
    COMMAND_ENDIF,
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

typedef union {
    sAssignCommand assign;
    sExpr call;
    sComCommand com_command;
    size_t trivial_com_index;
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
} sConst;

typedef enum {
    GLOBAL_FUNCTION,
    GLOBAL_CONST,
} eGlobalType;

typedef union {
    sConst *constant;
    sFunction *function;
} uGlobalData;

typedef struct {
    eGlobalType type;
    uGlobalData data;
} sGlobalRef;

typedef struct {
    size_t globals_len;
    sGlobalRef globals;
    size_t constants_len;
    sConstant *constants;
    size_t functions_len;
    sFunction *functions;
} sGlobals;

#endif
