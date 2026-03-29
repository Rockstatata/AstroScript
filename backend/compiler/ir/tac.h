#ifndef TAC_H
#define TAC_H

#include <unordered_map>
#include <variant>
#include <string>
#include <vector>

struct TACInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

struct ObjectRef {
    int id;
};

class TACGenerator {
public:
    using RuntimeValue = std::variant<double, std::string, ObjectRef>;

    TACGenerator();

    std::string newTemp();
    std::string newLabel();

    void emit(const std::string& op, const std::string& arg1 = "", const std::string& arg2 = "", const std::string& result = "");

    std::string emitBinary(const std::string& op, const std::string& left, const std::string& right);
    std::string emitUnary(const std::string& op, const std::string& operand);

    void emitAssign(const std::string& target, const std::string& source);
    void emitDeclare(const std::string& type, const std::string& name);
    void emitScopeBegin();
    void emitScopeEnd();
    void emitPrint(const std::string& value);
    void emitLabel(const std::string& label);
    void emitGoto(const std::string& label);
    void emitIfFalseGoto(const std::string& condition, const std::string& label);
    void emitFunctionBegin(const std::string& name);
    void emitFunctionEnd(const std::string& name);
    void emitFunctionReturn(const std::string& valuePlace = "");
    void emitInput(const std::string& target);
    void emitArrayDecl(const std::string& type, const std::string& name, const std::string& size);
    void emitArrayStore(const std::string& name, const std::string& indexPlace, const std::string& valuePlace);
    std::string emitArrayLoad(const std::string& name, const std::string& indexPlace);
    void emitParam(const std::string& valuePlace);
    std::string emitCall(const std::string& functionName, int argumentCount);
    std::string emitMethodCall(const std::string& objectPlace, const std::string& methodName, int argumentCount);
    void emitParamDef(const std::string& name);
    void emitCallTarget(const std::string& functionName, const std::string& label);
    void emitObjectNew(const std::string& className, const std::string& target);
    void emitFieldStore(const std::string& objectPlace, const std::string& fieldName, const std::string& valuePlace);
    std::string emitFieldLoad(const std::string& objectPlace, const std::string& fieldName);

    void registerClass(const std::string& className, const std::string& baseClassName);
    void registerField(const std::string& className, const std::string& fieldName, const std::string& defaultValueToken);
    void registerMethod(const std::string& className, const std::string& methodName, const std::string& qualifiedFunctionName);

    void optimize();
    void printCode(const std::string& title = "Three Address Code") const;
    void printCTranslation(const std::string& title = "C-Like Translation") const;
    void execute();

private:
    std::vector<TACInstruction> instructions;
    int tempCounter;
    int labelCounter;

    struct Frame {
        std::unordered_map<std::string, RuntimeValue> variables;
    };

    std::vector<Frame> frames;
    std::unordered_map<std::string, std::vector<RuntimeValue>> arrays;
    std::unordered_map<std::string, int> labelToIndex;
    std::unordered_map<std::string, std::pair<int, int>> functionBounds;
    std::unordered_map<std::string, std::string> functionEntryLabel;
    std::vector<RuntimeValue> parameterStack;

    struct ClassDescriptor {
        std::string baseClass;
        std::unordered_map<std::string, RuntimeValue> fieldDefaults;
        std::unordered_map<std::string, std::string> methods;
    };

    struct ObjectInstance {
        std::string className;
        std::unordered_map<std::string, RuntimeValue> fields;
    };

    std::unordered_map<std::string, ClassDescriptor> classDescriptors;
    std::unordered_map<int, ObjectInstance> objectInstances;
    int nextObjectId;

    RuntimeValue getValue(const std::string& token) const;
    void setValue(const std::string& name, const RuntimeValue& value);
    static bool isStringLiteral(const std::string& token);
    static std::string unquote(const std::string& token);
    static std::string valueToString(const RuntimeValue& value);
    static double valueToNumber(const RuntimeValue& value);
    static bool valueToBool(const RuntimeValue& value);
    static bool isObjectRefValue(const RuntimeValue& value);
    static ObjectRef toObjectRef(const RuntimeValue& value);
    RuntimeValue evalBinary(const TACInstruction& instruction) const;
    void buildExecutionMetadata();

    RuntimeValue createObject(const std::string& className);
    bool hasField(const std::string& className, const std::string& fieldName) const;
    RuntimeValue getDefaultFieldValue(const std::string& className, const std::string& fieldName) const;
    std::string resolveMethodTarget(const std::string& className, const std::string& methodName) const;
    std::string resolveMethodTargetFromBase(const std::string& baseClassName, const std::string& methodName) const;

    static bool isNumeric(const std::string& value);
    static double toNumber(const std::string& value);
    static std::string toString(double value);

    void constantFold();
    void algebraicSimplify();
    void removeRedundantMoves();
};

#endif
