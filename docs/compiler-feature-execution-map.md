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
