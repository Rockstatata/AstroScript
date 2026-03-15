#include "tac.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

TACGenerator::TACGenerator() : tempCounter(0), labelCounter(0) {}

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

void TACGenerator::emitAssign(const std::string& target, const std::string& source) {
    emit("=", source, "", target);
}

void TACGenerator::emitDeclare(const std::string& type, const std::string& name) {
    emit("decl", type, "", name);
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

void TACGenerator::emitParamDef(const std::string& name) {
    emit("param_def", name, "", "");
}

void TACGenerator::emitCallTarget(const std::string& functionName, const std::string& label) {
    functionEntryLabel[functionName] = label;
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
    return toString(std::get<double>(value));
}

double TACGenerator::valueToNumber(const RuntimeValue& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
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
    return !std::get<std::string>(value).empty();
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
    if (instruction.op == "/") return (std::fabs(r) < 1e-12) ? 0.0 : l / r;
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
        else if (instruction.op == "/") folded = (right == 0.0) ? 0.0 : left / right;
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
        } else if (instruction.op == "=") {
            std::cout << instruction.result << " = " << instruction.arg1;
        } else {
            std::cout << instruction.result << " = " << instruction.arg1 << " " << instruction.op << " " << instruction.arg2;
        }

        std::cout << '\n';
    }

    std::cout << "-----------------------------\n";
}

void TACGenerator::execute() {
    buildExecutionMetadata();

    frames.clear();
    frames.push_back(Frame{});
    arrays.clear();

    struct CallFrame {
        int returnPc;
        std::string returnTemp;
    };

    std::vector<CallFrame> callStack;
    int pc = 0;

    while (pc >= 0 && pc < static_cast<int>(instructions.size())) {
        const TACInstruction& instruction = instructions[pc];

        if (instruction.op == "label") {
            ++pc;
            continue;
        }

        if (instruction.op == "goto") {
            pc = labelToIndex[instruction.result];
            continue;
        }

        if (instruction.op == "ifFalse") {
            RuntimeValue condition = getValue(instruction.arg1);
            if (!valueToBool(condition)) {
                pc = labelToIndex[instruction.result];
            } else {
                ++pc;
            }
            continue;
        }

        if (instruction.op == "decl") {
            setValue(instruction.result, 0.0);
            ++pc;
            continue;
        }

        if (instruction.op == "decl_arr") {
            int size = static_cast<int>(valueToNumber(getValue(instruction.arg2)));
            if (size < 0) {
                size = 0;
            }
            arrays[instruction.result] = std::vector<RuntimeValue>(size, 0.0);
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
            if (arrayIt != arrays.end() && index >= 0 && index < static_cast<int>(arrayIt->second.size())) {
                arrayIt->second[index] = getValue(instruction.result);
            }
            ++pc;
            continue;
        }

        if (instruction.op == "load") {
            int index = static_cast<int>(valueToNumber(getValue(instruction.arg2)));
            auto arrayIt = arrays.find(instruction.arg1);
            if (arrayIt != arrays.end() && index >= 0 && index < static_cast<int>(arrayIt->second.size())) {
                setValue(instruction.result, arrayIt->second[index]);
            } else {
                setValue(instruction.result, 0.0);
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
                setValue(instruction.result, 0.0);
                ++pc;
                continue;
            }

            int argc = std::stoi(instruction.arg2);
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

        if (instruction.op == "return") {
            RuntimeValue returnValue = instruction.arg1.empty() ? RuntimeValue(0.0) : getValue(instruction.arg1);
            if (!callStack.empty()) {
                CallFrame call = callStack.back();
                callStack.pop_back();
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

        if (instruction.op == "+" || instruction.op == "-" || instruction.op == "*" || instruction.op == "/" ||
            instruction.op == "<" || instruction.op == ">" || instruction.op == "<=" || instruction.op == ">=" ||
            instruction.op == "==" || instruction.op == "!=") {
            setValue(instruction.result, evalBinary(instruction));
            ++pc;
            continue;
        }

        ++pc;
    }
}
