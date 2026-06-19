# OIL's command-line syntax
oil in.oil out.o => take in, turn it into out. Must both end with correct file formats.
oil uses gcc's assembler to compile the language. In future, perhaps llvm?

# Oil's syntax:

```js
function chtostr
takes (list(char) string)
returns tuple(list(char), int)
uses (int i = 0)
    while (string[i] != 0): i += 1;
    return tuple(string, i);
end

```

# OIL's compiler:
## Stages:
Oil's compiler has four stages:
- lex (while checking lexical correctness)
- parse (while checking well-formedness)
- type-checking
- optimisation (add later)
- codegen (while checking if constants are compile-time or not)
## AST representation
- expressions and types are stored recursively. commands are so simple, that they are not.
- therefore, when we (programmers) do optimisation later, we will perform pattern-matching using tree-values for expressions
- and pattern-matching similar to regex for code
- and a combination between the two for the two together
