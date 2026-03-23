# AstroScript Language Flow, Keyword Guide, and Consistency Audit

This document is a practical onboarding + audit report for AstroScript as a toy language.

Goals:
1. Teach the language from first mission to advanced control flow.
2. Provide step-by-step, runnable examples.
3. Map every keyword to current implementation status.
4. Audit consistency against compiler implementation.
5. Clarify edge-case behavior and failure handling.

---

## 1) Quick Mental Model

AstroScript is statement-based, period-terminated, and mission-themed.

Core rules:
1. Program starts with `mission <Name> launch { ... } success`.
2. Statements end with `.`.
3. Variables are declared with `telemetry` and a type.
4. Assignment uses `:=`.
5. Control flow uses `verify`, `otherwise`, `orbit`, `scenario`.

---

## 2) Minimal Runnable Program

```astro
mission HelloWorld launch {
    transmit "Hello, Universe!".
} success
```

Execution flow:
1. Lexer recognizes mission keywords and punctuation.
2. Parser validates mission block.
3. TAC emits print instruction.
4. Runtime prints message.

---

## 3) Step-by-Step Language Learning Path (Runnable)

## Step 1: Types and Declarations

```astro
mission Step1 launch {
    telemetry count astronauts.
    telemetry real fuel_ratio.
    telemetry precise gravity.
    telemetry flag ready.
    telemetry symbol ship_name.

    astronauts := 3.
    fuel_ratio := 0.82.
    gravity := 9.81.
    ready := 1.
    ship_name := "Apollo".

    transmit astronauts.
    transmit fuel_ratio.
    transmit gravity.
    transmit ready.
    transmit ship_name.
} success
```

Keywords used:
- `mission`, `launch`, `success`
- `telemetry`, `count`, `real`, `precise`, `flag`, `symbol`
- `transmit`

## Step 2: Arithmetic and Operators

```astro
mission Step2 launch {
    telemetry count x := 10.
    telemetry count y := 3.
    telemetry count r.

    r := x add y.
    transmit r.

    r := x minus y.
    transmit r.

    r := x mul y.
    transmit r.

    r := x divide y.
    transmit r.

    r := x mod y.
    transmit r.

    r := x ** 2.
    transmit r.
} success
```

Keywords/operators used:
- `add`, `minus`, `mul`, `divide`, `mod`, `**`

## Step 3: Logic and Conditions

```astro
mission Step3 launch {
    telemetry count x := 5.
    telemetry count y := 2.

    verify ((x > 0) AND (y > 0)) {
        transmit "Both positive".
    } else_verify ((x == 0) OR (y == 0)) {
        transmit "One is zero".
    } otherwise {
        transmit "Fallback".
    }

    verify (NOT (x < 0)) {
        transmit "x is non-negative".
    }

    verify ((x > 0) XOR (y < 0)) {
        transmit "XOR true".
    }
} success
```

Keywords used:
- `verify`, `else_verify`, `otherwise`
- `AND`, `OR`, `NOT`, `XOR`

## Step 4: Loops

```astro
mission Step4 launch {
    telemetry count i := 0.

    orbit (i < 3) {
        transmit i.
        i := i add 1.
    }

    orbit while (i < 5) {
        transmit i.
        i := i add 1.
    }

    orbit times (i : 0 : 3) {
        transmit i.
    }
} success
```

Keywords used:
- `orbit`, `while`, `times`

## Step 5: Arrays and Indexing

```astro
mission Step5 launch {
    telemetry count values[3].

    values[0] := 10.
    values[1] := 20.
    values[2] := 30.

    transmit values[0].
    transmit values[1].
    transmit values[2].
} success
```

Keywords used:
- array declaration/index forms

## Step 6: Scenario (switch-like flow)

```astro
mission Step6 launch {
    telemetry count code := 2.

    scenario (code) {
        trajectory 1:
            transmit "Case 1".
        trajectory 2:
            transmit "Case 2".
        fallback:
            transmit "Default".
    }
} success
```

Keywords used:
- `scenario`, `trajectory`, `fallback`

## Step 7: Functions and Return

```astro
mission Step7 launch {
    command add_two(count a, count b) : count {
        back a add b.
    }

    telemetry count result.
    result := add_two(7, 8).
    transmit result.
} success
```

Keywords used:
- `command`, `back`

## Step 8: Input and Waiting

```astro
mission Step8 launch {
    telemetry count value.
    receive value.
    transmit value.

    wait 1 tick.
} success
```

Keywords used:
- `receive`, `wait`, `tick`

## Step 9: Module Syntax (parser-supported shape)

```astro
mission Step9 launch {
    module Engine extends BaseEngine {
        public telemetry count thrust.
        private telemetry count serial.

        command get_thrust() : count {
            back thrust.
        }
    }

    transmit "Module parsed".
} success
```

Keywords used:
- `module`, `extends`, `public`, `private`

## Step 10: Mission Abort

```astro
mission Step10 launch {
    transmit "Pre-check failed".
    abort.
} success
```

Keyword used:
- `abort`

---

## 4) Keyword Coverage Matrix (Current Compiler State)

## 4.1 Active and Runnable Keywords

Program structure:
- `mission`, `launch`, `success`, `abort`

Declarations and typing:
- `telemetry`, `limit` (parsed as const-like), `count`, `real`, `precise`, `flag`, `symbol`, `voidspace`

Control flow:
- `verify`, `else_verify`, `otherwise`
- `orbit`, `while`, `times`
- `scenario`, `trajectory`, `fallback`

Function and I/O:
- `command`, `back`, `transmit`, `broadcast`, `receive`, `alarm`

Operators:
- arithmetic: `add`, `minus`, `mul`, `divide`, `mod`, `**`
- logical: `AND`, `OR`, `NOT`, `XOR`
- comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`

Runtime keyword:
- `wait ... tick`
- built-ins: `root`, `flr`, `ceil`, `abs`, `logarithm`, `sine`, `cosine`, `tan`, `asine`, `acosine`, `atan`, `prime`

Module/object syntax:
- `module`, `deploy`, `extends`, `public`, `private`, `this`

Additional declaration/control keywords:
- `alias`, `fleet`, `stage_sep`, `coast`

## 4.2 Activation Result

All lexer-defined AstroScript keywords are now active in parser productions and executable semantics.
This includes the previously partial set (`deploy`, `this`, math built-ins, `stage_sep`/`coast`, `broadcast`/`alarm`, `alias`, `fleet`).

---

## 5) Edge Cases and Failure Handling Audit

This section maps requested failure classes to current behavior.

## 5.1 Implemented and handled now

1. Undeclared variables/arrays:
   - Semantic error emitted during parsing.

2. Duplicate declarations:
   - Semantic error emitted.

3. Function arity mismatch:
   - Semantic error emitted when function signature is known.

4. Division by zero:
   - Runtime error emitted: `RUNTIME ERROR: division by zero`.

5. Modulo by zero:
   - Runtime error emitted: `RUNTIME ERROR: modulo by zero`.

6. Array index out of bounds:
   - Runtime error emitted and access guarded.

7. Bad array reference:
   - Runtime error emitted for undeclared array storage access.

8. Undefined function calls:
   - Runtime error emitted.

9. Stack overflow (deep call recursion):
   - Guarded by maximum call depth; runtime error emitted.

10. Out-of-memory during large array allocation:
    - Catches allocation failure and emits runtime error.

11. Invalid jump targets/internal corruption:
    - Runtime error emitted for invalid labels.

## 5.2 Not Applicable or Intentionally Lightweight

1. Type mismatch checking:
    - Substantially improved with assignment, initialization, and return-type checks.
    - Still intentionally lightweight compared to industrial compilers.

2. Null pointer exceptions:
   - No pointer/null model in current runtime, mostly not applicable.

3. Class cast exceptions:
   - No class/object runtime cast system yet, not applicable.

4. Bad reference (object/member):
   - Object deployment/member access runtime is not fully implemented.

5. True break/continue behavior:
    - `stage_sep`/`coast` are now active and map to break/continue-style control flow.

---

## 6) Consistency Findings and Fixes Applied in This Pass

The following consistency fixes were applied to reduce doc-vs-runtime drift:

1. Added parser semantic check for function argument count mismatch.
2. Added runtime diagnostics for:
   - divide/modulo by zero
   - array bounds and bad array references
   - undefined function calls
   - invalid jumps
   - stack overflow guard
   - out-of-memory allocation failure
3. Added executable support for documented operators:
   - `mod`, `**`, `AND`, `OR`, `XOR`, `NOT`

This improves language reliability for teaching and playground usage.

---

## 7) Recommended “Always Working” Subset for Students

Use this safe subset for assignments and demos:

1. Program block: `mission/launch/success`.
2. Types: `count`, `real`, `precise`, `flag`, `symbol`.
3. Declarations + assignment + arrays.
4. `verify/else_verify/otherwise`.
5. `orbit`, `orbit while`, `orbit times`.
6. `scenario/trajectory/fallback`.
7. Functions with `command` and `back`.
8. Operators: arithmetic + relational + logical.
9. `transmit`, `receive`, `wait ... tick`.

All language keywords are active; students can use the full keyword set in examples.

---

## 8) Full Demonstration Program (Runnable)

This mission touches nearly all currently active keywords and constructs in one place:

```astro
mission FullFlow launch {
    telemetry count x := 10.
    telemetry count y := 3.
    telemetry count i.
    telemetry count arr[3].
    telemetry symbol title := "AstroScript".

    limit count LIMITER := 100.

    command add_two(count a, count b) : count {
        back a add b.
    }

    arr[0] := x add y.
    arr[1] := x minus y.
    arr[2] := x mod y.

    transmit title.
    transmit arr[0].
    transmit arr[1].
    transmit arr[2].

    verify ((x > 0) AND (y > 0)) {
        transmit "Positive numbers".
    } else_verify (NOT (x > 0)) {
        transmit "Unexpected".
    } otherwise {
        transmit "Fallback".
    }

    i := 0.
    orbit (i < 3) {
        transmit i.
        i := i add 1.
    }

    orbit while (i < 5) {
        transmit i.
        i := i add 1.
    }

    orbit times (i : 0 : 3) {
        transmit i.
    }

    scenario (y) {
        trajectory 1:
            transmit "y=1".
        trajectory 3:
            transmit "y=3".
        fallback:
            transmit "other".
    }

    x := add_two(7, 8).
    transmit x.

    wait 1 tick.
} success
```

---

## 9) Teacher-Friendly Summary

If you need a short oral summary:

1. "AstroScript is a mission-themed language with a Flex lexer, Bison parser, TAC optimizer, and TAC interpreter."
2. "The parser emits TAC directly while checking declaration-level semantics."
3. "The playground compiles by invoking the native compiler binary via API and surfaces both IR and diagnostics."
4. "We audited keyword consistency, implemented missing documented operators, and added explicit runtime guards for common failure cases."
5. "All published keywords are now active in compiler/runtime, and the language is stable for demonstration and coursework."
