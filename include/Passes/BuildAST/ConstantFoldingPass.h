//
// Created by dhanrajbir on 04/12/22.
//

#ifndef GAZPREABASE_CONSTANTFOLDINGPASS_H
#define GAZPREABASE_CONSTANTFOLDINGPASS_H

struct ConstantFoldingPass : public gazprea::GazpreaBaseVisitor {
    std::any visitIntLiteral(gazprea::GazpreaParser::IntLiteralContext *ctx) override {
        return stol(ctx->getText());
    }

    std::any visitBracketExpr(gazprea::GazpreaParser::BracketExprContext *ctx) override {
        return castToLongVisit(ctx->expr());
    }

    std::any visitUnaryExpr(gazprea::GazpreaParser::UnaryExprContext *ctx) override {
        auto Expr = castToLongVisit(ctx->expr());
        if (ctx->ADD())
            return Expr;

        if (ctx->SUB())
            return -Expr;
    }

    std::any visitAddSubExpr(gazprea::GazpreaParser::AddSubExprContext *ctx) override {
        auto LHS = castToLongVisit(ctx->expr(0));
        auto RHS = castToLongVisit(ctx->expr(1));
        if (ctx->ADD())
            return LHS + RHS;

        if (ctx->SUB())
            return LHS - RHS;
    }

    std::any visitMulDivModDotProdExpr(gazprea::GazpreaParser::MulDivModDotProdExprContext *ctx) override {
        auto LHS = castToLongVisit(ctx->expr(0));
        auto RHS = castToLongVisit(ctx->expr(1));

        if (ctx->MUL())
            return LHS * RHS;

        if (ctx->DIV())
            return LHS / RHS;

        if (ctx->MOD())
            return LHS % RHS;
    }

    long castToLongVisit(antlr4::tree::ParseTree *Tree) {
        return std::any_cast<long>(visit(Tree));
    }
};


#endif //GAZPREABASE_CONSTANTFOLDINGPASS_H
