# AstroScript Feature Compilation Execution Map

This document is the exact execution map you asked for: when AstroScript code contains conditionals, loops, functions, and classes/modules, which concrete lexer, parser, semantic, TAC, C-translation, and runtime lines execute.

Verified against the current active backend on March 30, 2026.

## 1) Global Pipeline (What Runs First)

When you run the compiler binary, execution starts in:

- main entry: [backend/compiler/main.cpp](../backend/compiler/main.cpp#L13)
- parse trigger: [backend/compiler/main.cpp](../backend/compiler/main.cpp#L26)
- TAC optimize: [backend/compiler/main.cpp](../backend/compiler/main.cpp#L28)
- TAC print: [backend/compiler/main.cpp](../backend/compiler/main.cpp#L29)
- C-like translation print: [backend/compiler/main.cpp](../backend/compiler/main.cpp#L30)
- TAC runtime execute: [backend/compiler/main.cpp](../backend/compiler/main.cpp#L31)
- symbol table print: [backend/compiler/main.cpp](../backend/compiler/main.cpp#L32)

So the actual order is:

1. Lexer tokenizes.
2. Parser validates grammar and emits TAC in semantic actions.
3. TAC optimizer runs.
4. C-like projection is generated from optimized TAC.
5. TAC interpreter executes.
6. Symbol metadata table is printed.

## 2) Conditionals (verify / else_verify / otherwise)

### Lexer lines (token recognition)

- verify token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L88)
- else_verify token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L89)
- otherwise token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L90)

### Parser lines (grammar + TAC emission)

- conditional grammar starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L906)
- first branch jump emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L910)
- end-of-then jump emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L917)
- else label emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L918)
- final end label emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L924)
- else_verify chain grammar: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L928)
- else_verify condition jump emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L933)

### TAC API points used

- emit if-false jump: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L135)
- emit goto: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L131)
- emit label: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L127)

### C translation and runtime execution lines

- structured if reconstruction from TAC ifFalse: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1332)
- translated if rendering: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1347)
- translated else rendering: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1368)
- runtime goto dispatch: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1631)
- runtime ifFalse branch dispatch: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1641)

### What actually happens for one verify block

1. Lexer emits VERIFY/ELSE_VERIFY/OTHERWISE tokens.
2. Parser creates labels and emits TAC pattern: condition temp, ifFalse -> elseLabel, then body, goto endLabel, else body, endLabel.
3. C translator pattern-matches that TAC shape and prints structured if/else when possible.
4. Runtime interpreter still executes the original TAC with program counter jumps.

## 3) Loops (orbit, orbit while, orbit times)

### Lexer lines (token recognition)

- orbit token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L92)
- while token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L93)
- times token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L94)

### Parser lines (grammar + loop lowering)

- loop grammar starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L952)
- orbit(...) start label emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L958)
- orbit(...) condition jump emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L963)
- orbit while branch starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L973)
- orbit while label emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L978)
- orbit times branch starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L993)
- orbit times condition jump emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1000)
- orbit times continue label/increment emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1010)

### Break/continue style keywords

- stage_sep handling (break-like): [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L531)
- coast handling (continue-like): [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L541)

### C translation and runtime execution lines

- while reconstruction from TAC label/back-edge pattern: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1309)
- runtime goto: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1631)
- runtime ifFalse exit: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1641)

### What actually happens for orbit loops

1. Parser lowers each loop into labels + ifFalse + goto TAC.
2. Translator rebuilds while-style syntax when it sees the canonical back-edge label pattern.
3. Interpreter executes raw TAC jumps; this is the source of truth regardless of translation formatting.

## 4) Functions (command, parameters, calls, returns)

### Lexer lines

- command token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L81)

### Parser lines (definition + call + arguments)

- function grammar starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1224)
- function begin TAC emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1329)
- implicit function return emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1343)
- function end emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1344)
- argument list grammar starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1395)
- argument push to TAC param stack: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1398)
- additional argument push: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1404)
- function call expression grammar: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1937)
- function call TAC emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1944)

### TAC API lines

- emitParam: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L151)
- emitCall: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L155)
- emitMethodCall (method path): [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L161)

### Runtime execution lines

- function call dispatch: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1856)
- function begin handling: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1844)
- return handling: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L2010)
- function end unwinding: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L2029)

### C translation lines

- call translation branch: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1176)
- call+assign fusion handling: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1412)

### What actually happens for a call

1. Parser emits one param TAC op per argument.
2. Parser emits call TAC with function selector and arity.
3. Runtime pops arg values, pushes call frame, binds param_def names, jumps into function TAC range.
4. Return stores value into call temp and resumes caller.

## 5) Classes/Modules (module, extends, fields, methods, object calls)

### Lexer lines

- module token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L65)
- extends token: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L67)
- this/new/super tokens: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L70), [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L71), [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L72)
- member dot tokenization: [backend/compiler/lexer/lexer.l](../backend/compiler/lexer/lexer.l#L156)

### Parser lines (module semantics)

- module grammar starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1077)
- inheritance grammar starts: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1107)
- cyclic inheritance guard helper: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L225)
- register class in TAC metadata: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1099)
- register field defaults in TAC metadata: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1184), [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1200), [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1217)
- register method selector mapping: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1303)
- object construction expression branch: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1684)
- constructor method call emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1699)
- object method call grammar: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1744)
- object method call emission: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1768)
- field access grammar (obj.field): [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1829)

### Semantic helper lines used by module/member resolution

- resolve field hierarchy: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L245)
- resolve method hierarchy: [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L266)
- access control check (public/private): [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L310)

### TAC metadata/API lines

- registerClass: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L189)
- registerField: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L194)
- registerMethod: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L199)

### Runtime object/method execution lines

- object allocation op: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1701)
- field write op: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1776)
- field read op: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1803)
- method call op: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1905)

### C translation lines for object semantics

- method-call translation with args kept visible: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1202)
- method-call assignment fusion path: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1433)
- object runtime shim declaration (`astro_call_methodv`): [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1505)

## 6) Symbol Table: Where It Is and What It Really Does

### Files and APIs

- symbol struct definition: [backend/compiler/semantic/symbol_table.h](../backend/compiler/semantic/symbol_table.h#L7)
- insert API: [backend/compiler/semantic/symbol_table.h](../backend/compiler/semantic/symbol_table.h#L19)
- exists API: [backend/compiler/semantic/symbol_table.h](../backend/compiler/semantic/symbol_table.h#L20)
- getType API: [backend/compiler/semantic/symbol_table.h](../backend/compiler/semantic/symbol_table.h#L23)
- printTable API: [backend/compiler/semantic/symbol_table.h](../backend/compiler/semantic/symbol_table.h#L25)
- insert impl: [backend/compiler/semantic/symbol_table.cpp](../backend/compiler/semantic/symbol_table.cpp#L5)
- exists impl: [backend/compiler/semantic/symbol_table.cpp](../backend/compiler/semantic/symbol_table.cpp#L14)
- getType impl: [backend/compiler/semantic/symbol_table.cpp](../backend/compiler/semantic/symbol_table.cpp#L33)
- print impl: [backend/compiler/semantic/symbol_table.cpp](../backend/compiler/semantic/symbol_table.cpp#L49)

Important: scope stacks and most semantic resolution logic are in parser helpers, not in `symbol_table.cpp`.

## 7) What Was Fixed in C Translation (Why output looked weird before)

These fixes are in active `backend/compiler/ir/tac.cpp`:

1. side-effect calls/method calls are no longer silently dropped when temp results are unused.
   - temp usage accounting: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L828)
   - call branch fix: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1176)
   - method call branch fix: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1202)
2. translated method calls now show argument values directly in emitted C-like output.
   - vararg method shim emission: [backend/compiler/ir/tac.cpp](../backend/compiler/ir/tac.cpp#L1505)
3. constructor calls after object creation preserve constructor arguments in translated output.
   - argument stack usage preserved through object-new then mcall path in the same translation pass.

## 8) How To Trace Your Own Program Quickly

For any feature in your source file:

1. find its lexer token line in `lexer.l`.
2. find its grammar rule and semantic action line in `parser.y`.
3. find the corresponding TAC emit API in `tac.cpp`/`tac.h`.
4. find translation/rendering line in `printCTranslation`.
5. find runtime dispatch line in `execute`.

This is the same tracing pattern used for all feature maps above.

## 9) Parser.y Explained Like a Human (No Compiler Jargon First)

If `parser.y` feels impossible to read, use this mental model:

1. Left side of a rule is a language feature name.
2. Right side of a rule is the token pattern that matches source code.
3. `{ ... }` blocks are "what to do when that pattern matches".
4. That action block can do three jobs at the same time:
   - semantic checks (is this valid?)
   - symbol/scope updates (what names exist now?)
   - TAC emission (what runtime instructions should execute later?)

In this project, the parser is not only checking grammar. It is the central coordinator of semantic rules + IR generation.

### 9.1 How to read Bison value variables (`$$`, `$1`, `$2`, ...)

- `$1`, `$2`, `$3` are values of matched items on the right side.
- `$$` is the value produced by the whole left-side rule.

For expression rules, these are usually `ExprAttr*` objects that carry:

- `place`: where the value lives in TAC (`t1`, variable name, constant text)
- `typeName`: static type view (`COUNT`, `REAL`, `MODULE:Ship`, ...)
- `isConst` and `value`: constant tracking support

So in a rule like `expression ADD expression`, the parser reads both operands from `$1` and `$3`, emits TAC with `emitBinary`, then writes the result expression into `$$`.

### 9.2 What the top of parser.y is really doing

The top helper section defines parser runtime state:

1. Context stacks
   - `ifStack`, `loopStack`, `switchStack`
   - used to carry labels and branch targets while nested control flow is parsed
2. Function/module state
   - current function name, params, return type, whether currently inside module
3. Scope state
   - `variableScopes`, `constantScopes`, and declaration line maps
4. Semantic helper utilities
   - type compatibility, inheritance checks, member/method resolution

This is why the parser can enforce language rules immediately while reducing grammar.

## 10) How Parser Syncs With Symbol Table, Scope, and TAC

Think of each parsed statement as passing through 4 synchronized phases inside one action block.

### 10.1 Phase A: check declaration/scope validity

Example in declarations and assignments:

1. `declareScopedName(...)` is called to prevent redeclaration in same scope.
2. `isDeclaredName(...)` checks if variable/object exists before use.
3. `isConstantName(...)` prevents writes to constants.

This logic is parser-owned, scope-aware, and immediate.

### 10.2 Phase B: type and semantic rules

Parser checks compatibility before emitting runtime actions:

1. `areTypesCompatible(...)` for assignment/initialization/return.
2. module/member access checks:
   - `resolveFieldInHierarchy(...)`
   - `resolveMethodInHierarchy(...)`
   - `canAccessMember(...)`
3. function/method arity checks and constructor checks.

So semantic errors are raised while building TAC, not after.

### 10.3 Phase C: symbol metadata updates

When a declaration is accepted:

1. parser scope maps are updated (`variableScopes`, `constantScopes`)
2. global `SymbolTable` gets inserted when appropriate

Important design detail:

- parser owns rich scope logic
- `symbol_table.cpp` acts as a metadata registry and report source

### 10.4 Phase D: TAC emission for runtime behavior

After semantic acceptance, parser emits IR using `tacGenerator`:

1. declarations: `emitDeclare`, `emitArrayDecl`
2. assignments: `emitAssign`, `emitArrayStore`, `emitFieldStore`
3. expressions: `emitBinary`, `emitUnary`
4. control flow: `emitLabel`, `emitIfFalseGoto`, `emitGoto`
5. calls: `emitParam`, `emitCall`, `emitMethodCall`

This TAC is what the optimizer and interpreter consume later.

## 11) A Full Statement Walkthrough (What Happens Internally)

Source statement:

`telemetry count x := 5 add 3.`

What parser action does in order:

1. grammar rule matches declaration-with-initializer pattern
2. scope/semantic checks:
   - ensure `x` is not redeclared in current scope
   - ensure type compatibility (`count` with expression result)
3. symbol update:
   - register scoped name `x`
   - update symbol table metadata
4. TAC emission:
   - `decl COUNT x`
   - `t1 = 5 + 3`
   - `x = t1`
5. later pipeline:
   - optimizer may fold `t1 = 8`
   - interpreter executes final TAC

Same pattern applies to loops, conditionals, function calls, and module operations.

## 12) Why This Parser Feels Hard To Read

It combines three compiler layers in one file:

1. grammar definitions
2. semantic/type/access checks
3. IR lowering

So each action block is doing more than one conceptual task.

A practical reading strategy is:

1. ignore helper declarations on first pass
2. read only rule names in order (`declaration`, `assignment`, `expression`, ...)
3. inside each action, split lines mentally into:
   - check lines
   - symbol/scope lines
   - TAC emission lines

Once you separate those three roles, parser.y becomes much easier to follow.

## 13) Short Answer You Can Tell Your Teacher

"Our parser is doing synchronized compile-time work: it validates grammar, performs semantic checks with scope and type logic, updates symbol metadata, and emits TAC IR in the same pass. The symbol table stores declaration metadata, while parser-owned scope stacks handle rich visibility rules. TAC then becomes the single executable form for optimization and runtime interpretation."

## 14) True Line-by-Line Bison Walkthrough (How It Really Executes)

This section is the direct "explain the code lines" version.

How to read this section:

1. Each step references real parser line numbers.
2. The order shown is execution order of semantic actions while parsing that construct.
3. For each line, explanation focuses on what changes in parser state, semantic state, symbol state, and TAC.

### 14.1 Conditionals (`verify / else_verify / otherwise`) line-by-line

Relevant lines:

- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L906)

Execution:

1. `906`: enters `control_statement` rule for `VERIFY (...)`.
2. `907`: parser confirms opening shape `VERIFY LPAREN expression RPAREN`.
3. `909`: creates `IfContext` with two fresh TAC labels (else and end).
4. `910`: emits TAC branch `ifFalse condition goto elseLabel` using expression place from `$3`.
5. `911`: pushes this context onto `ifStack` so nested blocks can find active labels.
6. `912`: frees expression attribute object now that TAC operand was consumed.
7. `914`: parses the then-block.
8. `916`: pulls active `IfContext` from top of stack.
9. `917`: emits unconditional `goto endLabel` after then-body.
10. `918`: emits `elseLabel` so execution can enter else chain when condition is false.
11. `920`: parses chained `else_verify` and optional `otherwise`.
12. `922`: copies top `IfContext` for finalization.
13. `923`: pops conditional context from stack (conditional is closed).
14. `924`: emits `endLabel` join point.

`else_verify_chain` details:

1. `929`: matches one `ELSE_VERIFY (expression)` clause.
2. `931`: gets current shared `IfContext`.
3. `932`: allocates `nextLabel` for failing this else-if condition.
4. `933`: emits `ifFalse expr goto nextLabel`.
5. `934`: updates context's `elseLabel` to this next fallback label.
6. `935`: frees expression attr.
7. `937`: parses this else-if block.
8. `939`: reuses active context.
9. `940`: emits `goto endLabel` after successful else-if block.
10. `941`: emits current else label.
11. `943`: recursion allows more `else_verify` clauses.
12. `944`: empty production terminates chain.

`otherwise_clause` details:

1. `948`: optional `OTHERWISE block` branch.
2. `949`: or empty (no otherwise).

Net TAC shape produced:

1. evaluate condition
2. `ifFalse -> else`
3. then block
4. `goto end`
5. else/else-if block(s)
6. `end` label

### 14.2 Loops (`orbit`, `orbit while`, `orbit times`) line-by-line

Relevant lines:

- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L952)

`ORBIT (expr)` branch:

1. `953`: starts first loop pattern.
2. `955`: builds `LoopContext` with start and end labels.
3. `956`: sets `continueLabel` to `startLabel` for this loop form.
4. `957`: pushes loop context on `loopStack`.
5. `958`: emits start label.
6. `960`: parses loop condition expression.
7. `962`: gets active context.
8. `963`: emits `ifFalse cond goto endLabel`.
9. `964`: frees condition attr.
10. `966`: parses loop body.
11. `968`: copies current context for close-out.
12. `969`: pops loop context.
13. `970`: emits back-edge `goto startLabel`.
14. `971`: emits end label.

`ORBIT WHILE (expr)` branch:

1. `973`: second syntax form starts.
2. `975-978`: same context creation/push/start-label flow.
3. `980-984`: parse condition and emit false-exit jump.
4. `986-992`: parse body, emit back-edge, emit end label.

`ORBIT TIMES (init : start : end)` branch:

1. `993`: matches counted loop form.
2. `995`: context includes explicit `continueLabel` and iterator place (`$4->place`), `hasIterator=true`.
3. `996`: pushes context.
4. `997`: emits assignment of iterator variable from `$6` (initial value expression).
5. `998`: emits start label.
6. `999`: emits compare temp `iterator < endExpr`.
7. `1000`: emits false-exit jump to end label.
8. `1001-1003`: frees consumed expression attrs.
9. `1005`: parses loop body.
10. `1007-1008`: get and pop context.
11. `1009`: checks iterator mode.
12. `1010`: emits continue label.
13. `1011`: emits increment temp `iterator + 1`.
14. `1012`: writes increment back to iterator place.
15. `1014`: emits back-edge `goto startLabel`.
16. `1015`: emits end label.

### 14.3 Function Definitions and Calls line-by-line

Relevant lines:

- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1224)
- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1395)
- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1937)

Function header setup (`COMMAND name (...)`):

1. `1225`: starts function definition pattern.
2. `1227`: stores function name from token.
3. `1228-1230`: clears previous parameter/type accumulators.
4. `1231`: detects whether definition is inside a module.
5. `1232`: resets return-tracking flag.
6. `1233`: sets owner module name if in module context.
7. `1234`: marks whether this is a method.
8. `1235`: marks constructor if method name == module name.
9. `1236`: clears qualified function id placeholder.

Signature + semantic registration block:

1. `1238`: parses parameter list, closing paren, return markers, and type.
2. `1240-1242`: constructor return-type guard (`voidspace` required).
3. `1244-1247`: builds unique qualified function name (`owner::name@arity` style).
4. `1249`: records arity (adds implicit `this` for methods).
5. `1250`: stores param type map.
6. `1251`: stores return type.
7. `1252`: sets current function return type for return-statement checks.

If module method (`1254+`):

1. `1255`: injects synthetic `this` type into parameter type map.
2. `1257-1260`: validates module context exists.
3. `1261-1267`: resolves inherited method candidate for override validation.
4. `1269-1271`: error if marked override but no base method.
5. `1273-1277`: override arity compatibility check.
6. `1279-1288`: builds `MethodInfo` record (owner, signature, return, visibility, selector).
7. `1290-1297`: checks duplicate arity overload in same module.
8. `1299-1304`: reports duplicate or registers method and selector mapping to TAC metadata.

If global function (`1306+`):

1. `1307-1311`: builds global overload info record.
2. `1313-1320`: duplicate-arity check.
3. `1322-1326`: report duplicate or register overload.

TAC function prologue emission:

1. `1329`: emits `func_begin qualifiedName`.
2. `1330-1332`: emits `param_def this` for methods.
3. `1333-1335`: emits one `param_def` per declared parameter.

Function body + epilogue:

1. `1337`: parses body block.
2. `1339-1342`: warns if non-voidspace function may exit without value return.
3. `1343`: emits default `return` to keep TAC structurally complete.
4. `1344`: emits `func_end`.
5. `1345-1356`: clears all function-context state so next definition starts clean.

Argument and call handling:

1. `1396-1401`: first argument emits `param` and sets count=1.
2. `1402-1407`: each additional argument emits `param` and increments count.
3. `1937`: matches function-call expression `name(args)`.
4. `1939-1942`: resolves overload by arity or emits semantic error.
5. `1944`: emits TAC `call` with resolved qualified function target.
6. `1945`: wraps returned temp in expression attribute.

### 14.4 Module/Class Handling line-by-line

Relevant lines:

- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1077)
- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1173)
- [backend/compiler/parser/parser.y](../backend/compiler/parser/parser.y#L1684)

Module definition entry:

1. `1078`: starts `MODULE IDENTIFIER` rule.
2. `1080`: remembers pending module name.
3. `1082-1088`: duplicate module check or insert new `ModuleInfo` in `moduleTable`.
4. `1090`: increments module nesting depth.
5. `1091`: sets `currentModuleName`.
6. `1092-1093`: default member flags reset (`public`, not override).
7. `1095`: parses inheritance + class body.
8. `1097-1100`: registers class metadata in TAC generator (`name`, `base`).
9. `1102-1106`: clears module context after module parse completes.

Inheritance validation:

1. `1111`: matches `EXTENDS Base` form.
2. `1113-1116`: validates inheritance context and base existence.
3. `1118-1119`: prevents cycles (`A extends A` or derived-cycle case).
4. `1121`: stores base name on module info.

Field declarations in class body:

1. `1176`: finds active module record.
2. `1179-1181`: duplicate field check.
3. `1182-1184`: creates `FieldInfo`, stores in module, registers default in TAC metadata.
4. `1187-1201`: same flow with initializer and type compatibility check.
5. `1204-1217`: constant field flow (`LIMIT`), marks immutable and registers default.

Object construction expression (`NEW`):

1. `1684`: matches `NEW Module(args)` expression.
2. `1686-1689`: validates module existence or returns unknown expr.
3. `1690`: allocates temp to hold object reference.
4. `1691`: emits TAC `obj_new`.
5. `1695-1703`: resolves constructor overload by arity and emits TAC method call if found.
6. `1705`: returns expression typed as `MODULE:Name`.

Method-call expression (`obj.method(args)`):

1. `1746-1754`: checks object declaration and module-type validity.
2. `1755-1758`: resolves method in class hierarchy.
3. `1761-1763`: enforces access control.
4. `1765-1767`: arity validation.
5. `1768`: emits TAC `mcall`.
6. `1769`: returns typed expression for call result.

Field-read expression (`obj.field` / `this.field` / `super.field`):

1. `1829-1837`: validates object existence and object type.
2. `1840-1843`: resolves field in hierarchy.
3. `1844-1846`: access check.
4. `1848-1849`: emits TAC field load and returns typed expression.
5. `1856-1870`: `this.field` variant with current-module context checks.
6. `1876-1895`: `super.field` variant with base-module context checks.

### 14.5 Why this is synchronized and not separate passes here

At these lines, parser actions are intentionally doing all three responsibilities immediately:

1. semantic legality checks (types, visibility, inheritance, overloads),
2. symbol/module metadata updates,
3. TAC emission.

This is why parser.y looks dense: it is the front-end and IR-lowering coordinator in one file.
