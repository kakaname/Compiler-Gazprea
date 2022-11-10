//
// Created by rajan on 11/08/22.
//


#pragma once

#include <string>
#include <sstream>

using namespace std;

// TODO organize files
// FIXME consolidate

// --- SYNTAX ERRORS ---

class SyntaxError : public std::exception {
private:
    std::string msg;
public:
    SyntaxError(std::string msg) : msg(msg) {}

    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

// --- COMPILE TIME ERRORS ---

class CompilerError : public std::exception {
private:
    string message;
public:
    CompilerError(string title, size_t line, string message) {
        stringstream ss;
        ss << title << " Error [line " << line << "] : " << message << "\n";
        this->message = ss.str();
    }

    virtual const char *what() const throw() {
        return message.c_str();
    }
};

class CastError : public CompilerError {
public:
    CastError(size_t line, string from, string to) : CompilerError(
            "Cast", line,
            "Cannot cast " + from + " to " + to) {}
};

class OutputTypeError : public CompilerError {
public:
    OutputTypeError(size_t line, string typeName) : CompilerError(
            "Output Type", line,
            "Cannot output type " + typeName) {}
};

class InputTypeError: public CompilerError {
public:
    InputTypeError(size_t line, string typeName) : CompilerError(
            "Input Type", line,
            "Cannot input to type " + typeName) {}
};

class ScalarCastError: public CompilerError {
public:
    ScalarCastError(size_t line, string from, string to) : CompilerError(
            "Type Cast", line,
            "Cannot cast type " + from + " to " + to) {}
};

class TupleCastError: public CompilerError {
public:
    TupleCastError(size_t line, string from, string to) : CompilerError(
            "Type Cast", line,
            "Cannot cast tuple " + from + " to " + to) {}
};

class TupleAccessError: public CompilerError {
public:
    TupleAccessError(size_t line, string tupleName, string index) : CompilerError(
            "Tuple Access", line,
            "Cannot access tuple " + tupleName + " at item " + index) {}
};

class TupleSizeError: public CompilerError {
public:
    TupleSizeError(size_t line, size_t expected, size_t actual) : CompilerError(
            "Tuple Size", line,
            "Expected tuple of size " + to_string(expected) + " but got " + to_string(actual)) {}
};

class ScalarPromotionError : public CompilerError {
public:
    ScalarPromotionError(size_t line, string from, string to) : CompilerError(
            "Type Promotion", line,
            "Cannot automatically promote " + from + " to " + to) {}
};

class NullTypeInferenceError : public CompilerError {
public:
    NullTypeInferenceError(size_t line) : CompilerError(
            "Null Type Inference", line,
            "Cannot infer the type of null expression"
            ) {}
};

class IdentityTypeInferenceError: public CompilerError {
public:
    IdentityTypeInferenceError(size_t line) : CompilerError(
            "Identity Type Inference", line,
            "Cannot infer the type of identity expression"
            ) {}
};

class NullCastError: public CompilerError {
public:
    NullCastError(size_t line) : CompilerError(
            "Null Cast", line,
            "Cannot cast null to any value"
            ) {}
};

class IdentityCastError: public CompilerError {
public:
    IdentityCastError(size_t line) : CompilerError(
            "Identity Cast", line,
            "Cannot cast identity to any value"
            ) {}
};

class BadIdentifierNameError: public CompilerError {
public:
    BadIdentifierNameError(size_t line, string name) : CompilerError(
            "Bad Identifier Name", line,
            "Identifier name " + name + " is not valid"
            ) {}
};

class InvalidDeclarationError: public CompilerError {
public:
    InvalidDeclarationError(size_t line, string name) : CompilerError(
            "Invalid Declaration", line,
            "Illegal declaration of" + name
            ) {}
};

class RedeclarationError: public CompilerError {
public:
    RedeclarationError(size_t line, string name) : CompilerError(
            "Redeclaration", line,
            "Identifier " + name + " has already been declared"
            ) {}
};

class UndeclaredIdentifierError: public CompilerError {
public:
    UndeclaredIdentifierError(size_t line, string name) : CompilerError(
            "Undeclared Identifier", line,
            "Identifier " + name + " has not been declared"
            ) {}
};

class InvalidAssignmentError: public CompilerError {
public:
    InvalidAssignmentError(size_t line, string name) : CompilerError(
            "Invalid Assignment", line,
            "Cannot assign to " + name
            ) {}
};

class InvalidAssignmentTypeError: public CompilerError {
public:
    InvalidAssignmentTypeError(size_t line, string name, string expected, string actual) : CompilerError(
            "Invalid Assignment Type", line,
            "Cannot assign " + actual + " to " + name + " of type " + expected
            ) {}
};

class ConstantAssignmentError: public CompilerError {
public:
    ConstantAssignmentError(size_t line, string name) : CompilerError(
            "Constant Assignment", line,
            "Cannot assign to constant " + name
            ) {}
};

class GlobalConstantError: public CompilerError {
public:
    GlobalConstantError(size_t line, string name) : CompilerError(
            "Global Constant", line,
            "Global " + name + " must be a constant"
            ) {}
};

class InvalidFunctionCallError: public CompilerError {
public:
    InvalidFunctionCallError(size_t line, string name) : CompilerError(
            "Invalid Function Call", line,
            "Cannot call " + name
            ) {}
};

class InvalidProcedureCallError: public CompilerError {
public:
    InvalidProcedureCallError(size_t line, string name) : CompilerError(
            "Invalid Procedure Call", line,
            "Cannot call " + name
            ) {}
};

class TypedefGlobalError: public CompilerError {
public:
    TypedefGlobalError(size_t line, string name) : CompilerError(
            "Typedef Global", line,
            "Typedef " + name + " must be global"
            ) {}
};

class BuiltInFunctionError: public CompilerError {
public:
    BuiltInFunctionError(size_t line, string name) : CompilerError(
            "Built-In Function", line,
            "Cannot redefine built-in function " + name
            ) {}
};

class BuiltInProcedureError: public CompilerError {
public:
    BuiltInProcedureError(size_t line, string name) : CompilerError(
            "Built-In Procedure", line,
            "Cannot redefine built-in procedure " + name
            ) {}
};

class CombinedQualifierError: public CompilerError {
public:
    CombinedQualifierError(size_t line) : CompilerError(
            "Combined Qualifier", line,
            "Cannot combine type qualifiers"
            ) {}
};

class LValueError: public CompilerError {
public:
    LValueError(size_t line, string name) : CompilerError(
            "L-Value", line,
            "Identifier " + name + " must be a l-value"
            ) {}
};

class FunctionReturnError : public CompilerError {
public:
    FunctionReturnError(size_t line, string name) : CompilerError(
            "Function Return", line,
            "Function " + name + " must return a value in all paths"
            ) {}
};

class FunctionReturnTypeError : public CompilerError {
public:
    FunctionReturnTypeError(size_t line, string name, string expected, string actual) : CompilerError(
            "Function Return Type", line,
            "Function " + name + " must return " + expected + " but returns " + actual
            ) {}
};