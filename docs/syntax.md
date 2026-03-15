# Language Syntax

## Program Structure

Every AstroScript program must be wrapped in a mission block:

```
mission ProgramName launch {
    $$ Your code here
} success
```

## Statements

All statements end with a period (`.`), which acts as the statement terminator.

## Variable Declaration

Use `telemetry` to declare variables:

```
telemetry count x.
telemetry real pi.
telemetry symbol name.
telemetry flag isReady.
```

Declare with initialization:

```
telemetry count x := 10.
```

Use `limit` for constants:

```
limit count MAX := 100.
```

## Assignment

Use `:=` for assignment:

```
x := 42.
y := x add 10.
```

## Arithmetic

AstroScript supports both keyword and symbol operators:

| Operation      | Keyword  | Symbol |
|---------------|----------|--------|
| Addition      | `add`    | `+`    |
| Subtraction   | `minus`  | `-`    |
| Multiplication| `mul`    | `*`    |
| Division      | `divide` | `/`    |
| Modulo        | `mod`    |        |
| Power         |          | `**`   |

## Comparison Operators

`<`, `>`, `<=`, `>=`, `==`, `!=`

## Logical Operators

`AND`, `OR`, `NOT`, `XOR`

## Conditionals

```
verify (x > 0) {
    transmit x.
} else_verify (x == 0) {
    transmit 0.
} otherwise {
    transmit -1.
}
```

## Loops

While loop:

```
orbit (counter < 10) {
    transmit counter.
    counter := counter add 1.
}
```

For-style loop:

```
orbit times (i : 0 : 10) {
    transmit i.
}
```

## Functions

```
command add(count a, count b) : count {
    back a add b.
}
```

## Input / Output

```
transmit expression.    $$ print
receive variable.       $$ read input
```

## Comments

```
$$ This is a single-line comment

$* This is a
   multi-line comment *$
```

## Arrays

```
telemetry count arr[5].
arr[0] := 10.
transmit arr[0].
```
