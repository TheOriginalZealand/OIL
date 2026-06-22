# OIL's command-line syntax
oil in.oil out.o => take in, turn it into out. Must both end with correct file formats.
oil uses gcc's assembler to compile the language. In future, perhaps llvm?

# Oil's syntax:

```js
// (char i)
    if some_random_function() i = 1; else i = 0; end
    block
        i += 1
        print_i_somehow_function(i)
        if i < 10 continue; else break;
    end
```
^ This is an example of a block (if/elif, block are all valid)

```js
const char _nullch = 0;
// const char _a = 97;
// const char _A = 65;

function chtostr
takes (list(char) string)
returns tuple(list(char), int)
uses (int i)
    i = 0;
    while string:i != _nullch i += 1; end
    // while *(string + i * sizeof(char)) != _nullch i += 1; end
    return tuple(string, i);
end

function vim_g_ctrla_array
takes (list(int) array
       int array_len)
returns void
uses (int i)
    i = 0;
    while i < array_len
        array:i += i;
    end
end
```

^ This is how functions and constants are declared. The language has no formal grammar for syntax, so just look it up in the compiler's lexing/parsing code, or whatever.
## Notice
- while loops don't (any longer) have colons following the condition
- tuple is used both as a type and an expression.
- variables are only declared at the beginning of the function.
- arrays are indexed similarly to B, with array:i => array[i] in C.
- one may also add to arrays and then dereference \*(array + i\*sizeof(type))
- currently, one may use (and only use - not on array indices) sizeof on types.
- we will add lvalues later. until then, array/index expressions are disgustingly simple.
- function arguments are not separated by commas, but type arguments are. this is inspired by SQL.
- void is a type and alias for char.
- functions may not be called as types, as they do not exist as types.
## Notes
- characters are 8-bit.
- integers are 32-bit.

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
- therefore, when we (programmers) add optimisation later, we will perform pattern-matching using tree-values for expressions
- and pattern-matching similar to regex for code
- and a combination between the two for the two together

# Note on usage
WARNING: DO NOT USE FOR COMMERCIAL PURPOSES; THIS LANGUAGE IS LITERALLY MADE BY TEENAGERS. EVEN THE COMPILER IS UNSAFE. 
