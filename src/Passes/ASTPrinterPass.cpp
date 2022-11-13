//
// Created by Chen on 2022-11-05.
//

#include "Passes/ASTPrinterPass.h"

void ASTPrinterPass::printTree(ASTNodeT *Root) {
    // Print the root of AST
    visit(Root);
    printSubTree(Root, "");
    std::cout << "\n";
    std::cout << "----------------------------------------------------------\n";
}



void ASTPrinterPass::printSubTree(ASTNodeT *Root, const string &Prefix) {
    // Print the subtrees of AST
    if (Root->numOfChildren() == 0) return;
    std::cout << Prefix;
    size_t NumOfChildren = Root->numOfChildren();
    std::cout << (NumOfChildren > 1 ? "├── " : "");
    for (size_t i = 0; i < NumOfChildren; ++i) {
        ASTNodeT *Child = Root->getChildAt(i);
        if (i < NumOfChildren - 1) {
            if (i > 0) { // added fix
                std::cout << Prefix<< "├── "; // added fix
            } // added fix
            bool PrintStrand = NumOfChildren > 1 && (Child->numOfChildren() != 0);
            std::string NewPrefix = Prefix + (PrintStrand ? "│\t" : "\t");
//            printNodeName(Child);
            visit(Child);
            printSubTree(Child, NewPrefix);
        } else {
            std::cout << (NumOfChildren > 1 ? Prefix : "") << "└── ";
            //printNodeName(Child);
            visit(Child);
            printSubTree(Child, Prefix + "\t");
        }
    }
}

void ASTPrinterPass::visitLogicalOp(LogicalOp *Op) {
    string Symb;

    switch (Op->Op) {
        case LogicalOp::LT:
            Symb = "LT";
            break;
        case LogicalOp::GT:
            Symb = "GT";
            break;
        case LogicalOp::LTEQ:
            Symb = "LTEQ";
            break;
        case LogicalOp::GTEQ:
            Symb = "NE";
            break;
        case LogicalOp::EQEQ:
            Symb = "EQEQ";
            break;
        case LogicalOp::NEQ:
            Symb = "NEQ";
            break;
    }

    std::cout << Symb << "\n";
}

void ASTPrinterPass::visitArithmeticOp(ArithmeticOp *Op) {
    string Symb;
    switch (Op->Op) {
        case ArithmeticOp::EXP:
            Symb = "EXP";
            break;
        case ArithmeticOp::MUL:
            Symb = "MUL";
            break;
        case ArithmeticOp::DIV:
            Symb = "DIV";
            break;
        case ArithmeticOp::MOD:
            Symb = "MOD";
            break;
        case ArithmeticOp::SS:
            Symb = "SS";
            break;
        case ArithmeticOp::ADD:
            Symb = "ADD";
            break;
        case ArithmeticOp::SUB:
            Symb = "SUB";
            break;
    }
    std::cout << Symb << "\n";
}

void ASTPrinterPass::visitBitwiseOp(BitwiseOp *Op) {
    string Symb;

    switch (Op->Op) {
        case BitwiseOp::AND:
            Symb = "AND";
            break;
        case BitwiseOp::OR:
            Symb = "OR";
            break;
        case BitwiseOp::XOR:
            Symb = "XOR";
            break;
    }
    std::cout << Symb << "\n";
}

void ASTPrinterPass::visitUnaryOp(UnaryOp *Op) {
    string Symb;

    switch (Op->Op) {
        case UnaryOp::ADD:
            Symb = "ADD";
            break;
        case UnaryOp::SUB:
            Symb = "SUB";
            break;
        case UnaryOp::NOT:
            Symb = "NOT";
            break;
    }
    std::cout << Symb << "\n";
}
