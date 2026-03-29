#include "tac.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <new>
#include <sstream>
#include <unordered_set>

namespace {
double portableModulo(double left, double right) {
    const double quotient = left / right;

    // Truncate toward zero to match fmod semantics without calling libm fmod.
    const long long truncatedQuotient = static_cast<long long>(quotient);
    return left - (static_cast<double>(truncatedQuotient) * right);
}

bool tryParseInt(const std::string& text, int& value) {
    if (text.empty()) {
        return false;
    }

    std::size_t index = 0;
    int sign = 1;
    if (text[index] == '-') {
        sign = -1;
        ++index;
    }

    if (index >= text.size()) {
        return false;
    }

    long long parsed = 0;
    for (; index < text.size(); ++index) {
        const char ch = text[index];
        if (ch < '0' || ch > '9') {
            return false;
        }

        parsed = (parsed * 10) + (ch - '0');
        if (parsed > static_cast<long long>(std::numeric_limits<int>::max()) + 1LL) {
            return false;
        }
    }

    const long long signedValue = sign < 0 ? -parsed : parsed;
    if (signedValue < std::numeric_limits<int>::min() || signedValue > std::numeric_limits<int>::max()) {
        return false;
    }

    value = static_cast<int>(signedValue);
    return true;
}
}

TACGenerator::TACGenerator() : tempCounter(0), labelCounter(0), nextObjectId(1) {}

std::string TACGenerator::newTemp() {
    return "t" + std::to_string(++tempCounter);
}

std::string TACGenerator::newLabel() {
    return "L" + std::to_string(++labelCounter);
}

void TACGenerator::emit(const std::string& op, const std::string& arg1, const std::string& arg2, const std::string& result) {
    instructions.push_back({op, arg1, arg2, result});
}

std::string TACGenerator::emitBinary(const std::string& op, const std::string& left, const std::string& right) {
    std::string temp = newTemp();
    emit(op, left, right, temp);
    return temp;
}

std::string TACGenerator::emitUnary(const std::string& op, const std::string& operand) {
    std::string temp = newTemp();
    emit(op, operand, "", temp);
    return temp;
}

void TACGenerator::emitAssign(const std::string& target, const std::string& source) {
    emit("=", source, "", target);
}

void TACGenerator::emitDeclare(const std::string& type, const std::string& name) {
    emit("decl", type, "", name);
}

void TACGenerator::emitScopeBegin() {
    emit("scope_begin", "", "", "");
}

void TACGenerator::emitScopeEnd() {
    emit("scope_end", "", "", "");
}

void TACGenerator::emitArrayDecl(const std::string& type, const std::string& name, const std::string& size) {
    emit("decl_arr", type, size, name);
}

void TACGenerator::emitArrayStore(const std::string& name, const std::string& indexPlace, const std::string& valuePlace) {
    emit("store", name, indexPlace, valuePlace);
}

std::string TACGenerator::emitArrayLoad(const std::string& name, const std::string& indexPlace) {
    std::string temp = newTemp();
    emit("load", name, indexPlace, temp);
    return temp;
}

void TACGenerator::emitPrint(const std::string& value) {
    emit("print", value, "", "");
}

void TACGenerator::emitInput(const std::string& target) {
    emit("input", "", "", target);
}

void TACGenerator::emitLabel(const std::string& label) {
    emit("label", "", "", label);
}

void TACGenerator::emitGoto(const std::string& label) {
    emit("goto", "", "", label);
}

void TACGenerator::emitIfFalseGoto(const std::string& condition, const std::string& label) {
    emit("ifFalse", condition, "", label);
}

void TACGenerator::emitFunctionBegin(const std::string& name) {
    emit("func_begin", name, "", "");
}

void TACGenerator::emitFunctionEnd(const std::string& name) {
    emit("func_end", name, "", "");
}

void TACGenerator::emitFunctionReturn(const std::string& valuePlace) {
    emit("return", valuePlace, "", "");
}

void TACGenerator::emitParam(const std::string& valuePlace) {
    emit("param", valuePlace, "", "");
}

std::string TACGenerator::emitCall(const std::string& functionName, int argumentCount) {
    std::string temp = newTemp();
    emit("call", functionName, std::to_string(argumentCount), temp);
    return temp;
}

std::string TACGenerator::emitMethodCall(const std::string& objectPlace, const std::string& methodName, int argumentCount) {
    std::string temp = newTemp();
    emit("mcall", objectPlace, methodName + "|" + std::to_string(argumentCount), temp);
    return temp;
}

void TACGenerator::emitObjectNew(const std::string& className, const std::string& target) {
    emit("obj_new", className, "", target);
}

void TACGenerator::emitFieldStore(const std::string& objectPlace, const std::string& fieldName, const std::string& valuePlace) {
    emit("field_set", objectPlace, fieldName, valuePlace);
}

std::string TACGenerator::emitFieldLoad(const std::string& objectPlace, const std::string& fieldName) {
    std::string temp = newTemp();
    emit("field_get", objectPlace, fieldName, temp);
    return temp;
}

void TACGenerator::emitParamDef(const std::string& name) {
    emit("param_def", name, "", "");
}

void TACGenerator::emitCallTarget(const std::string& functionName, const std::string& label) {
    functionEntryLabel[functionName] = label;
}

void TACGenerator::registerClass(const std::string& className, const std::string& baseClassName) {
    ClassDescriptor& descriptor = classDescriptors[className];
    descriptor.baseClass = baseClassName;
}

void TACGenerator::registerField(const std::string& className, const std::string& fieldName, const std::string& defaultValueToken) {
    ClassDescriptor& descriptor = classDescriptors[className];
    descriptor.fieldDefaults[fieldName] = getValue(defaultValueToken.empty() ? "0" : defaultValueToken);
}

void TACGenerator::registerMethod(const std::string& className, const std::string& methodName, const std::string& qualifiedFunctionName) {
    ClassDescriptor& descriptor = classDescriptors[className];
    descriptor.methods[methodName] = qualifiedFunctionName;
}

bool TACGenerator::isNumeric(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    char* endPtr = nullptr;
    std::strtod(value.c_str(), &endPtr);
    return endPtr != value.c_str() && *endPtr == '\0';
}

bool TACGenerator::isStringLiteral(const std::string& token) {
    return token.size() >= 2 && token.front() == '"' && token.back() == '"';
}

std::string TACGenerator::unquote(const std::string& token) {
    if (!isStringLiteral(token)) {
        return token;
    }
    return token.substr(1, token.size() - 2);
}

double TACGenerator::toNumber(const std::string& value) {
    return std::stod(value);
}

std::string TACGenerator::toString(double value) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(6) << value;
    std::string asText = stream.str();

    while (!asText.empty() && asText.back() == '0') {
        asText.pop_back();
    }
    if (!asText.empty() && asText.back() == '.') {
        asText.pop_back();
    }
    if (asText.empty()) {
        return "0";
    }
    return asText;
}

std::string TACGenerator::valueToString(const RuntimeValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    if (std::holds_alternative<ObjectRef>(value)) {
        return "@obj" + std::to_string(std::get<ObjectRef>(value).id);
    }
    return toString(std::get<double>(value));
}

double TACGenerator::valueToNumber(const RuntimeValue& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }

    if (std::holds_alternative<ObjectRef>(value)) {
        return static_cast<double>(std::get<ObjectRef>(value).id);
    }

    const std::string& text = std::get<std::string>(value);
    if (isNumeric(text)) {
        return toNumber(text);
    }

    return 0.0;
}

bool TACGenerator::valueToBool(const RuntimeValue& value) {
    if (std::holds_alternative<double>(value)) {
        return std::fabs(std::get<double>(value)) > 1e-12;
    }
    if (std::holds_alternative<ObjectRef>(value)) {
        return std::get<ObjectRef>(value).id > 0;
    }
    return !std::get<std::string>(value).empty();
}

bool TACGenerator::isObjectRefValue(const RuntimeValue& value) {
    return std::holds_alternative<ObjectRef>(value);
}

ObjectRef TACGenerator::toObjectRef(const RuntimeValue& value) {
    if (std::holds_alternative<ObjectRef>(value)) {
        return std::get<ObjectRef>(value);
    }
    return ObjectRef{0};
}

TACGenerator::RuntimeValue TACGenerator::getValue(const std::string& token) const {
    if (token.empty()) {
        return 0.0;
    }

    if (isStringLiteral(token)) {
        return unquote(token);
    }

    if (isNumeric(token)) {
        return toNumber(token);
    }

    for (auto frameIt = frames.rbegin(); frameIt != frames.rend(); ++frameIt) {
        auto valueIt = frameIt->variables.find(token);
        if (valueIt != frameIt->variables.end()) {
            return valueIt->second;
        }
    }

    return 0.0;
}

void TACGenerator::setValue(const std::string& name, const RuntimeValue& value) {
    for (auto frameIt = frames.rbegin(); frameIt != frames.rend(); ++frameIt) {
        auto valueIt = frameIt->variables.find(name);
        if (valueIt != frameIt->variables.end()) {
            valueIt->second = value;
            return;
        }
    }

    if (frames.empty()) {
        frames.push_back(Frame{});
    }
    frames.back().variables[name] = value;
}

TACGenerator::RuntimeValue TACGenerator::evalBinary(const TACInstruction& instruction) const {
    RuntimeValue left = getValue(instruction.arg1);
    RuntimeValue right = getValue(instruction.arg2);

    if (instruction.op == "+") {
        if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
            return valueToString(left) + valueToString(right);
        }
        return valueToNumber(left) + valueToNumber(right);
    }

    double l = valueToNumber(left);
    double r = valueToNumber(right);

    if (instruction.op == "-") return l - r;
    if (instruction.op == "*") return l * r;
    if (instruction.op == "/") {
        if (std::fabs(r) < 1e-12) {
            std::cerr << "RUNTIME ERROR: division by zero" << std::endl;
            return 0.0;
        }
        return l / r;
    }
    if (instruction.op == "%") {
        if (std::fabs(r) < 1e-12) {
            std::cerr << "RUNTIME ERROR: modulo by zero" << std::endl;
            return 0.0;
        }
        return portableModulo(l, r);
    }
    if (instruction.op == "**") return std::pow(l, r);
    if (instruction.op == "AND") return (valueToBool(left) && valueToBool(right)) ? 1.0 : 0.0;
    if (instruction.op == "OR") return (valueToBool(left) || valueToBool(right)) ? 1.0 : 0.0;
    if (instruction.op == "XOR") return (valueToBool(left) != valueToBool(right)) ? 1.0 : 0.0;
    if (instruction.op == "<") return (l < r) ? 1.0 : 0.0;
    if (instruction.op == ">") return (l > r) ? 1.0 : 0.0;
    if (instruction.op == "<=") return (l <= r) ? 1.0 : 0.0;
    if (instruction.op == ">=") return (l >= r) ? 1.0 : 0.0;

    if (instruction.op == "==") {
        if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
            return (valueToString(left) == valueToString(right)) ? 1.0 : 0.0;
        }
        return (l == r) ? 1.0 : 0.0;
    }

    if (instruction.op == "!=") {
        if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
            return (valueToString(left) != valueToString(right)) ? 1.0 : 0.0;
        }
        return (l != r) ? 1.0 : 0.0;
    }

    return 0.0;
}

TACGenerator::RuntimeValue TACGenerator::createObject(const std::string& className) {
    int objectId = nextObjectId++;
    ObjectInstance instance;
    instance.className = className;

    std::vector<std::string> hierarchy;
    std::string cursor = className;
    while (!cursor.empty()) {
        hierarchy.push_back(cursor);
        auto classIt = classDescriptors.find(cursor);
        if (classIt == classDescriptors.end()) {
            break;
        }
        cursor = classIt->second.baseClass;
    }
    std::reverse(hierarchy.begin(), hierarchy.end());

    for (const std::string& hierarchyClass : hierarchy) {
        auto classIt = classDescriptors.find(hierarchyClass);
        if (classIt == classDescriptors.end()) {
            continue;
        }
        for (const auto& fieldEntry : classIt->second.fieldDefaults) {
            instance.fields[fieldEntry.first] = fieldEntry.second;
        }
    }

    objectInstances[objectId] = instance;
    return ObjectRef{objectId};
}

bool TACGenerator::hasField(const std::string& className, const std::string& fieldName) const {
    std::string cursor = className;
    while (!cursor.empty()) {
        auto classIt = classDescriptors.find(cursor);
        if (classIt == classDescriptors.end()) {
            break;
        }
        if (classIt->second.fieldDefaults.find(fieldName) != classIt->second.fieldDefaults.end()) {
            return true;
        }
        cursor = classIt->second.baseClass;
    }
    return false;
}

TACGenerator::RuntimeValue TACGenerator::getDefaultFieldValue(const std::string& className, const std::string& fieldName) const {
    std::string cursor = className;
    while (!cursor.empty()) {
        auto classIt = classDescriptors.find(cursor);
        if (classIt == classDescriptors.end()) {
            break;
        }
        auto fieldIt = classIt->second.fieldDefaults.find(fieldName);
        if (fieldIt != classIt->second.fieldDefaults.end()) {
            return fieldIt->second;
        }
        cursor = classIt->second.baseClass;
    }
    return 0.0;
}

std::string TACGenerator::resolveMethodTarget(const std::string& className, const std::string& methodName) const {
    std::string cursor = className;
    while (!cursor.empty()) {
        auto classIt = classDescriptors.find(cursor);
        if (classIt == classDescriptors.end()) {
            break;
        }
        auto methodIt = classIt->second.methods.find(methodName);
        if (methodIt != classIt->second.methods.end()) {
            return methodIt->second;
        }
        cursor = classIt->second.baseClass;
    }
    return "";
}

std::string TACGenerator::resolveMethodTargetFromBase(const std::string& baseClassName, const std::string& methodName) const {
    std::string cursor = baseClassName;
    while (!cursor.empty()) {
        auto classIt = classDescriptors.find(cursor);
        if (classIt == classDescriptors.end()) {
            break;
        }
        auto methodIt = classIt->second.methods.find(methodName);
        if (methodIt != classIt->second.methods.end()) {
            return methodIt->second;
        }
        cursor = classIt->second.baseClass;
    }
    return "";
}

void TACGenerator::buildExecutionMetadata() {
    labelToIndex.clear();
    functionBounds.clear();

    std::vector<std::pair<std::string, int>> functionStack;
    for (std::size_t i = 0; i < instructions.size(); ++i) {
        const auto& instruction = instructions[i];

        if (instruction.op == "label") {
            labelToIndex[instruction.result] = static_cast<int>(i);
        } else if (instruction.op == "func_begin") {
            functionStack.push_back({instruction.arg1, static_cast<int>(i)});
        } else if (instruction.op == "func_end" && !functionStack.empty()) {
            auto function = functionStack.back();
            functionStack.pop_back();
            functionBounds[function.first] = {function.second, static_cast<int>(i)};
        }
    }
}

void TACGenerator::constantFold() {
    for (auto& instruction : instructions) {
        if (instruction.op != "+" && instruction.op != "-" && instruction.op != "*" && instruction.op != "/" &&
            instruction.op != "%" && instruction.op != "**" &&
            instruction.op != "AND" && instruction.op != "OR" && instruction.op != "XOR" &&
            instruction.op != "<" && instruction.op != ">" && instruction.op != "<=" && instruction.op != ">=" &&
            instruction.op != "==" && instruction.op != "!=") {
            continue;
        }

        if (!isNumeric(instruction.arg1) || !isNumeric(instruction.arg2)) {
            continue;
        }

        double left = toNumber(instruction.arg1);
        double right = toNumber(instruction.arg2);
        double folded = 0.0;

        if (instruction.op == "+") folded = left + right;
        else if (instruction.op == "-") folded = left - right;
        else if (instruction.op == "*") folded = left * right;
        else if (instruction.op == "/") {
            if (right == 0.0) {
                continue;
            }
            folded = left / right;
        }
        else if (instruction.op == "%") {
            if (right == 0.0) {
                continue;
            }
            folded = portableModulo(left, right);
        }
        else if (instruction.op == "**") folded = std::pow(left, right);
        else if (instruction.op == "AND") folded = ((std::fabs(left) > 1e-12) && (std::fabs(right) > 1e-12)) ? 1.0 : 0.0;
        else if (instruction.op == "OR") folded = ((std::fabs(left) > 1e-12) || (std::fabs(right) > 1e-12)) ? 1.0 : 0.0;
        else if (instruction.op == "XOR") folded = ((std::fabs(left) > 1e-12) != (std::fabs(right) > 1e-12)) ? 1.0 : 0.0;
        else if (instruction.op == "<") folded = (left < right) ? 1.0 : 0.0;
        else if (instruction.op == ">") folded = (left > right) ? 1.0 : 0.0;
        else if (instruction.op == "<=") folded = (left <= right) ? 1.0 : 0.0;
        else if (instruction.op == ">=") folded = (left >= right) ? 1.0 : 0.0;
        else if (instruction.op == "==") folded = (left == right) ? 1.0 : 0.0;
        else if (instruction.op == "!=") folded = (left != right) ? 1.0 : 0.0;

        instruction.op = "=";
        instruction.arg1 = toString(folded);
        instruction.arg2.clear();
    }

    for (auto& instruction : instructions) {
        if (instruction.op != "root" && instruction.op != "flr" && instruction.op != "ceil" && instruction.op != "abs" &&
            instruction.op != "logarithm" && instruction.op != "sine" && instruction.op != "cosine" && instruction.op != "tan" &&
            instruction.op != "asine" && instruction.op != "acosine" && instruction.op != "atan" && instruction.op != "prime") {
            continue;
        }

        if (!isNumeric(instruction.arg1)) {
            continue;
        }

        double value = toNumber(instruction.arg1);
        double folded = 0.0;

        if (instruction.op == "root") {
            if (value < 0.0) {
                continue;
            }
            folded = std::sqrt(value);
        } else if (instruction.op == "flr") {
            folded = std::floor(value);
        } else if (instruction.op == "ceil") {
            folded = std::ceil(value);
        } else if (instruction.op == "abs") {
            folded = std::fabs(value);
        } else if (instruction.op == "logarithm") {
            if (value <= 0.0) {
                continue;
            }
            folded = std::log(value);
        } else if (instruction.op == "sine") {
            folded = std::sin(value);
        } else if (instruction.op == "cosine") {
            folded = std::cos(value);
        } else if (instruction.op == "tan") {
            folded = std::tan(value);
        } else if (instruction.op == "asine") {
            if (value < -1.0 || value > 1.0) {
                continue;
            }
            folded = std::asin(value);
        } else if (instruction.op == "acosine") {
            if (value < -1.0 || value > 1.0) {
                continue;
            }
            folded = std::acos(value);
        } else if (instruction.op == "atan") {
            folded = std::atan(value);
        } else if (instruction.op == "prime") {
            long long n = static_cast<long long>(std::llround(value));
            if (n < 2) {
                folded = 0.0;
            } else {
                bool isPrime = true;
                for (long long i = 2; i * i <= n; ++i) {
                    if (n % i == 0) {
                        isPrime = false;
                        break;
                    }
                }
                folded = isPrime ? 1.0 : 0.0;
            }
        }

        instruction.op = "=";
        instruction.arg1 = toString(folded);
        instruction.arg2.clear();
    }
}

void TACGenerator::algebraicSimplify() {
    for (auto& instruction : instructions) {
        if (instruction.op == "+") {
            if (instruction.arg1 == "0") {
                instruction.op = "=";
                instruction.arg1 = instruction.arg2;
                instruction.arg2.clear();
            } else if (instruction.arg2 == "0") {
                instruction.op = "=";
                instruction.arg2.clear();
            }
        } else if (instruction.op == "-") {
            if (instruction.arg2 == "0") {
                instruction.op = "=";
                instruction.arg2.clear();
            }
        } else if (instruction.op == "*") {
            if (instruction.arg1 == "1") {
                instruction.op = "=";
                instruction.arg1 = instruction.arg2;
                instruction.arg2.clear();
            } else if (instruction.arg2 == "1") {
                instruction.op = "=";
                instruction.arg2.clear();
            } else if (instruction.arg1 == "0" || instruction.arg2 == "0") {
                instruction.op = "=";
                instruction.arg1 = "0";
                instruction.arg2.clear();
            }
        } else if (instruction.op == "/") {
            if (instruction.arg2 == "1") {
                instruction.op = "=";
                instruction.arg2.clear();
            }
        }
    }
}

void TACGenerator::removeRedundantMoves() {
    std::vector<TACInstruction> filtered;
    filtered.reserve(instructions.size());

    for (const auto& instruction : instructions) {
        if (instruction.op == "=" && instruction.arg1 == instruction.result) {
            continue;
        }
        filtered.push_back(instruction);
    }

    instructions.swap(filtered);
}

void TACGenerator::optimize() {
    constantFold();
    algebraicSimplify();
    removeRedundantMoves();
}

void TACGenerator::printCode(const std::string& title) const {
    std::cout << "\n--- " << title << " ---\n";

    for (std::size_t index = 0; index < instructions.size(); ++index) {
        const auto& instruction = instructions[index];
        std::cout << std::setw(3) << index << ": ";

        if (instruction.op == "label") {
            std::cout << instruction.result << ":";
        } else if (instruction.op == "goto") {
            std::cout << "goto " << instruction.result;
        } else if (instruction.op == "ifFalse") {
            std::cout << "ifFalse " << instruction.arg1 << " goto " << instruction.result;
        } else if (instruction.op == "print") {
            std::cout << "print " << instruction.arg1;
        } else if (instruction.op == "input") {
            std::cout << "input " << instruction.result;
        } else if (instruction.op == "decl") {
            std::cout << "decl " << instruction.arg1 << " " << instruction.result;
        } else if (instruction.op == "scope_begin") {
            std::cout << "scope_begin";
        } else if (instruction.op == "scope_end") {
            std::cout << "scope_end";
        } else if (instruction.op == "decl_arr") {
            std::cout << "decl_arr " << instruction.arg1 << " " << instruction.result << "[" << instruction.arg2 << "]";
        } else if (instruction.op == "store") {
            std::cout << instruction.arg1 << "[" << instruction.arg2 << "] = " << instruction.result;
        } else if (instruction.op == "load") {
            std::cout << instruction.result << " = " << instruction.arg1 << "[" << instruction.arg2 << "]";
        } else if (instruction.op == "func_begin") {
            std::cout << "func " << instruction.arg1 << " begin";
        } else if (instruction.op == "func_end") {
            std::cout << "func " << instruction.arg1 << " end";
        } else if (instruction.op == "return") {
            std::cout << "return " << instruction.arg1;
        } else if (instruction.op == "param") {
            std::cout << "param " << instruction.arg1;
        } else if (instruction.op == "param_def") {
            std::cout << "param_def " << instruction.arg1;
        } else if (instruction.op == "call") {
            std::cout << instruction.result << " = call " << instruction.arg1 << " (" << instruction.arg2 << " args)";
        } else if (instruction.op == "mcall") {
            std::cout << instruction.result << " = mcall " << instruction.arg1 << " . " << instruction.arg2;
        } else if (instruction.op == "obj_new") {
            std::cout << instruction.result << " = new " << instruction.arg1;
        } else if (instruction.op == "field_set") {
            std::cout << instruction.arg1 << "." << instruction.arg2 << " = " << instruction.result;
        } else if (instruction.op == "field_get") {
            std::cout << instruction.result << " = " << instruction.arg1 << "." << instruction.arg2;
        } else if (instruction.op == "=") {
            std::cout << instruction.result << " = " << instruction.arg1;
        } else if (instruction.op == "root" || instruction.op == "flr" || instruction.op == "ceil" || instruction.op == "abs" ||
                   instruction.op == "logarithm" || instruction.op == "sine" || instruction.op == "cosine" || instruction.op == "tan" ||
                   instruction.op == "asine" || instruction.op == "acosine" || instruction.op == "atan" || instruction.op == "prime") {
            std::cout << instruction.result << " = " << instruction.op << "(" << instruction.arg1 << ")";
        } else {
            std::cout << instruction.result << " = " << instruction.arg1 << " " << instruction.op << " " << instruction.arg2;
        }

        std::cout << '\n';
    }

    std::cout << "-----------------------------\n";
}

void TACGenerator::printCTranslation(const std::string& title) const {
    std::cout << "\n--- " << title << " ---\n";

    auto toIdentifier = [](const std::string& raw) {
        if (raw.empty()) {
            return std::string("tmp");
        }

        std::string out;
        out.reserve(raw.size() + 8);

        for (char ch : raw) {
            const unsigned char uch = static_cast<unsigned char>(ch);
            if (std::isalnum(uch) || ch == '_') {
                out.push_back(ch);
            } else {
                out.push_back('_');
            }
        }

        if (out.empty()) {
            out = "tmp";
        }

        const unsigned char first = static_cast<unsigned char>(out.front());
        if (!std::isalpha(first) && out.front() != '_') {
            out.insert(out.begin(), '_');
        }

        static const std::unordered_set<std::string> cKeywords = {
            "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
            "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register",
            "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
            "union", "unsigned", "void", "volatile", "while", "_Bool", "_Complex", "_Imaginary"
        };
        if (cKeywords.find(out) != cKeywords.end()) {
            out += "_v";
        }

        return out;
    };

    auto toCType = [](const std::string& astroType) {
        if (astroType.rfind("MODULE:", 0) == 0) {
            return std::string("AstroObject*");
        }
        if (astroType == "COUNT" || astroType == "FLAG") {
            return std::string("int");
        }
        if (astroType == "REAL" || astroType == "PRECISE") {
            return std::string("double");
        }
        if (astroType == "SYMBOL") {
            return std::string("char*");
        }
        if (astroType == "VOIDSPACE") {
            return std::string("void");
        }
        return std::string("double");
    };

    auto isExpressionOp = [](const std::string& op) {
        return op == "=" || op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "**" ||
               op == "AND" || op == "OR" || op == "XOR" ||
               op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=" ||
               op == "root" || op == "flr" || op == "ceil" || op == "abs" || op == "logarithm" ||
               op == "sine" || op == "cosine" || op == "tan" || op == "asine" || op == "acosine" ||
               op == "atan" || op == "prime" ||
               op == "load" || op == "field_get" || op == "obj_new";
    };

    auto isTempName = [](const std::string& token) {
        if (token.size() < 2 || token[0] != 't') {
            return false;
        }
        for (std::size_t i = 1; i < token.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(token[i]))) {
                return false;
            }
        }
        return true;
    };

    std::unordered_map<std::string, std::string> inlineTempExpr;
    std::unordered_map<std::string, int> tempUseCount;
    for (const TACInstruction& instruction : instructions) {
        if (isTempName(instruction.arg1)) {
            ++tempUseCount[instruction.arg1];
        }
        if (isTempName(instruction.arg2)) {
            ++tempUseCount[instruction.arg2];
        }
    }

    auto toCValue = [&](const std::string& token) {
        if (token.empty()) {
            return std::string("0");
        }
        if (isStringLiteral(token) || isNumeric(token)) {
            return token;
        }

        auto tempIt = inlineTempExpr.find(token);
        if (tempIt != inlineTempExpr.end()) {
            return "(" + tempIt->second + ")";
        }

        return toIdentifier(token);
    };

    auto mapUnary = [](const std::string& op) {
        if (op == "root") return std::string("sqrt");
        if (op == "flr") return std::string("floor");
        if (op == "ceil") return std::string("ceil");
        if (op == "abs") return std::string("fabs");
        if (op == "logarithm") return std::string("log");
        if (op == "sine") return std::string("sin");
        if (op == "cosine") return std::string("cos");
        if (op == "tan") return std::string("tan");
        if (op == "asine") return std::string("asin");
        if (op == "acosine") return std::string("acos");
        if (op == "atan") return std::string("atan");
        if (op == "prime") return std::string("astro_is_prime");
        return op;
    };

    auto mapBinary = [](const std::string& op) {
        if (op == "AND") return std::string("&&");
        if (op == "OR") return std::string("||");
        if (op == "XOR") return std::string("^");
        return op;
    };

    auto expressionFromInstruction = [&](const TACInstruction& instruction) {
        if (instruction.op == "=") {
            return toCValue(instruction.arg1);
        }

        if (instruction.op == "+" || instruction.op == "-" || instruction.op == "*" || instruction.op == "/" ||
            instruction.op == "AND" || instruction.op == "OR" || instruction.op == "XOR" ||
            instruction.op == "<" || instruction.op == ">" || instruction.op == "<=" || instruction.op == ">=" ||
            instruction.op == "==" || instruction.op == "!=") {
            return toCValue(instruction.arg1) + " " + mapBinary(instruction.op) + " " + toCValue(instruction.arg2);
        }

        if (instruction.op == "%") {
            return "fmod(" + toCValue(instruction.arg1) + ", " + toCValue(instruction.arg2) + ")";
        }

        if (instruction.op == "**") {
            return "pow(" + toCValue(instruction.arg1) + ", " + toCValue(instruction.arg2) + ")";
        }

        if (instruction.op == "root" || instruction.op == "flr" || instruction.op == "ceil" || instruction.op == "abs" ||
            instruction.op == "logarithm" || instruction.op == "sine" || instruction.op == "cosine" || instruction.op == "tan" ||
            instruction.op == "asine" || instruction.op == "acosine" || instruction.op == "atan" || instruction.op == "prime") {
            return mapUnary(instruction.op) + "(" + toCValue(instruction.arg1) + ")";
        }

        if (instruction.op == "load") {
            return toIdentifier(instruction.arg1) + "[" + toCValue(instruction.arg2) + "]";
        }

        if (instruction.op == "call") {
            return toIdentifier(instruction.arg1) + "(/* " + instruction.arg2 + " args */)";
        }

        if (instruction.op == "mcall") {
            std::string methodName = instruction.arg2;
            const std::size_t sep = methodName.rfind('|');
            std::string argcText = "0";
            if (sep != std::string::npos) {
                argcText = methodName.substr(sep + 1);
                methodName = methodName.substr(0, sep);
            }
            return "astro_call_methodv(" + toIdentifier(instruction.arg1) + ", \"" + methodName + "\", " + argcText + ")";
        }

        if (instruction.op == "obj_new") {
            return "astro_new_object(\"" + instruction.arg1 + "\")";
        }

        if (instruction.op == "field_get") {
            return "astro_get_field(" + toIdentifier(instruction.arg1) + ", \"" + instruction.arg2 + "\")";
        }

        return toCValue(instruction.result.empty() ? instruction.arg1 : instruction.result);
    };

    constexpr std::size_t kMaxRenderedLines = 7000;
    constexpr std::size_t kMaxRenderedChars = 700000;
    std::size_t renderedLines = 0;
    std::size_t renderedChars = 0;
    bool truncated = false;

    auto renderLine = [&](const std::string& line, int indentLevel = 0) {
        if (truncated) {
            return;
        }
        const std::string indent(static_cast<std::size_t>(std::max(0, indentLevel)) * 4, ' ');
        const std::string finalLine = indent + line;

        if (renderedLines >= kMaxRenderedLines || renderedChars + finalLine.size() + 1 >= kMaxRenderedChars) {
            truncated = true;
            return;
        }

        ++renderedLines;
        renderedChars += finalLine.size() + 1;
        std::cout << finalLine << '\n';
    };

    auto ensureDeclared = [&](std::unordered_set<std::string>& declaredNames,
                              const std::string& rawName,
                              const std::string& typeHint,
                              int indentLevel) {
        const std::string name = toIdentifier(rawName);
        if (name.empty()) {
            return name;
        }

        if (declaredNames.find(name) == declaredNames.end()) {
            renderLine(toCType(typeHint) + " " + name + ";", indentLevel);
            declaredNames.insert(name);
        }

        return name;
    };

    std::unordered_map<std::string, std::size_t> labelToIndex;
    std::unordered_set<std::string> emittedLabelTargets;
    std::unordered_map<std::size_t, std::size_t> scopePairs;
    std::unordered_set<std::size_t> emittedScopeMarkers;
    for (std::size_t i = 0; i < instructions.size(); ++i) {
        if (instructions[i].op == "label") {
            labelToIndex[instructions[i].result] = i;
        }
    }

    {
        std::vector<std::size_t> scopeStack;
        for (std::size_t i = 0; i < instructions.size(); ++i) {
            if (instructions[i].op == "scope_begin") {
                scopeStack.push_back(i);
            } else if (instructions[i].op == "scope_end" && !scopeStack.empty()) {
                const std::size_t begin = scopeStack.back();
                scopeStack.pop_back();
                scopePairs[begin] = i;
                scopePairs[i] = begin;
            }
        }

        for (const auto& pairEntry : scopePairs) {
            const std::size_t begin = pairEntry.first;
            const std::size_t end = pairEntry.second;
            if (begin >= end) {
                continue;
            }

            bool hasDeclaration = false;
            for (std::size_t cursor = begin + 1; cursor < end; ++cursor) {
                if (instructions[cursor].op == "decl" || instructions[cursor].op == "decl_arr") {
                    hasDeclaration = true;
                    break;
                }
            }

            if (hasDeclaration) {
                emittedScopeMarkers.insert(begin);
                emittedScopeMarkers.insert(end);
            }
        }
    }

    struct FunctionRange {
        std::string name;
        std::size_t beginIndex;
        std::size_t endIndex;
        std::vector<std::string> params;
    };

    std::vector<FunctionRange> functions;
    std::vector<bool> belongsToFunction(instructions.size(), false);

    for (std::size_t i = 0; i < instructions.size(); ++i) {
        if (instructions[i].op != "func_begin") {
            continue;
        }

        std::size_t endIndex = i + 1;
        while (endIndex < instructions.size()) {
            if (instructions[endIndex].op == "func_end" && instructions[endIndex].arg1 == instructions[i].arg1) {
                break;
            }
            ++endIndex;
        }
        if (endIndex >= instructions.size()) {
            endIndex = instructions.size() - 1;
        }

        FunctionRange range;
        range.name = instructions[i].arg1;
        range.beginIndex = i;
        range.endIndex = endIndex;

        std::size_t paramCursor = i + 1;
        while (paramCursor < endIndex && instructions[paramCursor].op == "param_def") {
            range.params.push_back(instructions[paramCursor].arg1);
            ++paramCursor;
        }

        for (std::size_t mark = i; mark <= endIndex && mark < belongsToFunction.size(); ++mark) {
            belongsToFunction[mark] = true;
        }

        functions.push_back(range);
        i = endIndex;
    }

    auto trimScopeBounds = [&](std::size_t& start, std::size_t& endExclusive) {
        if (start >= endExclusive) {
            return;
        }
        if (instructions[start].op == "scope_begin" && instructions[endExclusive - 1].op == "scope_end") {
            ++start;
            --endExclusive;
        }
    };

    std::function<void(std::size_t,
                       std::size_t,
                       int,
                       std::unordered_set<std::string>&,
                       bool,
                       const std::string&,
                       const std::string&,
                       std::vector<std::string>&)> emitRange;

    auto emitInstruction = [&](std::size_t i,
                               int indentLevel,
                               std::unordered_set<std::string>& declaredNames,
                               bool functionContext,
                               const std::string& loopContinueLabel,
                               const std::string& loopBreakLabel,
                               std::vector<std::string>& pendingArgs) {
        const TACInstruction& instruction = instructions[i];

        if (instruction.op == "scope_begin") {
            if (emittedScopeMarkers.find(i) != emittedScopeMarkers.end()) {
                renderLine("{", indentLevel);
            }
            return;
        }
        if (instruction.op == "scope_end") {
            if (emittedScopeMarkers.find(i) != emittedScopeMarkers.end()) {
                renderLine("}", indentLevel);
            }
            return;
        }
        if (instruction.op == "decl") {
            const std::string name = toIdentifier(instruction.result);
            renderLine(toCType(instruction.arg1) + " " + name + ";", indentLevel);
            declaredNames.insert(name);
            return;
        }
        if (instruction.op == "decl_arr") {
            const std::string name = toIdentifier(instruction.result);
            renderLine(toCType(instruction.arg1) + " " + name + "[" + toCValue(instruction.arg2) + "];", indentLevel);
            declaredNames.insert(name);
            return;
        }
        if (instruction.op == "store") {
            renderLine(toIdentifier(instruction.arg1) + "[" + toCValue(instruction.arg2) + "] = " + toCValue(instruction.result) + ";", indentLevel);
            return;
        }
        if (instruction.op == "field_set") {
            renderLine("astro_set_field(" + toIdentifier(instruction.arg1) + ", \"" + instruction.arg2 + "\", " + toCValue(instruction.result) + ");", indentLevel);
            return;
        }
        if (instruction.op == "print") {
            renderLine("ASTRO_PRINT(" + toCValue(instruction.arg1) + ");", indentLevel);
            return;
        }
        if (instruction.op == "input") {
            const std::string target = ensureDeclared(declaredNames, instruction.result, "REAL", indentLevel);
            renderLine("ASTRO_INPUT(&" + target + ");", indentLevel);
            return;
        }
        if (instruction.op == "return") {
            if (instruction.arg1.empty()) {
                renderLine(functionContext ? "return 0.0;" : "return;", indentLevel);
            } else {
                renderLine("return " + toCValue(instruction.arg1) + ";", indentLevel);
            }
            return;
        }
        if (instruction.op == "wait") {
            renderLine("wait_ticks(" + toCValue(instruction.arg1) + ");", indentLevel);
            return;
        }
        if (instruction.op == "param") {
            pendingArgs.push_back(toCValue(instruction.arg1));
            return;
        }
        if (instruction.op == "param_def") {
            return;
        }
        if (instruction.op == "goto") {
            if (!loopContinueLabel.empty() && instruction.result == loopContinueLabel) {
                renderLine("continue;", indentLevel);
                return;
            }
            if (!loopBreakLabel.empty() && instruction.result == loopBreakLabel) {
                renderLine("break;", indentLevel);
                return;
            }
            emittedLabelTargets.insert(instruction.result);
            renderLine("goto " + toIdentifier(instruction.result) + ";", indentLevel);
            return;
        }
        if (instruction.op == "ifFalse") {
            emittedLabelTargets.insert(instruction.result);
            renderLine("if (!(" + toCValue(instruction.arg1) + ")) goto " + toIdentifier(instruction.result) + ";", indentLevel);
            return;
        }
        if (instruction.op == "label") {
            if (emittedLabelTargets.find(instruction.result) != emittedLabelTargets.end()) {
                renderLine(toIdentifier(instruction.result) + ": ;", indentLevel);
            }
            return;
        }

        if (instruction.op == "call") {
            std::string argsText;
            for (std::size_t argIndex = 0; argIndex < pendingArgs.size(); ++argIndex) {
                if (argIndex > 0) {
                    argsText += ", ";
                }
                argsText += pendingArgs[argIndex];
            }
            pendingArgs.clear();

            const std::string callExpr = toIdentifier(instruction.arg1) + "(" + argsText + ")";
            if (isTempName(instruction.result)) {
                const auto useIt = tempUseCount.find(instruction.result);
                if (useIt != tempUseCount.end() && useIt->second > 0) {
                    const std::string target = ensureDeclared(declaredNames, instruction.result, "REAL", indentLevel);
                    renderLine(target + " = " + callExpr + ";", indentLevel);
                } else {
                    renderLine(callExpr + ";", indentLevel);
                }
            } else {
                const std::string target = ensureDeclared(declaredNames, instruction.result, "REAL", indentLevel);
                renderLine(target + " = " + callExpr + ";", indentLevel);
            }
            return;
        }

        if (instruction.op == "mcall") {
            std::string methodName = instruction.arg2;
            const std::size_t sep = methodName.rfind('|');
            int argcFromMetadata = static_cast<int>(pendingArgs.size());
            if (sep != std::string::npos) {
                int parsedArgc = argcFromMetadata;
                if (tryParseInt(methodName.substr(sep + 1), parsedArgc) && parsedArgc >= 0) {
                    argcFromMetadata = parsedArgc;
                }
                methodName = methodName.substr(0, sep);
            }
            const int argc = pendingArgs.empty() ? argcFromMetadata : static_cast<int>(pendingArgs.size());
            std::string argsText;
            for (std::size_t argIndex = 0; argIndex < pendingArgs.size(); ++argIndex) {
                if (argIndex > 0) {
                    argsText += ", ";
                }
                argsText += pendingArgs[argIndex];
            }
            pendingArgs.clear();

            std::string callExpr = "astro_call_methodv(" + toIdentifier(instruction.arg1) + ", \"" + methodName + "\", " + std::to_string(argc);
            if (!argsText.empty()) {
                callExpr += ", " + argsText;
            }
            callExpr += ")";

            if (isTempName(instruction.result)) {
                const auto useIt = tempUseCount.find(instruction.result);
                if (useIt != tempUseCount.end() && useIt->second > 0) {
                    const std::string target = ensureDeclared(declaredNames, instruction.result, "REAL", indentLevel);
                    renderLine(target + " = " + callExpr + ";", indentLevel);
                } else {
                    renderLine(callExpr + ";", indentLevel);
                }
            } else {
                const std::string target = ensureDeclared(declaredNames, instruction.result, "REAL", indentLevel);
                renderLine(target + " = " + callExpr + ";", indentLevel);
            }
            return;
        }

        if (isExpressionOp(instruction.op)) {
            const std::string expr = expressionFromInstruction(instruction);
            if (isTempName(instruction.result)) {
                inlineTempExpr[instruction.result] = expr;
            } else {
                const std::string target = ensureDeclared(declaredNames, instruction.result, "REAL", indentLevel);
                renderLine(target + " = " + expr + ";", indentLevel);
            }
            return;
        }

        renderLine("/* unsupported TAC op: " + instruction.op + " */", indentLevel);
    };

    emitRange = [&](std::size_t start,
                    std::size_t endExclusive,
                    int indentLevel,
                    std::unordered_set<std::string>& declaredNames,
                    bool functionContext,
                    const std::string& loopContinueLabel,
                    const std::string& loopBreakLabel,
                    std::vector<std::string>& pendingArgs) {
        std::size_t i = start;
        while (i < endExclusive && !truncated) {
            if (i < belongsToFunction.size() && belongsToFunction[i] && !functionContext) {
                ++i;
                continue;
            }

            const TACInstruction& instruction = instructions[i];

            if (instruction.op == "func_begin" || instruction.op == "func_end") {
                ++i;
                continue;
            }

            if (i + 1 < endExclusive && instructions[i + 1].op == "ifFalse" &&
                instructions[i].result == instructions[i + 1].arg1 &&
                isExpressionOp(instructions[i].op)) {
                ++i;
                continue;
            }

            if (instruction.op == "label") {
                std::size_t ifIndex = i + 1;
                bool hasCondInstruction = false;

                if (ifIndex < endExclusive && instructions[ifIndex].op != "ifFalse" &&
                    ifIndex + 1 < endExclusive && instructions[ifIndex + 1].op == "ifFalse" &&
                    instructions[ifIndex].result == instructions[ifIndex + 1].arg1) {
                    hasCondInstruction = true;
                    ++ifIndex;
                }

                if (ifIndex < endExclusive && instructions[ifIndex].op == "ifFalse") {
                    auto endLabelIt = labelToIndex.find(instructions[ifIndex].result);
                    if (endLabelIt != labelToIndex.end()) {
                        const std::size_t endLabelIndex = endLabelIt->second;
                        if (endLabelIndex > ifIndex && endLabelIndex < endExclusive && endLabelIndex > 0) {
                            const TACInstruction& jumpBack = instructions[endLabelIndex - 1];
                            if (jumpBack.op == "goto" && jumpBack.result == instruction.result) {
                                std::string conditionText = hasCondInstruction
                                    ? expressionFromInstruction(instructions[ifIndex - 1])
                                    : toCValue(instructions[ifIndex].arg1);

                                renderLine("while (" + conditionText + ") {", indentLevel);

                                std::size_t bodyStart = ifIndex + 1;
                                std::size_t bodyEnd = endLabelIndex - 1;
                                trimScopeBounds(bodyStart, bodyEnd);
                                emitRange(bodyStart,
                                          bodyEnd,
                                          indentLevel + 1,
                                          declaredNames,
                                          functionContext,
                                          instruction.result,
                                          instructions[ifIndex].result,
                                          pendingArgs);

                                renderLine("}", indentLevel);
                                i = endLabelIndex;
                                continue;
                            }
                        }
                    }
                }
            }

            if (instruction.op == "ifFalse") {
                auto falseLabelIt = labelToIndex.find(instruction.result);
                if (falseLabelIt != labelToIndex.end()) {
                    const std::size_t falseLabelIndex = falseLabelIt->second;
                    if (falseLabelIndex > i && falseLabelIndex < endExclusive) {
                        const std::string conditionText =
                            (i > start && instructions[i - 1].result == instruction.arg1 && isExpressionOp(instructions[i - 1].op))
                                ? expressionFromInstruction(instructions[i - 1])
                                : toCValue(instruction.arg1);

                        if (falseLabelIndex > 0 && instructions[falseLabelIndex - 1].op == "goto") {
                            auto endLabelIt = labelToIndex.find(instructions[falseLabelIndex - 1].result);
                            if (endLabelIt != labelToIndex.end()) {
                                const std::size_t endLabelIndex = endLabelIt->second;
                                if (endLabelIndex > falseLabelIndex && endLabelIndex <= endExclusive) {
                                    renderLine("if (" + conditionText + ") {", indentLevel);

                                    std::size_t thenStart = i + 1;
                                    std::size_t thenEnd = falseLabelIndex - 1;
                                    while (thenEnd > thenStart && instructions[thenEnd - 1].op == "goto" &&
                                           instructions[thenEnd - 1].result == instructions[falseLabelIndex - 1].result) {
                                        --thenEnd;
                                    }
                                    trimScopeBounds(thenStart, thenEnd);
                                    emitRange(thenStart,
                                              thenEnd,
                                              indentLevel + 1,
                                              declaredNames,
                                              functionContext,
                                              loopContinueLabel,
                                              loopBreakLabel,
                                              pendingArgs);

                                    renderLine("}", indentLevel);

                                    if (falseLabelIndex + 1 < endLabelIndex) {
                                        renderLine("else {", indentLevel);

                                        std::size_t elseStart = falseLabelIndex + 1;
                                        std::size_t elseEnd = endLabelIndex;
                                        trimScopeBounds(elseStart, elseEnd);
                                        emitRange(elseStart,
                                                  elseEnd,
                                                  indentLevel + 1,
                                                  declaredNames,
                                                  functionContext,
                                                  loopContinueLabel,
                                                  loopBreakLabel,
                                                  pendingArgs);

                                        renderLine("}", indentLevel);
                                    }

                                    i = endLabelIndex;
                                    continue;
                                }
                            }
                        }

                        renderLine("if (" + conditionText + ") {", indentLevel);

                        std::size_t thenStart = i + 1;
                        std::size_t thenEnd = falseLabelIndex;
                        trimScopeBounds(thenStart, thenEnd);
                        emitRange(thenStart,
                                  thenEnd,
                                  indentLevel + 1,
                                  declaredNames,
                                  functionContext,
                                  loopContinueLabel,
                                  loopBreakLabel,
                                  pendingArgs);

                        renderLine("}", indentLevel);
                        i = falseLabelIndex;
                        continue;
                    }
                }
            }

            if (instruction.op == "call" && i + 1 < endExclusive &&
                instructions[i + 1].op == "=" &&
                instructions[i + 1].arg1 == instruction.result) {
                const TACInstruction& assignInstruction = instructions[i + 1];
                const std::string targetName =
                    ensureDeclared(declaredNames, assignInstruction.result, "REAL", indentLevel);

                std::string argsText;
                for (std::size_t argIndex = 0; argIndex < pendingArgs.size(); ++argIndex) {
                    if (argIndex > 0) {
                        argsText += ", ";
                    }
                    argsText += pendingArgs[argIndex];
                }
                pendingArgs.clear();

                renderLine(targetName + " = " + toIdentifier(instruction.arg1) + "(" + argsText + ");", indentLevel);
                i += 2;
                continue;
            }

            if (instruction.op == "mcall" && i + 1 < endExclusive &&
                instructions[i + 1].op == "=" &&
                instructions[i + 1].arg1 == instruction.result) {
                const TACInstruction& assignInstruction = instructions[i + 1];
                const std::string targetName =
                    ensureDeclared(declaredNames, assignInstruction.result, "REAL", indentLevel);

                std::string methodName = instruction.arg2;
                const std::size_t sep = methodName.rfind('|');
                int argcFromMetadata = static_cast<int>(pendingArgs.size());
                if (sep != std::string::npos) {
                    int parsedArgc = argcFromMetadata;
                    if (tryParseInt(methodName.substr(sep + 1), parsedArgc) && parsedArgc >= 0) {
                        argcFromMetadata = parsedArgc;
                    }
                    methodName = methodName.substr(0, sep);
                }

                const int argc = pendingArgs.empty() ? argcFromMetadata : static_cast<int>(pendingArgs.size());
                std::string argsText;
                for (std::size_t argIndex = 0; argIndex < pendingArgs.size(); ++argIndex) {
                    if (argIndex > 0) {
                        argsText += ", ";
                    }
                    argsText += pendingArgs[argIndex];
                }
                pendingArgs.clear();

                std::string line = targetName + " = astro_call_methodv(" + toIdentifier(instruction.arg1) + ", \"" + methodName + "\", " + std::to_string(argc);
                if (!argsText.empty()) {
                    line += ", " + argsText;
                }
                line += ");";
                renderLine(line, indentLevel);
                i += 2;
                continue;
            }

            if (i + 1 < endExclusive && isExpressionOp(instruction.op) &&
                instruction.op != "call" && instruction.op != "mcall" &&
                instructions[i + 1].op == "=" &&
                instructions[i + 1].arg1 == instruction.result) {
                const TACInstruction& assignInstruction = instructions[i + 1];
                const std::string targetName =
                    ensureDeclared(declaredNames, assignInstruction.result, "REAL", indentLevel);
                renderLine(targetName + " = " + expressionFromInstruction(instruction) + ";", indentLevel);
                i += 2;
                continue;
            }

            emitInstruction(i,
                            indentLevel,
                            declaredNames,
                            functionContext,
                            loopContinueLabel,
                            loopBreakLabel,
                            pendingArgs);
            ++i;
        }
    };

    renderLine("/* Structured C translation generated from optimized TAC. */", 0);
    renderLine("#include <math.h>", 0);
    renderLine("#include <stdio.h>", 0);
    renderLine("#include <stdarg.h>", 0);
    renderLine("", 0);
    renderLine("typedef struct AstroObject AstroObject;", 0);
    renderLine("struct AstroObject { int _placeholder; };", 0);
    renderLine("", 0);
    renderLine("static AstroObject* astro_new_object(const char* class_name) { (void)class_name; return NULL; }", 0);
    renderLine("static double astro_get_field(AstroObject* obj, const char* field) { (void)obj; (void)field; return 0.0; }", 0);
    renderLine("static void astro_set_field(AstroObject* obj, const char* field, double value) { (void)obj; (void)field; (void)value; }", 0);
    renderLine("static double astro_call_methodv(AstroObject* obj, const char* method, int argc, ...) {", 0);
    renderLine("(void)obj; (void)method;", 1);
    renderLine("va_list args;", 1);
    renderLine("va_start(args, argc);", 1);
    renderLine("for (int i = 0; i < argc; ++i) { (void)va_arg(args, double); }", 1);
    renderLine("va_end(args);", 1);
    renderLine("return 0.0;", 1);
    renderLine("}", 0);
    renderLine("static int astro_is_prime(double n) {", 0);
    renderLine("if (n < 2.0 || floor(n) != n) return 0;", 1);
    renderLine("for (int i = 2; (double)i * (double)i <= n; ++i) {", 1);
    renderLine("if (((int)n) % i == 0) return 0;", 2);
    renderLine("}", 1);
    renderLine("return 1;", 1);
    renderLine("}", 0);
    renderLine("static void wait_ticks(double ticks) { (void)ticks; }", 0);
    renderLine("", 0);
    renderLine("#define ASTRO_PRINT(x) _Generic((x), char*: printf(\"%s\\n\", (x)), const char*: printf(\"%s\\n\", (x)), default: printf(\"%g\\n\", (double)(x)))", 0);
    renderLine("#define ASTRO_INPUT(x) scanf(\"%lf\", (x))", 0);
    renderLine("", 0);

    for (const FunctionRange& function : functions) {
        if (truncated) {
            break;
        }

        inlineTempExpr.clear();

        std::string signature = "static double " + toIdentifier(function.name) + "(";
        if (function.params.empty()) {
            signature += "void";
        } else {
            for (std::size_t p = 0; p < function.params.size(); ++p) {
                if (p > 0) {
                    signature += ", ";
                }
                signature += (function.params[p] == "this" ? "AstroObject* " : "double ") + toIdentifier(function.params[p]);
            }
        }
        signature += ")";

        renderLine(signature, 0);
        renderLine("{", 0);

        std::unordered_set<std::string> declaredInFunction;
        for (const std::string& param : function.params) {
            declaredInFunction.insert(toIdentifier(param));
        }

        std::size_t bodyStart = function.beginIndex + 1 + function.params.size();
        std::size_t bodyEnd = function.endIndex;

        trimScopeBounds(bodyStart, bodyEnd);
        while (bodyEnd > bodyStart && instructions[bodyEnd - 1].op == "return" && instructions[bodyEnd - 1].arg1.empty()) {
            --bodyEnd;
        }
        trimScopeBounds(bodyStart, bodyEnd);

        std::vector<std::string> pendingArgs;
        emitRange(bodyStart, bodyEnd, 1, declaredInFunction, true, "", "", pendingArgs);

        bool hasExplicitReturn = false;
        for (std::size_t cursor = bodyStart; cursor < bodyEnd; ++cursor) {
            if (instructions[cursor].op == "return") {
                hasExplicitReturn = true;
                break;
            }
        }
        if (!hasExplicitReturn) {
            renderLine("return 0.0;", 1);
        }
        renderLine("}", 0);
        renderLine("", 0);
    }

    renderLine("int main(void)", 0);
    renderLine("{", 0);
    inlineTempExpr.clear();
    std::unordered_set<std::string> declaredInMain;
    std::size_t mainStart = 0;
    std::size_t mainEnd = instructions.size();
    trimScopeBounds(mainStart, mainEnd);
    std::vector<std::string> pendingMainArgs;
    emitRange(mainStart, mainEnd, 1, declaredInMain, false, "", "", pendingMainArgs);
    renderLine("return 0;", 1);
    renderLine("}", 0);

    if (truncated) {
        std::cout << "/* C translation truncated to keep output manageable in browser. */\n";
    }

    std::cout << "-----------------------------\n";
}

void TACGenerator::execute() {
    buildExecutionMetadata();

    frames.clear();
    frames.push_back(Frame{});
    arrays.clear();
    objectInstances.clear();
    parameterStack.clear();
    nextObjectId = 1;

    struct CallFrame {
        int returnPc;
        std::string returnTemp;
    };

    std::vector<CallFrame> callStack;
    int pc = 0;
    constexpr int kMaxCallDepth = 1024;
    constexpr int kMaxArrayElements = 1000000;

    auto runtimeError = [](const std::string& message) {
        std::cerr << "RUNTIME ERROR: " << message << std::endl;
    };

    while (pc >= 0 && pc < static_cast<int>(instructions.size())) {
        const TACInstruction& instruction = instructions[pc];

        if (instruction.op == "label") {
            ++pc;
            continue;
        }

        if (instruction.op == "goto") {
            auto labelIt = labelToIndex.find(instruction.result);
            if (labelIt == labelToIndex.end()) {
                runtimeError("invalid jump target '" + instruction.result + "'");
                break;
            }
            pc = labelIt->second;
            continue;
        }

        if (instruction.op == "ifFalse") {
            RuntimeValue condition = getValue(instruction.arg1);
            if (!valueToBool(condition)) {
                auto labelIt = labelToIndex.find(instruction.result);
                if (labelIt == labelToIndex.end()) {
                    runtimeError("invalid conditional jump target '" + instruction.result + "'");
                    break;
                }
                pc = labelIt->second;
            } else {
                ++pc;
            }
            continue;
        }

        if (instruction.op == "scope_begin") {
            frames.push_back(Frame{});
            ++pc;
            continue;
        }

        if (instruction.op == "scope_end") {
            if (frames.size() > 1) {
                frames.pop_back();
            } else {
                runtimeError("attempted to exit root scope");
            }
            ++pc;
            continue;
        }

        if (instruction.op == "decl") {
            if (frames.empty()) {
                frames.push_back(Frame{});
            }
            frames.back().variables[instruction.result] = 0.0;
            ++pc;
            continue;
        }

        if (instruction.op == "decl_arr") {
            int size = static_cast<int>(valueToNumber(getValue(instruction.arg2)));
            if (size < 0) {
                runtimeError("negative array size for '" + instruction.result + "'");
                size = 0;
            }
            if (size > kMaxArrayElements) {
                runtimeError("array '" + instruction.result + "' exceeds maximum allowed size");
                size = kMaxArrayElements;
            }
            try {
                arrays[instruction.result] = std::vector<RuntimeValue>(size, 0.0);
            } catch (const std::bad_alloc&) {
                runtimeError("out of memory while allocating array '" + instruction.result + "'");
                break;
            }
            ++pc;
            continue;
        }

        if (instruction.op == "obj_new") {
            if (classDescriptors.find(instruction.arg1) == classDescriptors.end()) {
                runtimeError("instantiation of unknown class '" + instruction.arg1 + "'");
                setValue(instruction.result, 0.0);
            } else {
                setValue(instruction.result, createObject(instruction.arg1));
            }
            ++pc;
            continue;
        }

        if (instruction.op == "=") {
            setValue(instruction.result, getValue(instruction.arg1));
            ++pc;
            continue;
        }

        if (instruction.op == "print") {
            std::cout << "PRINT: " << valueToString(getValue(instruction.arg1)) << std::endl;
            ++pc;
            continue;
        }

        if (instruction.op == "input") {
            std::string line;
            std::getline(std::cin, line);
            if (line.empty()) {
                setValue(instruction.result, 0.0);
            } else if (isNumeric(line)) {
                setValue(instruction.result, toNumber(line));
            } else {
                setValue(instruction.result, line);
            }
            ++pc;
            continue;
        }

        if (instruction.op == "store") {
            int index = static_cast<int>(valueToNumber(getValue(instruction.arg2)));
            auto arrayIt = arrays.find(instruction.arg1);
            if (arrayIt == arrays.end()) {
                runtimeError("bad reference to undeclared array '" + instruction.arg1 + "'");
                ++pc;
                continue;
            }
            if (index < 0 || index >= static_cast<int>(arrayIt->second.size())) {
                runtimeError("array index out of bounds for '" + instruction.arg1 + "' at index " + std::to_string(index));
                ++pc;
                continue;
            }
            arrayIt->second[index] = getValue(instruction.result);
            ++pc;
            continue;
        }

        if (instruction.op == "load") {
            int index = static_cast<int>(valueToNumber(getValue(instruction.arg2)));
            auto arrayIt = arrays.find(instruction.arg1);
            if (arrayIt == arrays.end()) {
                runtimeError("bad reference to undeclared array '" + instruction.arg1 + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }
            if (index < 0 || index >= static_cast<int>(arrayIt->second.size())) {
                runtimeError("array index out of bounds for '" + instruction.arg1 + "' at index " + std::to_string(index));
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }
            setValue(instruction.result, arrayIt->second[index]);
            ++pc;
            continue;
        }

        if (instruction.op == "field_set") {
            RuntimeValue objectValue = getValue(instruction.arg1);
            if (!isObjectRefValue(objectValue)) {
                runtimeError("field write on non-object value '" + instruction.arg1 + "'");
                ++pc;
                continue;
            }

            ObjectRef ref = toObjectRef(objectValue);
            auto objectIt = objectInstances.find(ref.id);
            if (objectIt == objectInstances.end()) {
                runtimeError("invalid object reference during field write");
                ++pc;
                continue;
            }

            if (!hasField(objectIt->second.className, instruction.arg2)) {
                runtimeError("unknown field '" + instruction.arg2 + "' for class '" + objectIt->second.className + "'");
                ++pc;
                continue;
            }

            objectIt->second.fields[instruction.arg2] = getValue(instruction.result);
            ++pc;
            continue;
        }

        if (instruction.op == "field_get") {
            RuntimeValue objectValue = getValue(instruction.arg1);
            if (!isObjectRefValue(objectValue)) {
                runtimeError("field read on non-object value '" + instruction.arg1 + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            ObjectRef ref = toObjectRef(objectValue);
            auto objectIt = objectInstances.find(ref.id);
            if (objectIt == objectInstances.end()) {
                runtimeError("invalid object reference during field read");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            if (!hasField(objectIt->second.className, instruction.arg2)) {
                runtimeError("unknown field '" + instruction.arg2 + "' for class '" + objectIt->second.className + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            auto valueIt = objectIt->second.fields.find(instruction.arg2);
            if (valueIt != objectIt->second.fields.end()) {
                setValue(instruction.result, valueIt->second);
            } else {
                setValue(instruction.result, getDefaultFieldValue(objectIt->second.className, instruction.arg2));
            }
            ++pc;
            continue;
        }

        if (instruction.op == "param") {
            parameterStack.push_back(getValue(instruction.arg1));
            ++pc;
            continue;
        }

        if (instruction.op == "func_begin") {
            if (callStack.empty()) {
                auto functionIt = functionBounds.find(instruction.arg1);
                if (functionIt != functionBounds.end()) {
                    pc = functionIt->second.second + 1;
                    continue;
                }
            }
            ++pc;
            continue;
        }

        if (instruction.op == "call") {
            auto functionIt = functionBounds.find(instruction.arg1);
            if (functionIt == functionBounds.end()) {
                runtimeError("undefined function '" + instruction.arg1 + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            if (static_cast<int>(callStack.size()) >= kMaxCallDepth) {
                runtimeError("stack overflow while calling '" + instruction.arg1 + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            int argc = 0;
            if (!tryParseInt(instruction.arg2, argc) || argc < 0) {
                runtimeError("invalid function argument count '" + instruction.arg2 + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            std::vector<RuntimeValue> args;
            for (int i = 0; i < argc && !parameterStack.empty(); ++i) {
                args.push_back(parameterStack.back());
                parameterStack.pop_back();
            }
            std::reverse(args.begin(), args.end());

            callStack.push_back({pc + 1, instruction.result});
            frames.push_back(Frame{});

            int cursor = functionIt->second.first + 1;
            int argIndex = 0;
            while (cursor < functionIt->second.second && instructions[cursor].op == "param_def") {
                if (argIndex < static_cast<int>(args.size())) {
                    frames.back().variables[instructions[cursor].arg1] = args[argIndex++];
                } else {
                    frames.back().variables[instructions[cursor].arg1] = 0.0;
                }
                ++cursor;
            }

            pc = cursor;
            continue;
        }

        if (instruction.op == "mcall") {
            RuntimeValue objectValue = getValue(instruction.arg1);
            if (!isObjectRefValue(objectValue)) {
                runtimeError("method call on non-object value '" + instruction.arg1 + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            ObjectRef ref = toObjectRef(objectValue);
            auto objectIt = objectInstances.find(ref.id);
            if (objectIt == objectInstances.end()) {
                runtimeError("invalid object reference during method call");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            std::size_t separator = instruction.arg2.rfind('|');
            if (separator == std::string::npos) {
                runtimeError("invalid method call metadata '" + instruction.arg2 + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            std::string methodName = instruction.arg2.substr(0, separator);
            std::string argcText = instruction.arg2.substr(separator + 1);

            int argc = 0;
            if (!tryParseInt(argcText, argc) || argc < 0) {
                runtimeError("invalid method argument count '" + argcText + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            std::vector<RuntimeValue> args;
            for (int i = 0; i < argc && !parameterStack.empty(); ++i) {
                args.push_back(parameterStack.back());
                parameterStack.pop_back();
            }
            std::reverse(args.begin(), args.end());

            std::string resolvedFunction;
            if (methodName.rfind("super:", 0) == 0) {
                std::string payload = methodName.substr(std::string("super:").size());
                std::size_t split = payload.find(':');
                if (split == std::string::npos) {
                    runtimeError("invalid super method descriptor '" + methodName + "'");
                    setValue(instruction.result, 0.0);
                    ++pc;
                    continue;
                }

                std::string baseClass = payload.substr(0, split);
                std::string baseMethod = payload.substr(split + 1);
                resolvedFunction = resolveMethodTargetFromBase(baseClass, baseMethod);
            } else {
                resolvedFunction = resolveMethodTarget(objectIt->second.className, methodName);
            }

            if (resolvedFunction.empty()) {
                runtimeError("undefined method target for '" + methodName + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            auto functionIt = functionBounds.find(resolvedFunction);
            if (functionIt == functionBounds.end()) {
                runtimeError("resolved method function not found: '" + resolvedFunction + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            if (static_cast<int>(callStack.size()) >= kMaxCallDepth) {
                runtimeError("stack overflow while calling method '" + methodName + "'");
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            callStack.push_back({pc + 1, instruction.result});
            frames.push_back(Frame{});

            int cursor = functionIt->second.first + 1;
            int argIndex = 0;
            while (cursor < functionIt->second.second && instructions[cursor].op == "param_def") {
                const std::string& paramName = instructions[cursor].arg1;
                if (paramName == "this") {
                    frames.back().variables[paramName] = ref;
                } else if (argIndex < static_cast<int>(args.size())) {
                    frames.back().variables[paramName] = args[argIndex++];
                } else {
                    frames.back().variables[paramName] = 0.0;
                }
                ++cursor;
            }

            pc = cursor;
            continue;
        }

        if (instruction.op == "return") {
            RuntimeValue returnValue = instruction.arg1.empty() ? RuntimeValue(0.0) : getValue(instruction.arg1);
            if (!callStack.empty()) {
                CallFrame call = callStack.back();
                callStack.pop_back();
                if (frames.size() <= 1) {
                    runtimeError("invalid return frame state");
                    break;
                }
                frames.pop_back();
                setValue(call.returnTemp, returnValue);
                pc = call.returnPc;
                continue;
            }

            ++pc;
            continue;
        }

        if (instruction.op == "func_end") {
            if (!callStack.empty()) {
                CallFrame call = callStack.back();
                callStack.pop_back();
                if (frames.size() <= 1) {
                    runtimeError("invalid function frame state");
                    break;
                }
                frames.pop_back();
                setValue(call.returnTemp, 0.0);
                pc = call.returnPc;
                continue;
            }

            ++pc;
            continue;
        }

        if (instruction.op == "param_def") {
            ++pc;
            continue;
        }

        if (instruction.op == "wait") {
            ++pc;
            continue;
        }

        if (instruction.op == "root" || instruction.op == "flr" || instruction.op == "ceil" || instruction.op == "abs" ||
            instruction.op == "logarithm" || instruction.op == "sine" || instruction.op == "cosine" || instruction.op == "tan" ||
            instruction.op == "asine" || instruction.op == "acosine" || instruction.op == "atan" || instruction.op == "prime") {
            double value = valueToNumber(getValue(instruction.arg1));
            double computed = 0.0;

            if (instruction.op == "root") {
                if (value < 0.0) {
                    runtimeError("root() domain error for value " + toString(value));
                    setValue(instruction.result, 0.0);
                    ++pc;
                    continue;
                }
                computed = std::sqrt(value);
            } else if (instruction.op == "flr") {
                computed = std::floor(value);
            } else if (instruction.op == "ceil") {
                computed = std::ceil(value);
            } else if (instruction.op == "abs") {
                computed = std::fabs(value);
            } else if (instruction.op == "logarithm") {
                if (value <= 0.0) {
                    runtimeError("logarithm() domain error for value " + toString(value));
                    setValue(instruction.result, 0.0);
                    ++pc;
                    continue;
                }
                computed = std::log(value);
            } else if (instruction.op == "sine") {
                computed = std::sin(value);
            } else if (instruction.op == "cosine") {
                computed = std::cos(value);
            } else if (instruction.op == "tan") {
                computed = std::tan(value);
            } else if (instruction.op == "asine") {
                if (value < -1.0 || value > 1.0) {
                    runtimeError("asine() domain error for value " + toString(value));
                    setValue(instruction.result, 0.0);
                    ++pc;
                    continue;
                }
                computed = std::asin(value);
            } else if (instruction.op == "acosine") {
                if (value < -1.0 || value > 1.0) {
                    runtimeError("acosine() domain error for value " + toString(value));
                    setValue(instruction.result, 0.0);
                    ++pc;
                    continue;
                }
                computed = std::acos(value);
            } else if (instruction.op == "atan") {
                computed = std::atan(value);
            } else if (instruction.op == "prime") {
                long long n = static_cast<long long>(std::llround(value));
                if (n < 2) {
                    computed = 0.0;
                } else {
                    bool isPrime = true;
                    for (long long i = 2; i * i <= n; ++i) {
                        if (n % i == 0) {
                            isPrime = false;
                            break;
                        }
                    }
                    computed = isPrime ? 1.0 : 0.0;
                }
            }

            setValue(instruction.result, computed);
            ++pc;
            continue;
        }

        if (instruction.op == "+" || instruction.op == "-" || instruction.op == "*" || instruction.op == "/" ||
            instruction.op == "%" || instruction.op == "**" ||
            instruction.op == "AND" || instruction.op == "OR" || instruction.op == "XOR" ||
            instruction.op == "<" || instruction.op == ">" || instruction.op == "<=" || instruction.op == ">=" ||
            instruction.op == "==" || instruction.op == "!=") {
            setValue(instruction.result, evalBinary(instruction));
            ++pc;
            continue;
        }

        ++pc;
    }
}
