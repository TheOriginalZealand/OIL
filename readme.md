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
- semantics/type-checking
- codegen
