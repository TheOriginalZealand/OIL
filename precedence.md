# Precedence, lowest to greatest
- addition/subtraction
- multiplication/division
- all bitwise infix operators, as well as boolean infix operators
- all prefix expressions
- index expressions
- variable names/function calls (yes, these come before index expressions)

## Examples
```js
function main
takes (int argc list(list(char)) argv)
returns int
uses (int a)
    a = 2 & 1 * 3 / 4 + 10 * 6 << 2 / 8;
    // a = ((2 & 1) * (3 / 4)) + (10 * ((6 << 2) / 8)));
    modify_int(&a);
    if 2 < 1 + 1 print("Hello, World!"); end // prints hello world :)
    // if (2 < 1) + 1 // 0 + 1
    //     print("Hello, World!");
    // end
    return 0;
end
```
