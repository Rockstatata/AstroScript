// AUTO-GENERATED FILE. DO NOT EDIT.
// Source: examples/modules/index.json

export const playgroundExamples = {
  "basics": {
    "label": "Module 1: Mission Basics",
    "description": "Entry mission, simple verify, and orbit loop",
    "sourceFile": "examples/modules/01-mission-basics.as",
    "code": "mission Apollo launch\n{\n  telemetry count astronauts := 3.\n\n  verify (astronauts > 0)\n  {\n    transmit \"Mission ready\".\n  }\n\n  orbit (astronauts > 0)\n  {\n    transmit astronauts.\n    astronauts := astronauts minus 1.\n  }\n}\nsuccess\n"
  },
  "declarationsTypes": {
    "label": "Module 2: Declarations and Types",
    "description": "Types, alias, constants, arrays, and fleet declarations",
    "sourceFile": "examples/modules/02-declarations-types.as",
    "code": "mission DeclarationsTypes launch\n{\n  alias count IntAlias.\n  telemetry count c := 7.\n  telemetry real r := 3.5.\n  telemetry precise p := 9.25.\n  telemetry flag ok := 1.\n  telemetry symbol msg := \"types\".\n  telemetry IntAlias custom := 11.\n  limit count MAX_LIMIT := 100.\n\n  fleet count dynFleet[4].\n  telemetry count arr[3].\n  dynFleet[0] := c add 1.\n  dynFleet[1] := dynFleet[0] add 2.\n  arr[0] := c.\n  arr[1] := custom.\n  arr[2] := MAX_LIMIT.\n\n  transmit c.\n  transmit r.\n  transmit p.\n  transmit ok.\n  transmit msg.\n  transmit custom.\n  transmit dynFleet[1].\n  transmit arr[1].\n}\nsuccess\n"
  },
  "operatorsKeywordAndSymbol": {
    "label": "Module 3: Operators (Keyword and Symbol)",
    "description": "Keyword operators and symbol operators side by side",
    "sourceFile": "examples/modules/03-operators-keyword-symbol.as",
    "code": "mission OperatorsShowcase launch\n{\n  telemetry count a := 12.\n  telemetry count b := 5.\n  telemetry count out.\n\n  out := a add b.\n  transmit out.\n  out := a + b.\n  transmit out.\n\n  out := a minus b.\n  transmit out.\n  out := a - b.\n  transmit out.\n\n  out := a mul b.\n  transmit out.\n  out := a * b.\n  transmit out.\n\n  out := a divide b.\n  transmit out.\n  out := a / b.\n  transmit out.\n\n  out := a mod b.\n  transmit out.\n\n  out := a ** 2.\n  transmit out.\n}\nsuccess\n"
  },
  "comparisonsAndLogic": {
    "label": "Module 4: Comparisons and Logic",
    "description": "Relational and logical branching patterns",
    "sourceFile": "examples/modules/04-comparisons-logic.as",
    "code": "mission ComparisonsLogic launch\n{\n  telemetry count x := 6.\n  telemetry count y := 3.\n\n  verify (x > y) { transmit \"GT\". }\n  verify (x < y) { transmit \"LT\". } otherwise { transmit \"NOT_LT\". }\n  verify (x >= y) { transmit \"GE\". }\n  verify (x <= y) { transmit \"LE\". } otherwise { transmit \"NOT_LE\". }\n  verify (x == y) { transmit \"EQ\". } otherwise { transmit \"NOT_EQ\". }\n  verify (x != y) { transmit \"NEQ\". }\n\n  verify ((x > 0) AND (y > 0)) { transmit \"AND\". }\n  verify ((x < 0) OR (y > 0)) { transmit \"OR\". }\n  verify ((x > 0) XOR (y < 0)) { transmit \"XOR\". }\n  verify (NOT (x < 0)) { transmit \"NOT\". }\n}\nsuccess\n"
  },
  "controlFlow": {
    "label": "Module 5: verify/else_verify/otherwise",
    "description": "Multi-branch conditional flow",
    "sourceFile": "examples/modules/05-control-flow.as",
    "code": "mission ControlFlow launch\n{\n  telemetry count fuel := 5.\n\n  verify (fuel > 3)\n  {\n    transmit \"Launch condition satisfied\".\n  }\n  else_verify (fuel == 3)\n  {\n    transmit \"Edge condition\".\n  }\n  otherwise\n  {\n    transmit \"Fuel below launch threshold\".\n  }\n}\nsuccess\n"
  },
  "loopsAndFlowControl": {
    "label": "Module 6: orbit/while/times + coast/stage_sep",
    "description": "Loop variants with continue and break behavior",
    "sourceFile": "examples/modules/06-loops-flow-control.as",
    "code": "mission LoopsFlowControl launch\n{\n  telemetry count i := 0.\n\n  orbit (i < 6)\n  {\n    i := i add 1.\n    verify (i == 2)\n    {\n      coast.\n    }\n\n    transmit i.\n\n    verify (i == 4)\n    {\n      stage_sep.\n    }\n  }\n\n  orbit while (i < 7)\n  {\n    transmit i.\n    i := i add 1.\n  }\n\n  orbit times (i : 0 : 3)\n  {\n    transmit i.\n  }\n}\nsuccess\n"
  },
  "scenarioSwitch": {
    "label": "Module 7: scenario/trajectory/fallback",
    "description": "Switch-like control flow with fallback",
    "sourceFile": "examples/modules/07-scenario-switch.as",
    "code": "mission ScenarioSwitch launch\n{\n  telemetry count code := 2.\n\n  scenario (code)\n  {\n    trajectory 1:\n      transmit \"case 1\".\n      stage_sep.\n    trajectory 2:\n      transmit \"case 2\".\n      stage_sep.\n    fallback:\n      transmit \"default\".\n  }\n}\nsuccess\n"
  },
  "functionsAndReturns": {
    "label": "Module 8: command/back/: and :->",
    "description": "Function declarations and return forms",
    "sourceFile": "examples/modules/08-functions-returns.as",
    "code": "mission FunctionsReturns launch\n{\n  command add_kw(count a, count b) : count\n  {\n    back a add b.\n  }\n\n  command mul_sym(count a, count b) :-> count\n  {\n    back a * b.\n  }\n\n  command ping() : voidspace\n  {\n    transmit \"ping\".\n    back.\n  }\n\n  telemetry count r1 := add_kw(7, 8).\n  telemetry count r2 := mul_sym(3, 4).\n  transmit r1.\n  transmit r2.\n  ping().\n}\nsuccess\n"
  },
  "ioAndMessaging": {
    "label": "Module 9: transmit/broadcast/alarm/receive",
    "description": "Output channels plus runtime input via receive",
    "sourceFile": "examples/modules/09-io-messaging.as",
    "code": "mission IOMessaging launch\n{\n  telemetry symbol txt := \"hello\".\n  telemetry count val.\n\n  transmit txt.\n  broadcast \"broadcast channel\".\n  alarm \"alarm channel\".\n  receive val.\n  transmit val.\n}\nsuccess\n"
  },
  "waitAndMathBuiltins": {
    "label": "Module 10: wait/tick + Math Built-ins",
    "description": "Runtime delay and math built-in functions",
    "sourceFile": "examples/modules/10-wait-math-builtins.as",
    "code": "mission WaitMath launch\n{\n  telemetry count x := 9.\n  telemetry count y.\n\n  y := root(x).\n  transmit y.\n  y := flr(3.9).\n  transmit y.\n  y := ceil(3.1).\n  transmit y.\n  y := abs(-22).\n  transmit y.\n  y := logarithm(1).\n  transmit y.\n  y := sine(0).\n  transmit y.\n  y := cosine(0).\n  transmit y.\n  y := tan(0).\n  transmit y.\n  y := asine(0).\n  transmit y.\n  y := acosine(1).\n  transmit y.\n  y := atan(1).\n  transmit y.\n  y := prime(17).\n  transmit y.\n\n  wait 1 tick.\n}\nsuccess\n"
  },
  "modulesDeployThis": {
    "label": "Module 11: OOP (extends/override/super/new)",
    "description": "Modules, inheritance, override, and object construction",
    "sourceFile": "examples/modules/11-oop-extends-override.as",
    "code": "mission MissionOOP launch\n{\n  module BaseModule\n  {\n    public telemetry count id := 0.\n\n    command BaseModule(count initial) : voidspace\n    {\n      this.id := initial.\n    }\n\n    command base_id() : count\n    {\n      back this.id.\n    }\n\n    command add_task(count incoming) : count\n    {\n      this.id := this.id add incoming.\n      back this.id.\n    }\n  }\n\n  module ChildModule extends BaseModule\n  {\n    public telemetry count emergencyBoost := 2.\n\n    command ChildModule(count initial) : voidspace\n    {\n      this.id := initial.\n    }\n\n    override command add_task(count incoming) : count\n    {\n      back super.add_task(incoming add this.emergencyBoost).\n    }\n  }\n\n  deploy ChildModule ship(10).\n  transmit ship.add_task(3).\n  transmit ship.id.\n\n  telemetry ChildModule backup := new ChildModule(20).\n  transmit backup.add_task(1).\n}\nsuccess\n"
  },
  "overloadingAndScope": {
    "label": "Module 12: Overloading + Scoped Variables",
    "description": "Function/method overloads and block-level shadowing",
    "sourceFile": "examples/modules/12-overloading-scope.as",
    "code": "mission OverloadingScope launch\n{\n  command route(count normal) : count\n  {\n    back normal add 1.\n  }\n\n  command route(count normal, count emergency) : count\n  {\n    back normal add emergency.\n  }\n\n  module MissionQueue\n  {\n    public telemetry count backlog := 0.\n\n    command MissionQueue(count seed) : voidspace\n    {\n      this.backlog := seed.\n    }\n\n    command enqueue(count tasks) : count\n    {\n      this.backlog := this.backlog add tasks.\n      back this.backlog.\n    }\n\n    command enqueue(count tasks, count priority) : count\n    {\n      this.backlog := this.backlog add tasks add priority.\n      back this.backlog.\n    }\n  }\n\n  deploy MissionQueue q(5).\n\n  telemetry count r1 := route(3).\n  telemetry count r2 := route(3, 4).\n  transmit r1.\n  transmit r2.\n  transmit q.enqueue(2).\n  transmit q.enqueue(2, 1).\n\n  telemetry count backlog := 77.\n  verify (1 == 1)\n  {\n    telemetry count backlog := 500.\n    transmit backlog.\n  }\n\n  transmit backlog.\n}\nsuccess\n"
  },
  "modeAndAbort": {
    "label": "Module 13: mode + abort",
    "description": "Mode declaration and mission abort",
    "sourceFile": "examples/modules/13-mode-abort.as",
    "code": "mission ModeAbort launch\n{\n  mode MissionState\n  {\n    trajectory 1.\n    trajectory 2.\n    fallback 0.\n  }\n\n  telemetry count code := 1.\n  transmit code.\n  abort.\n}\nsuccess\n"
  },
  "nestedLoopsGrid": {
    "label": "Module 14: Nested Loops Grid",
    "description": "Nested orbit-times loops for grid processing",
    "sourceFile": "examples/modules/14-nested-loops-grid.as",
    "code": "mission NestedLoopsGrid launch\n{\n  telemetry count row.\n  telemetry count col.\n  telemetry count total := 0.\n\n  orbit times (row : 0 : 3)\n  {\n    orbit times (col : 0 : 3)\n    {\n      telemetry count cell := row mul 10 add col.\n      transmit cell.\n      total := total add cell.\n    }\n  }\n\n  transmit total.\n}\nsuccess\n"
  },
  "nestedIfElse": {
    "label": "Module 15: Nested verify/else branches",
    "description": "Nested if/else trees inside outer conditions",
    "sourceFile": "examples/modules/15-nested-if-else.as",
    "code": "mission NestedIfElse launch\n{\n  telemetry count fuel := 7.\n  telemetry count signal := 3.\n\n  verify (fuel > 0)\n  {\n    verify (signal > 5)\n    {\n      transmit \"High fuel, high signal\".\n    }\n    else_verify (signal > 2)\n    {\n      verify (fuel > 5)\n      {\n        transmit \"High fuel, medium signal\".\n      }\n      otherwise\n      {\n        transmit \"Low fuel, medium signal\".\n      }\n    }\n    otherwise\n    {\n      transmit \"High fuel, low signal\".\n    }\n  }\n  else_verify (fuel == 0)\n  {\n    transmit \"No fuel\".\n  }\n  otherwise\n  {\n    transmit \"Invalid fuel\".\n  }\n}\nsuccess\n"
  },
  "nestedFunctionCalls": {
    "label": "Module 16: Nested Function Calls",
    "description": "Function chaining and function-in-function calls",
    "sourceFile": "examples/modules/16-nested-function-calls.as",
    "code": "mission NestedFunctionCalls launch\n{\n  command boost(count base) : count\n  {\n    back base add 2.\n  }\n\n  command triple(count base) : count\n  {\n    back base mul 3.\n  }\n\n  command nested_compute(count seed) : count\n  {\n    telemetry count staged := triple(boost(seed)).\n    back boost(staged).\n  }\n\n  telemetry count result := nested_compute(boost(4)).\n  transmit result.\n}\nsuccess\n"
  },
  "nestedControlFlowMixed": {
    "label": "Module 17: Nested Mixed Control Flow",
    "description": "Nested loops with nested branching and coast control",
    "sourceFile": "examples/modules/17-nested-control-flow-mixed.as",
    "code": "mission NestedControlFlowMixed launch\n{\n  telemetry count outer := 0.\n  telemetry count inner.\n\n  orbit (outer < 3)\n  {\n    inner := 0.\n\n    orbit (inner < 4)\n    {\n      verify ((outer add inner) mod 2 == 0)\n      {\n        transmit outer add inner.\n      }\n      otherwise\n      {\n        verify (inner == 3)\n        {\n          transmit outer mul 100 add inner.\n        }\n        otherwise\n        {\n          inner := inner add 1.\n          coast.\n        }\n      }\n\n      inner := inner add 1.\n    }\n\n    outer := outer add 1.\n  }\n}\nsuccess\n"
  },
  "nestedScenarioLoops": {
    "label": "Module 18: Nested scenario inside loops",
    "description": "Nested scenario/trajectory blocks in iterative flow",
    "sourceFile": "examples/modules/18-nested-scenario-loops.as",
    "code": "mission NestedScenarioLoops launch\n{\n  telemetry count lane.\n  telemetry count scenarioMode := 2.\n\n  orbit times (lane : 0 : 4)\n  {\n    scenario (scenarioMode)\n    {\n      trajectory 1:\n        transmit lane.\n        stage_sep.\n      trajectory 2:\n        verify (lane mod 2 == 0)\n        {\n          transmit lane mul 10.\n        }\n        otherwise\n        {\n          transmit lane mul 100.\n        }\n        stage_sep.\n      fallback:\n        transmit -1.\n    }\n  }\n}\nsuccess\n"
  },
  "nestedModuleMethods": {
    "label": "Module 19: Nested Module Method Calls",
    "description": "Method-to-method calls using this inside modules",
    "sourceFile": "examples/modules/19-nested-module-methods.as",
    "code": "mission NestedModuleMethods launch\n{\n  module Engine\n  {\n    public telemetry count power := 0.\n\n    command Engine(count seed) : voidspace\n    {\n      this.power := seed.\n    }\n\n    command calibrate(count delta) : count\n    {\n      this.power := this.power add delta.\n      back this.power.\n    }\n\n    command ignite(count level) : count\n    {\n      telemetry count tuned := this.calibrate(level).\n      back tuned mul 2.\n    }\n  }\n\n  deploy Engine e(5).\n  transmit e.ignite(3).\n  transmit e.calibrate(2).\n  transmit e.power.\n}\nsuccess\n"
  },
  "arrayFleetMatrix": {
    "label": "Module 20: Array/Fleet Matrix Walk",
    "description": "Nested indexing patterns over array and fleet storage",
    "sourceFile": "examples/modules/20-array-fleet-matrix.as",
    "code": "mission ArrayFleetMatrix launch\n{\n  telemetry count row.\n  telemetry count col.\n  telemetry count index.\n  fleet count matrix[9].\n  telemetry count last.\n\n  orbit times (row : 0 : 3)\n  {\n    orbit times (col : 0 : 3)\n    {\n      index := row mul 3 add col.\n      matrix[index] := row mul 10 add col.\n    }\n  }\n\n  last := 0.\n  orbit times (index : 0 : 9)\n  {\n    transmit matrix[index].\n    last := matrix[index].\n  }\n\n  transmit last.\n}\nsuccess\n"
  }
} as const;
