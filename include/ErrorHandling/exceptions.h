//
// Created by rajan on 11/08/22.
//


#pragma once

#include <string>
#include <sstream>
#include <utility>
#include "Common/TreeNode.h"

using namespace std;

// TODO organize files
// FIXME consolidate

// --- SYNTAX ERRORS ---

class SyntaxError : public std::exception {
private:
    std::string msg;
public:
    explicit SyntaxError(std::string msg) : msg(std::move(msg)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return msg.c_str();
    }
};

// --- COMPILE TIME ERRORS ---

class CompilerError : public std::exception {
private:
    string message;
public:
    CompilerError(const string& title, TreeNode *node, const string& message) {
        stringstream ss;
        ss << title << " Error [line " << node->getLine() << "] : " << message << "\n";
        this->message = ss.str();
    }

    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }
};

class CastError : public CompilerError {
public:
    CastError(TreeNode *node, const string& from, const string& to) : CompilerError(
            "Cast", node,
            "Cannot cast " + from + " to " + to) {}
};

class OutputTypeError : public CompilerError {
public:
    OutputTypeError(TreeNode *node, const string& typeName) : CompilerError(
            "Output Type", node,
            "Cannot output type " + typeName) {}
};

class InputTypeError: public CompilerError {
public:
    InputTypeError(TreeNode *node, const string& typeName) : CompilerError(
            "Input Type", node,
            "Cannot input to type " + typeName) {}
};

class ScalarCastError: public CompilerError {
public:
    ScalarCastError(TreeNode *node, const string& from, const string& to) : CompilerError(
            "Type Cast", node,
            "Cannot cast type " + from + " to " + to) {}
};

class TupleCastError: public CompilerError {
public:
    TupleCastError(TreeNode *node, const string& from, const string& to) : CompilerError(
            "Type Cast", node,
            "Cannot cast tuple " + from + " to " + to) {}
};

class TupleAccessError: public CompilerError {
public:
    TupleAccessError(TreeNode *node, const string& tupleName, const string& index) : CompilerError(
            "Tuple Access", node,
            "Cannot access type " + tupleName + " at item " + index) {}
};

class TupleExpressionError: public CompilerError {
public:
    TupleExpressionError(TreeNode *node) : CompilerError(
            "Tuple Expression", node,
            "Invalid expression used to access tuple") {}
};

class TupleSizeError: public CompilerError {
public:
    TupleSizeError(TreeNode *node, size_t expected, size_t actual) : CompilerError(
            "Tuple Size", node,
            "Expected tuple of size " + to_string(expected) + " but got " + to_string(actual)) {}
};

class OutOfRangeError: public CompilerError {
public:
    OutOfRangeError(TreeNode *node, int index, int size, const string& name) : CompilerError(
            "Out of Range", node,
            "Attempting to access index " + to_string(index) + " on type " + name + " of size " + to_string(size)) {}
};

class ScalarPromotionError : public CompilerError {
public:
    ScalarPromotionError(TreeNode *node, const string& from, const string& to) : CompilerError(
            "Type Promotion", node,
            "Cannot automatically promote " + from + " to " + to) {}
};

class NullDeclarationInferenceError : public CompilerError {
public:
    explicit NullDeclarationInferenceError(TreeNode *node) : CompilerError(
            "Null Declaration Inference", node,
            "Cannot infer the type of null expression"
    ) {}
};

class NullIdentityOpError : public CompilerError {
public:
    explicit NullIdentityOpError(TreeNode *node) : CompilerError(
            "Null Identity Op", node,
            "Cannot operate on null or identity types"
    ) {}
};

class NullTypeInferenceError : public CompilerError {
public:
    explicit NullTypeInferenceError(TreeNode *node) : CompilerError(
            "Null Type Inference", node,
            "Cannot infer the type of null expression"
    ) {}
};

class IdentityTypeInferenceError: public CompilerError {
public:
    explicit IdentityTypeInferenceError(TreeNode *node) : CompilerError(
            "Identity Type Inference", node,
            "Cannot infer the type of identity expression"
    ) {}
};

class NullCastError: public CompilerError {
public:
    explicit NullCastError(TreeNode *node) : CompilerError(
            "Null Cast", node,
            "Cannot cast null to any value"
    ) {}
};

class IdentityCastError: public CompilerError {
public:
    explicit IdentityCastError(TreeNode *node) : CompilerError(
            "Identity Cast", node,
            "Cannot cast identity to any value"
    ) {}
};

class BadIdentifierNameError: public CompilerError {
public:
    BadIdentifierNameError(TreeNode *node, const string& name) : CompilerError(
            "Bad Identifier Name", node,
            "Identifier name " + name + " is not valid"
    ) {}
};

class DeclarationPositionError: public CompilerError {
public:
    DeclarationPositionError(TreeNode *node, const string& name) : CompilerError(
            "Declaration Position", node,
            "Declaration of identifier " + name + " is not at the beginning of the block"
    ) {}
};

class RedeclarationError: public CompilerError {
public:
    RedeclarationError(TreeNode *node, const string& name) : CompilerError(
            "Redeclaration", node,
            "Identifier " + name + " has already been declared"
    ) {}
};

class RedefinitionError: public CompilerError {
public:
    RedefinitionError(TreeNode *node, const string& name) : CompilerError(
            "Redefinition", node,
            "Identifier " + name + " has already been defined"
    ) {}
};

class UndeclaredIdentifierError: public CompilerError {
public:
    UndeclaredIdentifierError(TreeNode *node, const string& name) : CompilerError(
            "Undeclared Identifier", node,
            "Identifier " + name + " has not been declared"
    ) {}
};

class SymbolNotFoundError: public CompilerError {
public:
    SymbolNotFoundError(TreeNode *node, const string& name) : CompilerError(
            "Symbol Not Found", node,
            "Symbol " + name + " has not been found"
    ) {}
};

class InvalidAssignmentError: public CompilerError {
public:
    InvalidAssignmentError(TreeNode *node, const string& name) : CompilerError(
            "Invalid Assignment", node,
            "Cannot assign to " + name
    ) {}
};

class InvalidComparisonOpError: public CompilerError {
public:
    InvalidComparisonOpError(TreeNode *node, const string& type) : CompilerError(
            "Invalid Comparison Op", node,
            "Cannot use comparison operator on type " + type
    ) {}
};

class InvalidEqualityOpError: public CompilerError {
public:
    InvalidEqualityOpError(TreeNode *node, const string& type) : CompilerError(
            "Invalid Equality Op", node,
            "Cannot use equality operator on type " + type
    ) {}
};

class InvalidLogicalOpError: public CompilerError {
public:
    InvalidLogicalOpError(TreeNode *node, const string& type) : CompilerError(
            "Invalid Logical Op", node,
            "Cannot use logical operator on type " + type
    ) {}
};

class InvalidUnaryNotError: public CompilerError {
public:
    InvalidUnaryNotError(TreeNode *node, const string& type) : CompilerError(
            "Invalid Unary Not", node,
            "Cannot use unary not operator on type " + type
    ) {}
};

class InvalidUnaryAddOrSubError: public CompilerError {
public:
    InvalidUnaryAddOrSubError(TreeNode *node, const string& type) : CompilerError(
            "Invalid Unary Add or Sub", node,
            "Cannot use unary add or sub operator on type " + type
    ) {}
};

class InvalidArithmeticOpError: public CompilerError {
public:
    InvalidArithmeticOpError(TreeNode *node, const string& type) : CompilerError(
            "Invalid Arithmetic Op", node,
            "Cannot use arithmetic operator on type " + type
    ) {}
};

class InvalidComparisonError: public CompilerError {
public:
    InvalidComparisonError(TreeNode *node, const string& type, const string& type2) : CompilerError(
            "Invalid Comparison", node,
            "Cannot compare type " + type + " with type " + type2
    ) {}
};

class InvalidTupleComparisonError: public CompilerError {
public:
    InvalidTupleComparisonError(TreeNode *node, const string& type, const string& type2) : CompilerError(
            "Invalid Tuple Comparison", node,
            "Cannot compare tuples of type " + type + " to type " + type2
    ) {}
};

class InvalidTypeSymbolError: public CompilerError {
public:
    InvalidTypeSymbolError(TreeNode *node, const string& name) : CompilerError(
            "Invalid Type Symbol", node,
            "Cannot use " + name + " as a type"
    ) {}
};

class NonCallableError: public CompilerError {
public:
    NonCallableError(TreeNode *node, const string& name) : CompilerError(
            "Non Callable", node,
            "Cannot call " + name + " as it is not callable"
    ) {}
};

class ReservedKeywordError: public CompilerError {
public:
    ReservedKeywordError(TreeNode *node, const string& name) : CompilerError(
            "Reserved Keyword", node,
            "Cannot use reserved keyword " + name
    ) {}
};

class TypeMismatchError: public CompilerError {
public:
    TypeMismatchError(TreeNode *node, const string& name, const string& expected, const string& current) : CompilerError(
            "Type Mismatch", node,
            "Identifier " + name + " is of type " + expected + " but got " + current
    ) {}
};

class ArgumentCountError: public CompilerError {
public:
    ArgumentCountError(TreeNode *node, const string& name, size_t expected, size_t current) : CompilerError(
            "Argument Count", node,
            "Callee " + name + " expects " + to_string(expected) + " arguments but got " + to_string(current)
    ) {}
};

class InvalidAssignmentTypeError: public CompilerError {
public:
    InvalidAssignmentTypeError(TreeNode *node, const string& name, const string& expected, const string& actual) : CompilerError(
            "Invalid Assignment Type", node,
            "Cannot assign " + actual + " to " + name + " of type " + expected
    ) {}
};

class ConstantAssignmentError: public CompilerError {
public:
    ConstantAssignmentError(TreeNode *node, const string& name) : CompilerError(
            "Constant Assignment", node,
            "Cannot assign to constant " + name
    ) {}
};

class GlobalConstantError: public CompilerError {
public:
    GlobalConstantError(TreeNode *node, const string& name) : CompilerError(
            "Global Constant", node,
            "Global " + name + " must be a constant"
    ) {}
};

class InvalidFunctionCallError: public CompilerError {
public:
    InvalidFunctionCallError(TreeNode *node, const string& name) : CompilerError(
            "Invalid Function Call", node,
            "Cannot call " + name
    ) {}
};

class InvalidProcedureCallError: public CompilerError {
public:
    InvalidProcedureCallError(TreeNode *node, const string& name) : CompilerError(
            "Invalid Procedure Call", node,
            "Cannot call " + name
    ) {}
};

class TypedefGlobalError: public CompilerError {
public:
    TypedefGlobalError(TreeNode *node, const string& name) : CompilerError(
            "Typedef Global", node,
            "Typedef " + name + " must be global"
    ) {}
};

class BuiltInFunctionError: public CompilerError {
public:
    BuiltInFunctionError(TreeNode *node, const string& name) : CompilerError(
            "Built-In Function", node,
            "Cannot redefine built-in function " + name
    ) {}
};

class BuiltInProcedureError: public CompilerError {
public:
    BuiltInProcedureError(TreeNode *node, const string& name) : CompilerError(
            "Built-In Procedure", node,
            "Cannot redefine built-in procedure " + name
    ) {}
};

class CombinedQualifierError: public CompilerError {
public:
    explicit CombinedQualifierError(TreeNode *node) : CompilerError(
            "Combined Qualifier", node,
            "Cannot combine type qualifiers"
    ) {}
};

class LValueError: public CompilerError {
public:
    LValueError(TreeNode *node, const string& name) : CompilerError(
            "L-Value", node,
            "Identifier " + name + " must be a l-value"
    ) {}
};

class VariableArgumentError: public CompilerError {
public:
    VariableArgumentError(TreeNode *node, int i, const string& name) : CompilerError(
            "Variable Argument", node,
            "Argument " + to_string(i) + " in call to " + name + " must be a variable (l-value) of the same type"
    ) {}
};

class ConstantArgumentError: public CompilerError {
public:
    ConstantArgumentError(TreeNode *node, int i, const string& name) : CompilerError(
            "Constant Argument", node,
            "Argument " + to_string(i) + " in call to " + name + " must not be a constant type"
    ) {}
};

class FunctionReturnError : public CompilerError {
public:
    FunctionReturnError(TreeNode *node, const string& name) : CompilerError(
            "Function Return", node,
            "Function " + name + " must return a value in all paths"
    ) {}
};

class AliasError : public CompilerError {
public:
    AliasError(TreeNode *node) : CompilerError(
            "Alias", node,
            "Cannot alias variables"
    ) {}
};

class ProcedureReturnError : public CompilerError {
public:
    ProcedureReturnError(TreeNode *node, const string& name) : CompilerError(
            "Procedure Return", node,
            "Procedure " + name + " must return a value in all paths"
    ) {}
};
class NonAccessibleError : public CompilerError {
public:
    NonAccessibleError(TreeNode *node, const string& name) : CompilerError(
            "Non Accessible", node,
            "Type " + name + " is not capable of having an access to members"
    ) {}
};

class FunctionReturnTypeError : public CompilerError {
public:
    FunctionReturnTypeError(TreeNode *node, const string& expected, const string& actual) : CompilerError(
            "Return Type", node,
            "Function or procedure must return " + expected + " but returns " + actual
    ) {}
};

class NoReturnError : public CompilerError {
public:
    NoReturnError(TreeNode *node) : CompilerError(
            "No Return", node,
            "Procedure or function should not return a value"
    ) {}
};

class ConflictingReturnTypeError : public CompilerError {
public:
    ConflictingReturnTypeError(TreeNode *node, const string& name, const string& type1, const string& type2) : CompilerError(
            "Conflicting Return Type", node,
            "Function " + name + " returns " + type1 + " and " + type2
    ) {}
};

class BreakOutsideLoopError : public CompilerError {
public:
    explicit BreakOutsideLoopError(TreeNode *node) : CompilerError(
            "Break Outside Loop", node,
            "Break statement must be inside a loop"
    ) {}
};

class ContinueOutsideLoopError : public CompilerError {
public:
    explicit ContinueOutsideLoopError(TreeNode *node) : CompilerError(
            "Continue Outside Loop", node,
            "Continue statement must be inside a loop"
    ) {}
};