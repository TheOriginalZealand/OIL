#ifndef function_h
#define function_h

typedef struct {
    size_t name_len;
    const char *name;

    size_t arguments;
} sTypeNode;

typedef struct {
    size_t type_names_len;
    sTypeNode *type_names_rpn;
} sType;

typedef struct {
    size_t name_len;
    const char *name;
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
} sAssignCommand;

typedef union {
    sAssignCommand assign_command;
    sCallCommand call_command;
    sWhileCommand while_command;
    sIfCommand if_command;
    sElseIfCommand else_if_command;
    size_t end_if_index;
    size_t end_while_index;
    size_t else_index;
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

#endif
