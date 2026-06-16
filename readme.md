oil in.oil out.o => take in, turn it into out. Must both end with correct file formats.
oil uses gcc's assembler to compile the language. In future, perhaps llvm?

syntax:

```python

function chtostr
takes (list(char) string)
returns tuple(list(char), int)
uses (int i = 0)
    while (string[i] != 0): i += 1;
    return tuple(string, i);
end

```
