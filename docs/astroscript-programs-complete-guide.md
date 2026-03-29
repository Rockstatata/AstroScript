# AstroScript Comprehensive Programs Guide

This guide is a practical, example-heavy reference to learn AstroScript by running full programs.

Goal of this file:

1. Show mix-and-match syntax in real programs.
2. Demonstrate all language keywords from `docs/keywords.md`.
3. Provide corresponding expected outputs (or behavior notes).

Important runtime note:

- In current backend, `module`/`deploy`/inheritance/member access/override/super workflows execute at runtime.
- `abort` is still parse-level in this implementation (execution continues).
- Outputs below focus on what the current compiler runtime prints (`PRINT: ...`).

---

## Program 1: Mission Basics, Comments, and Terminators

Keywords covered:
`mission`, `launch`, `success`, `abort`, `transmit`, `$$`, `$*...*$`, `.`, `{`, `}`

```astroscript
$$ Single-line comment demo
$*
  Multi-line comment demo
*$

mission Basics launch {
    transmit "AstroScript boot sequence".
    abort.
    transmit "Abort is currently parsed; runtime continues in this implementation".
} success
```

Expected output:

```text
PRINT: AstroScript boot sequence
PRINT: Abort is currently parsed; runtime continues in this implementation
```

---

## Program 2: Primitive Types, Assignment, Arithmetic, Logic, Comparisons

Keywords covered:
`count`, `real`, `precise`, `flag`, `symbol`, `telemetry`, `limit`, `:=`, `add`, `minus`, `mul`, `divide`, `mod`, `**`, `AND`, `OR`, `NOT`, `XOR`, `<`, `>`, `<=`, `>=`, `==`, `!=`, `transmit`

```astroscript
mission TypesAndOps launch {
    telemetry count a := 10.
    telemetry count b := 3.
    telemetry real r := 2.5.
    telemetry precise p := 9.75.
    telemetry flag ready := 1.
    telemetry symbol msg := "status-ok".
    limit count immutable := 42.

    transmit a add b.
    transmit a minus b.
    transmit a mul b.
    transmit a divide b.
    transmit a mod b.
    transmit a ** b.

    transmit ready AND 1.
    transmit ready OR 0.
    transmit NOT 0.
    transmit 1 XOR 1.

    transmit a < b.
    transmit a > b.
    transmit a <= 10.
    transmit a >= 11.
    transmit a == 10.
    transmit a != 10.

    transmit msg.
    transmit immutable.
    transmit r add p.
} success
```

Expected output:

```text
PRINT: 13
PRINT: 7
PRINT: 30
PRINT: 3.333333
PRINT: 1
PRINT: 1000
PRINT: 1
PRINT: 1
PRINT: 1
PRINT: 0
PRINT: 0
PRINT: 1
PRINT: 1
PRINT: 0
PRINT: 1
PRINT: 0
PRINT: status-ok
PRINT: 42
PRINT: 12.25
```

---

## Program 3: Math Built-ins

Keywords covered:
`root`, `flr`, `ceil`, `abs`, `logarithm`, `sine`, `cosine`, `tan`, `asine`, `acosine`, `atan`, `prime`

```astroscript
mission MathBuiltins launch {
    transmit root(16).
    transmit flr(2.9).
    transmit ceil(2.1).
    transmit abs(-7).
    transmit logarithm(1).

    transmit sine(0).
    transmit cosine(0).
    transmit tan(0).
    transmit asine(0).
    transmit acosine(1).
    transmit atan(0).

    transmit prime(11).
    transmit prime(12).
} success
```

Expected output:

```text
PRINT: 4
PRINT: 2
PRINT: 3
PRINT: 7
PRINT: 0
PRINT: 0
PRINT: 1
PRINT: 0
PRINT: 0
PRINT: 0
PRINT: 0
PRINT: 1
PRINT: 0
```

---

## Program 4: Conditional Branching (`verify`, `else_verify`, `otherwise`)

Keywords covered:
`verify`, `else_verify`, `otherwise`

```astroscript
mission ConditionalFlow launch {
    telemetry count fuel := 35.

    verify (fuel > 80) {
        transmit "Full tank".
    }
    else_verify (fuel > 40) {
        transmit "Moderate tank".
    }
    otherwise {
        transmit "Low tank".
    }
} success
```

Expected output:

```text
PRINT: Low tank
```

---

## Program 5: Loops (`orbit`, `orbit while`, `orbit times`, `coast`, `stage_sep`)

Keywords covered:
`orbit`, `while`, `times`, `coast`, `stage_sep`

```astroscript
mission LoopShowcase launch {
    telemetry count i := 0.

    orbit (i < 3) {
        transmit i.
        i := i add 1.
    }

    telemetry count j := 0.
    orbit while (j < 3) {
        transmit j add 100.
        j := j add 1.
    }

    telemetry count k.
    orbit times (k : 0 : 6) {
        verify (k == 2) {
            coast.
        }

        verify (k == 5) {
            stage_sep.
        }

        transmit k add 200.
    }
} success
```

Expected output:

```text
PRINT: 0
PRINT: 1
PRINT: 2
PRINT: 100
PRINT: 101
PRINT: 102
PRINT: 200
PRINT: 201
PRINT: 203
PRINT: 204
```

Explanation:

- `coast` skips the `k == 2` iteration body.
- `stage_sep` exits loop when `k == 5`.

---

## Program 6: Scenario Switching (`scenario`, `trajectory`, `fallback`)

Keywords covered:
`scenario`, `trajectory`, `fallback`

```astroscript
mission ScenarioDemo launch {
    telemetry count state := 2.

    scenario (state) {
        trajectory 1:
            transmit "STATE-ONE".
            stage_sep.

        trajectory 2:
            transmit "STATE-TWO".
            stage_sep.

        fallback:
            transmit "STATE-UNKNOWN".
    }
} success
```

Expected output:

```text
PRINT: STATE-TWO
```

---

## Program 7: Arrays, Fleet, and Indexed Operations

Keywords covered:
`fleet`, `telemetry`, indexed access `[]`

```astroscript
mission ArraysAndFleet launch {
    fleet count standbyFleet.

    telemetry count nums[5].
    nums[0] := 7.
    nums[1] := nums[0] add 5.
    nums[2] := nums[1] mul 2.

    transmit nums[0].
    transmit nums[1].
    transmit nums[2].
} success
```

Expected output:

```text
PRINT: 7
PRINT: 12
PRINT: 24
```

Note:

- `fleet` is parsed and represented as array-like type metadata.

---

## Program 8: Functions (`command`, `back`, `voidspace`, `:`, `:->`)

Keywords covered:
`command`, `back`, `voidspace`, `:` return style, `:->` return style

```astroscript
mission FunctionDemo launch {
    command sum(count x, count y) : count {
        back x add y.
    }

    command square(count n) :-> count {
        back n mul n.
    }

    command ping() : voidspace {
        transmit "PING".
        back.
    }

    transmit sum(5, 6).
    transmit square(9).
    ping().
} success
```

Expected output:

```text
PRINT: 11
PRINT: 81
PRINT: PING
```

---

## Program 9: OOP End-to-End (`module`, `extends`, `this`, `super`, `override`, `deploy`, `new`)

Keywords covered:
`module`, `extends`, `public`, `this`, `super`, `override`, `deploy`, `new`

```astroscript
mission OOPCheck launch {
    module Engine {
        public telemetry count hp := 10.

        command Engine(count start) : voidspace {
            this.hp := start.
        }

        command inc(count x) : count {
            this.hp := this.hp add x.
            back this.hp.
        }
    }

    module Turbo extends Engine {
        public telemetry count bonus := 5.

        command Turbo(count start) : voidspace {
            this.hp := start.
        }

        override command inc(count x) : count {
            back super.inc(x add this.bonus).
        }
    }

    deploy Turbo t(20).
    telemetry count result := t.inc(2).
    transmit result.
    transmit t.hp.
} success
```

Expected output:

```text
PRINT: 27
PRINT: 27
```

Notes:

- Constructors are regular module commands with the same name as the module.
- `override` validates that a compatible method exists in the base module chain.
- `super.method(...)` dispatches directly to base-module implementation.

---

## Program 9.1: Overloading and Scoped Variables

Keywords covered:
`command` overloading, module method overloading, lexical variable shadowing

```astroscript
mission OverloadAndScope launch {
    command allocate(count slots) : count {
        back slots add 1.
    }

    command allocate(count slots, count reserve) : count {
        back slots add reserve.
    }

    module Queue {
        public telemetry count backlog := 0.

        command Queue(count seed) : voidspace {
            this.backlog := seed.
        }

        command enqueue(count tasks) : count {
            this.backlog := this.backlog add tasks.
            back this.backlog.
        }

        command enqueue(count tasks, count priority) : count {
            this.backlog := this.backlog add tasks add priority.
            back this.backlog.
        }
    }

    deploy Queue q(5).

    transmit allocate(3).
    transmit allocate(3, 2).
    transmit q.enqueue(2).
    transmit q.enqueue(2, 1).

    telemetry count backlog := 42.
    verify (1 == 1) {
        telemetry count backlog := 99.
        transmit backlog.
    }

    transmit backlog.
} success
```

Expected output:

```text
PRINT: 4
PRINT: 5
PRINT: 7
PRINT: 10
PRINT: 99
PRINT: 42
```

Notes:

- Overloads are selected strictly by parameter count.
- Inner block declarations shadow outer variables and are released at block exit.

---

## Program 10: Type Alias and Mode Syntax (`alias`, `mode`)

Keywords covered:
`alias`, `mode`, plus `trajectory`/`fallback` mode entries

```astroscript
mission AliasAndMode launch {
    alias precise decimal.

    telemetry decimal ratio := 3.14159.
    transmit ratio.

    mode EngineMode {
        trajectory 1.
        trajectory 2.
        fallback 0.
    }

    transmit "Mode declared".
} success
```

Expected output:

```text
PRINT: 3.14159
PRINT: Mode declared
```

---

## Program 11: Input and Delay (`receive`, `broadcast`, `alarm`, `wait`, `tick`)

Keywords covered:
`receive`, `broadcast`, `alarm`, `wait`, `tick`

```astroscript
mission IOAndWait launch {
    telemetry count userValue.

    broadcast "Enter a number:".
    receive userValue.

    wait 2 tick.

    alarm "Echo:".
    transmit userValue.
} success
```

Expected output (interactive run, user enters `17`):

```text
PRINT: Enter a number:
PRINT: Echo:
PRINT: 17
```

Expected output (non-interactive/no stdin):

```text
PRINT: Enter a number:
PRINT: Echo:
PRINT: 0
```

---

## Program 12: Full Mix-and-Match Mission (Mega Example)

This final program intentionally combines many features in one place.

```astroscript
mission MegaMission launch {
    $$ Setup
    telemetry count i := 0.
    telemetry real avg := 12.5.
    telemetry precise factor := 2.0.
    telemetry symbol tag := "MEGA".
    limit count maxI := 6.

    alias count idx.
    telemetry idx loopIndex := 0.

    telemetry count arr[4].
    arr[0] := 10.
    arr[1] := arr[0] add 2.

    command calc(count x, count y) : count {
        back (x mul y) add 1.
    }

    command mood() : symbol {
        back "stable".
    }

    transmit tag.
    transmit root(25).
    transmit prime(19).

    orbit times (loopIndex : 0 : maxI) {
        verify (loopIndex == 2) {
            coast.
        }
        else_verify (loopIndex > 4) {
            stage_sep.
        }
        otherwise {
            transmit loopIndex add 500.
        }
    }

    scenario (arr[1]) {
        trajectory 12:
            transmit "Array branch OK".
            stage_sep.
        fallback:
            transmit "Array branch fallback".
    }

    transmit calc(3, 4).
    transmit mood().
    transmit avg divide factor.

    module Probe {
        public telemetry count hp := 99.
        private limit count id := 1.
        command who() : symbol {
            back this.
        }
    }

    deploy Probe p.
    transmit who().

    wait 1 tick.
    broadcast "Mega mission done".
    alarm "Shutdown".

    abort.
} success
```

Expected output:

```text
PRINT: MEGA
PRINT: 5
PRINT: 1
PRINT: 500
PRINT: 501
PRINT: 503
PRINT: 504
PRINT: Array branch OK
PRINT: 13
PRINT: stable
PRINT: 6.25
PRINT: this
PRINT: Mega mission done
PRINT: Shutdown
```

---

## Keyword Coverage Matrix

The following matrix points to where each keyword is demonstrated.

| Keyword | Shown in Program(s) |
| --- | --- |
| mission, launch, success | All programs |
| abort | 1, 12 |
| count, real, precise, flag, symbol, voidspace | 2, 8, 12 |
| telemetry, limit, :=, . | Most programs |
| transmit, broadcast, receive, alarm | 1, 2, 11, 12 |
| $$, $*...*$ | 1 |
| command, back | 8, 9, 12 |
| verify, else_verify, otherwise | 4, 5, 12 |
| orbit, while, times, stage_sep, coast | 5, 6, 12 |
| scenario, trajectory, fallback | 6, 10, 12 |
| add, minus, mul, divide, mod, ** | 2, 12 |
| AND, OR, NOT, XOR | 2 |
| <, >, <=, >=, ==, != | 2, 4, 5, 12 |
| root, flr, ceil, abs, logarithm, sine, cosine, tan, asine, acosine, atan, prime | 3 |
| wait, tick | 11, 12 |
| module, deploy, extends, public, private, this | 9, 12 |
| fleet | 7 |
| mode | 10 |
| alias | 10, 12 |
| {, } | All programs |

---

## Suggested Learning Order

1. Program 1-3: syntax + operators + math built-ins.
2. Program 4-6: branching and control flow.
3. Program 7-8: arrays and functions.
4. Program 9-10: module/type surface syntax.
5. Program 11: I/O and runtime behavior.
6. Program 12: complete mixed mission.

---

## Quick Practice Tasks

1. In Program 5, remove `coast` and compare output.
2. In Program 6, set `state := 9` and observe fallback branch.
3. In Program 8, intentionally pass wrong argument count to see semantic diagnostics.
4. In Program 12, change `arr[1]` assignment so scenario picks fallback.

This is the fastest way to internalize the language and compiler behavior end-to-end.
