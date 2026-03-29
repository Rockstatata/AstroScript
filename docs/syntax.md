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

## Object-Oriented Programming

### Module (class) declaration

```
module Engine {
    public telemetry count hp := 100.

    command Engine(count start) : voidspace {
        this.hp := start.
    }

    command boost(count x) : count {
        this.hp := this.hp add x.
        back this.hp.
    }
}
```

### Inheritance and override

```
module Turbo extends Engine {
    public telemetry count bonus := 5.

    override command boost(count x) : count {
        back super.boost(x add this.bonus).
    }
}
```

### Object creation

```
deploy Turbo t.
deploy Turbo t2(25).

telemetry Turbo direct := new Turbo(30).
```

### Member access and calls

```
t.hp := 20.
transmit t.hp.
transmit t.boost(3).
```

Notes:
- Use `this.member` inside module methods.
- Use `super.method(...)` for base-module method dispatch.
- Member access uses dot syntax and the dot must be directly followed by the member name (for example `t.hp`, not `t. hp`).

## Overloading and Scoped Variables

### Function and method overloading (by parameter count)

```
command allocate(count slots) : count {
    back slots add 1.
}

command allocate(count slots, count reserve) : count {
    back slots add reserve.
}

module Queue {
    public telemetry count backlog := 0.

    command enqueue(count tasks) : count {
        this.backlog := this.backlog add tasks.
        back this.backlog.
    }

    command enqueue(count tasks, count priority) : count {
        this.backlog := this.backlog add tasks add priority.
        back this.backlog.
    }
}
```

### Scoped variable shadowing

```
telemetry count backlog := 10.

verify (1 == 1) {
    telemetry count backlog := 99.
    transmit backlog.
}

transmit backlog.
```

Notes:
- Overloads are selected by arity (the number of arguments).
- Variable redeclaration is allowed in inner blocks and shadows outer variables until the block ends.
