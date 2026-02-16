| SL NO | Keyword / Symbol (AstroScript) | String / Keyword / Symbol in C / C++ | Description                           |   |            |
| ----: | ------------------------------ | ------------------------------------ | ------------------------------------- | - | ---------- |
|     1 | `mission`                      | `main()`                             | Entry point of an AstroScript program |   |            |
|     2 | `launch`                       | start of `main`                      | Begins mission execution              |   |            |
|     3 | `success`                      | `return 0;`                          | Successful mission termination        |   |            |
|     4 | `abort`                        | `exit(1)`                            | Abnormal mission termination          |   |            |
|     5 | `count`                        | `int`                                | Integer type for discrete values      |   |            |
|     6 | `real`                         | `float`                              | Floating-point type                   |   |            |
|     7 | `precise`                      | `double`                             | High-precision numeric type           |   |            |
|     8 | `flag`                         | `bool`                               | Boolean signal type                   |   |            |
|     9 | `symbol`                       | `char`                               | Single character type                 |   |            |
|    10 | `voidspace`                    | `void`                               | No return type                        |   |            |
|    11 | `telemetry <type> <id>`        | variable                             | Mutable variable / class member       |   |            |
|    12 | `limit <type> <id>`            | `const`                              | Immutable constant / const member     |   |            |
|    13 | `:=`                           | `=`                                  | Assignment operator                   |   |            |
|    14 | `.`                            | `;`                                  | End of statement                      |   |            |
|    15 | `broadcast()`                  | `printf()`                           | Output to console                     |   |            |
|    16 | `receive()`                    | `scanf()`                            | Input from user                       |   |            |
|    17 | `alarm()`                      | `printf()`                           | Critical warning output               |   |            |
|    18 | `$$`                           | `//`                                 | Single-line comment                   |   |            |
|    19 | `$* ... *$`                    | `/* ... */`                          | Multi-line comment                    |   |            |
|    20 | `command name(...) : type`     | method / function                    | Defines a function or class method    |   |            |
|    21 | `back`                         | `return`                             | Return statement                      |   |            |
|    22 | `verify (condition)`           | `if`                                 | Conditional check                     |   |            |
|    23 | `else_verify (condition)`      | `else if`                            | Alternate condition                   |   |            |
|    24 | `otherwise`                    | `else`                               | Default condition                     |   |            |
|    25 | `orbit (condition)`            | `while`                              | Loop with condition                   |   |            |
|    26 | `orbit times (i:c:u)`          | `for`                                | Planned / not in current parser       |   |            |
|    27 | `stage_sep`                    | `break`                              | Exit loop                             |   |            |
|    28 | `coast`                        | `continue`                           | Skip loop iteration                   |   |            |
|    29 | `scenario (expression)`        | `switch`                             | Multi-path decision                   |   |            |
|    30 | `trajectory`                   | `case`                               | Switch case                           |   |            |
|    31 | `fallback`                     | `default`                            | Default case                          |   |            |
|    32 | `add`                          | `+`                                  | Addition                              |   |            |
|    33 | `minus`                        | `-`                                  | Subtraction                           |   |            |
|    34 | `mul`                          | `*`                                  | Multiplication                        |   |            |
|    35 | `divide`                       | `/`                                  | Division                              |   |            |
|    36 | `mod`                          | `%`                                  | Modulus                               |   |            |
|    37 | `**`                           | `pow()`                              | Exponentiation                        |   |            |
|    38 | `AND`                          | `&&`                                 | Logical AND                           |   |            |
|    39 | `OR`                           | `||`                                 | Logical OR                            |   |            |
|    40 | `NOT`                          | `!`                                  | Logical NOT                           |   |            |
|    41 | `XOR`                          | `^`                                  | Logical XOR                           |   |            |
|    42 | `<`                            | `<`                                  | Less than                             |   |            |
|    43 | `>`                            | `>`                                  | Greater than                          |   |            |
|    44 | `<=`                           | `<=`                                 | Less than or equal                    |   |            |
|    45 | `>=`                           | `>=`                                 | Greater than or equal                 |   |            |
|    46 | `==`                           | `==`                                 | Equality check                        |   |            |
|    47 | `!=`                           | `!=`                                 | Not equal                             |   |            |
|    48 | `root()`                       | `sqrt()`                             | Square root                           |   |            |
|    49 | `flr()`                        | `floor()`                            | Floor                                 |   |            |
|    50 | `ceil()`                       | `ceil()`                             | Ceiling                               |   |            |
|    51 | `abs()`                        | `abs()`                              | Absolute value                        |   |            |
|    52 | `logarithm()`                  | `log()`                              | Logarithm                             |   |            |
|    53 | `sine()`                       | `sin()`                              | Sine                                  |   |            |
|    54 | `cosine()`                     | `cos()`                              | Cosine                                |   |            |
|    55 | `tan()`                        | `tan()`                              | Tangent                               |   |            |
|    56 | `asine()`                      | `asin()`                             | Inverse sine                          |   |            |
|    57 | `acosine()`                    | `acos()`                             | Inverse cosine                        |   |            |
|    58 | `atan()`                       | `atan()`                             | Inverse tangent                       |   |            |
|    59 | `prime()`                      | user-defined                         | Prime number check                    |   |            |
|    60 | `wait <n> tick`                | delay                                | Time-based execution pause            |   |            |
|    61 | `module`                       | `class / struct`                     | Defines a class                       |   |            |
|    62 | `deploy`                       | object creation                      | Instantiate an object                 |   |            |
|    63 | `extends`                      | inheritance                          | Single inheritance                    |   |            |
|    64 | `public`                       | public                               | Public access specifier               |   |            |
|    65 | `private`                      | private                              | Private access specifier              |   |            |
|    66 | `this`                         | `this`                               | Reference to current object           |   |            |
|    67 | `fleet`                        | array                                | Collection of objects or values       |   |            |
|    68 | `mode`                         | `enum`                               | Enumerated type                       |   |            |
|    69 | `alias`                        | `typedef`                            | Type alias                            |   |            |
|    70 | `{`                            | `{`                                  | Block start                           |   |            |
|    71 | `}`                            | `}`                                  | Block end                             |   |            |

### Current parser notes (progress-2)

- Function definitions currently use `command name(...) : type { ... }`.
- Relational operators in scripts are symbols (`<`, `>`, `<=`, `>=`, `==`, `!=`), not token names like `LT`/`GT`.
- `.` is the statement terminator, so member access like `obj.field` is not currently supported in the parser.
- `orbit` currently supports condition-style looping: `orbit (condition) { ... }`.
