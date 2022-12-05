//
// Created by dhanrajbir on 11/11/22.
//

#ifndef GAZPREABASE_SCOPERESOLUTIONPASS_H
#define GAZPREABASE_SCOPERESOLUTIONPASS_H

#include <map>
#include <string>

#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/VisitorPass.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"
#include "ASTBuilderPass.h"


using llvm::isa;
using llvm::dyn_cast;

using std::map;
using std::string;

// TODO confirm that all children are visited

struct ScopeTreeNode: public TreeNode, ResourceIdMixin<ScopeTreeNode> {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNode::N_ScopeTreeNode;
    }

    using SymbolMapT = map<string, Symbol *>;

    SymbolMapT SymbolsInScope;
    SymbolMapT TypeSymbolsInScope;
    // Note: The type symbols is only really relevant for global scopes

    void declareInScope(const string& Name, Symbol *Sym) {
        declareHelper(Name, Sym, &SymbolsInScope);
    }

    void declareType(const string& Name, Symbol *Sym) {
        declareHelper(Name, Sym, &TypeSymbolsInScope);
    }

    Symbol *resolveType(const string &Name) {
        auto Res = TypeSymbolsInScope.find(Name);
        if (Res != TypeSymbolsInScope.end())
            return Res->second;

        if (!getParent())
            return nullptr;

        auto Parent = dyn_cast<ScopeTreeNode>(getParent());
        assert(Parent && "ScopeTreeNode has a non ScopeTreeNode parent?");
        return Parent->resolveType(Name);
    }

    const Symbol *resolve(const string &Name) {
        auto Res = SymbolsInScope.find(Name);
        if (Res != SymbolsInScope.end())
            return Res->second;

        if (!getParent())
            return nullptr;

        auto Parent = dyn_cast<ScopeTreeNode>(getParent());
        assert(Parent && "ScopeTreeNode has a non ScopeTreeNode parent?");
        return Parent->resolve(Name);
    }



    ScopeTreeNode(): TreeNode(TreeNodeKind::N_ScopeTreeNode) {}

private:

    void declareHelper(const string& Name, Symbol *Sym, SymbolMapT *ScopeMap) {
        auto Res = ScopeMap->find(Name);
        if (Res != ScopeMap->end())
            throw std::runtime_error("Tried to redeclare when already in scope");

        ScopeMap->insert({Name, Sym});
    }


};


struct ScopeResolutionPass : VisitorPass<ScopeResolutionPass, void> {

    ScopeTreeNode *CurrentScope;
    ASTPassManager *PM;

    ExprTypeAnnotatorPass ExprAnnotator;

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ScopeResolutionPass should run on the"
                                      " entire program");
        PM = &PManager;
        auto &GlobalScope = PM->getResource<ScopeTreeNode>();
        CurrentScope = &GlobalScope;
        visit(Root);
    }

    void visitDeclaration(Declaration *Decl) {
        visit(Decl->getInitExpr());
        auto ExprType = runTypeAnnotator(Decl->getInitExpr(), Decl->getIdentType());

        // The type must be inferred.
        if (!Decl->getIdentType()) {
            assert(ExprType && "Cannot infer declaration type");
            assert(!isa<NullTy>(ExprType) && !isa<IdentityTy>(ExprType) &&
                    "Cannot infer identity or null type." );
            if (!Decl->IsConst)
                ExprType = PM->TypeReg.getVarTypeOf(ExprType);
            Decl->setIdentType(ExprType);
            Decl->getIdentifier()->setIdentType(ExprType);
        }


        ExprAnnotator.setOpaqueTyCastTargetTy(nullptr);

        // Infer the size from the expression type.
        do {
            if (!Decl->getIdentType()->isCompositeTy())
                break;

            if (auto VecTy = dyn_cast<VectorTy>(Decl->getIdentType())) {
                // If the size is known, we don't change the type and let it
                // potentially fail when the AssignmentTypeCheckerPass runs.
                if (VecTy->isSizeKnown())
                    break;

                // If the size was specified as an expression, visit it to
                // resolve references.
                if (VecTy->getSizeExpr()) {
                    visit(VecTy->getSizeExpr());
                    break;
                }
                // Otherwise the size must be inferred.
                auto ExprVecTy = dyn_cast<VectorTy>(ExprType);
                assert(ExprVecTy && "Trying to assign non vector to vector with inferred size");
                auto NewDeclType = cast<VectorTy>(PM->TypeReg.getVectorType(
                        VecTy->getInnerTy(), ExprVecTy->getSize(),
                        Decl->IsConst));

                NewDeclType->setSizeExpr(ExprVecTy->getSizeExpr());

                Decl->setIdentType(NewDeclType);
                Decl->getIdentifier()->setIdentType(NewDeclType);
                break;
            }

            if (auto MatTy = dyn_cast<MatrixTy>(Decl->getIdentType())) {
                if (MatTy->isSizeKnown())
                    break;

                // If the size was specified as an expression, we do nothing
                // and let the AssignmentTypeCheckerPass handle it.
                auto Dimensions = make_pair(MatTy->getRowSizeExpr(),
                                            MatTy->getColSizeExpr());

                // If either one of them has an expression, we bail
                if (Dimensions.first || Dimensions.second)
                    break;

                auto ExprMatTy = dyn_cast<MatrixTy>(ExprType);
                assert(ExprMatTy && "Trying to assign non matrix to matrix "
                                    "of unknown size");

                auto NumOfRows = MatTy->isNumOfRowsIsKnown() ?
                        MatTy->getNumOfRows() : ExprMatTy->getNumOfRows();
                auto NumOfCols = MatTy->isNumOfColumnsIsKnown() ?
                        MatTy->getNumOfColumns() : MatTy->getNumOfRows();

                auto NewDeclType = cast<MatrixTy>(PM->TypeReg.getMatrixType(
                        MatTy->getInnerTy(),
                        NumOfRows,
                        NumOfCols,
                        Decl->IsConst));
                NewDeclType->setColSizeExpr(ExprMatTy->getColSizeExpr());
                NewDeclType->setRowSizeExpr(ExprMatTy->getRowSizeExpr());

                Decl->setIdentType(NewDeclType);
                Decl->getIdentifier()->setIdentType(NewDeclType);
                break;
            }
        } while (false);

        auto Sym = PM->SymTable.defineObject(
                Decl->getIdentifier()->getName(), Decl->getIdentType());
        Decl->getIdentifier()->setReferred(Sym);
        CurrentScope->declareInScope(Decl->getIdentifier()->getName(), Sym);
    }

    void visitIdentifier(Identifier *Ident) const {
        auto Resolved = CurrentScope->resolve(Ident->getName());
        if (!Resolved)
            throw std::runtime_error("Symbol for identifier " + Ident->getName() + " not found");
        assert(Resolved->getSymbolType() && "Null symbol type?");
        Ident->setIdentType(Resolved->getSymbolType());
        Ident->setReferred(Resolved);
    }

    void visitAssignment(Assignment *Assign) {
        visit(Assign->getAssignedTo());
        visit(Assign->getExpr());
        auto LeftTy = runTypeAnnotator(Assign->getAssignedTo());
        runTypeAnnotator(Assign->getExpr(), LeftTy);
        ExprAnnotator.setOpaqueTyCastTargetTy(nullptr);
    }

    void visitMemberReference(MemberReference *Ref) const {
        auto IdentName = Ref->getIdentifier()->getName();
        auto Resolved = CurrentScope->resolve(IdentName);
        if (!Resolved)
            throw std::runtime_error("Symbol for identifier " + IdentName + " not found");
        Ref->getIdentifier()->setIdentType(Resolved->getSymbolType());
        Ref->getIdentifier()->setReferred(Resolved);
    }

    void visitProcedureDef(ProcedureDef *Def) {

        auto ProcName = Def->getIdentifier()->getName();
        auto ProcTy = Def->getIdentifier()->getIdentType();
        auto Resolved = CurrentScope->resolve(ProcName);
        if (Resolved) {
            if (!Resolved->getSymbolType()->isSameTypeAs(ProcTy))
                throw runtime_error("Differing declaration and definition");
        } else {
            auto ProcSym = PM->SymTable.defineObject(ProcName, ProcTy);
            CurrentScope->declareInScope(ProcName, ProcSym);
            Def->getIdentifier()->setReferred(ProcSym);
        }


        auto ParamList = Def->getParamList();
        // Add a scope for the parameters of the function.
        auto ProcParamScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(ProcParamScope);
        CurrentScope = ProcParamScope;
        // Iterate over the identifiers that are the function parameters
        // and add them to the scope.
        for (auto *Param : *ParamList) {
            auto ParamIdent = dyn_cast<Identifier>(Param);
            assert(ParamIdent);
            auto ParamName = ParamIdent->getName();
            auto ParamType = ParamIdent->getIdentType();
            assert(ParamType);

            // Declare it in scope.
            auto ParamSym = PM->SymTable.defineObject(ParamName, ParamType);
            ParamIdent->setReferred(ParamSym);
            ProcParamScope->declareInScope(ParamIdent->getName(), ParamSym);
        }
        // Add another scope for the function body.
        visit(Def->getBlock());

        // Go back to the original scope.
        CurrentScope = cast<ScopeTreeNode>(ProcParamScope->getParent());
    }

    void visitFunctionDef(FunctionDef *Def) {

        auto FuncName = Def->getIdentifier()->getName();
        auto FuncTy = Def->getIdentifier()->getIdentType();
        auto Resolved = CurrentScope->resolve(FuncName);
        if (Resolved) {
            assert(Resolved->getSymbolType()->isSameTypeAs(FuncTy) &&
                   "Differing declaration and definition");
            Def->getIdentifier()->setReferred(Resolved);
        } else {
            auto FuncSym = PM->SymTable.defineObject(FuncName, FuncTy);
            CurrentScope->declareInScope(FuncName, FuncSym);
            Def->getIdentifier()->setReferred(FuncSym);
        }

        auto ParamList = Def->getParamList();

        // Add a scope for the parameters of the function.
        auto FuncParamScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(FuncParamScope);
        CurrentScope = FuncParamScope;

        // Iterate over the identifiers that are the function parameters
        // and add them to the scope.
        for (auto *Param : *ParamList) {
            auto ParamIdent = dyn_cast<Identifier>(Param);
            assert(ParamIdent);
            auto ParamName = ParamIdent->getName();
            auto ParamType = ParamIdent->getIdentType();
            assert(ParamType);

            // Declare it in scope.
            auto ParamSym = PM->SymTable.defineObject(ParamName, ParamType);
            ParamIdent->setReferred(ParamSym);
            FuncParamScope->declareInScope(ParamIdent->getName(), ParamSym);
        }
        visit(Def->getBlock());

        // Go back to the original scope.
        CurrentScope = cast<ScopeTreeNode>(FuncParamScope->getParent());


   }

    void visitFunctionDecl(FunctionDecl *Decl) {
        auto FuncName = Decl->getIdentifier()->getName();
        auto FuncTy = Decl->getIdentifier()->getIdentType();
        auto Resolved = CurrentScope->resolve(FuncName);
        if (Resolved) {
            assert(FuncTy->isSameTypeAs(Resolved->getSymbolType())
                && "Differing forward declarations for the same function.");
            Decl->getIdentifier()->setReferred(Resolved);
            return;
        }
        auto FuncSym = PM->SymTable.defineObject(FuncName, FuncTy);
        CurrentScope->declareInScope(FuncName, FuncSym);
        Decl->getIdentifier()->setReferred(FuncSym);
    }

    void visitProcedureDecl(ProcedureDecl *Decl) {
        auto ProcName = Decl->getIdentifier()->getName();
        auto ProcTy = Decl->getIdentifier()->getIdentType();
        auto Resolved = CurrentScope->resolve(ProcName);
        if (Resolved) {
            assert(ProcTy->isSameTypeAs(Resolved->getSymbolType())
                   && "Differing forward declarations for the same function.");
            Decl->getIdentifier()->setReferred(Resolved);
            return;
        }
        auto ProcSym = PM->SymTable.defineObject(ProcName, ProcTy);
        CurrentScope->declareInScope(ProcName, ProcSym);
        Decl->getIdentifier()->setReferred(ProcSym);
    }

    void visitProcedureCall(ProcedureCall *Call) {
        visit(Call->getArgsList());
        visit(Call->getIdentifier());

        vector<Type*> ParamTypes;

        if (auto ProcTy = dyn_cast<ProcedureTy>(
                Call->getIdentifier()->getIdentType())) {
            ParamTypes = ProcTy->getParamTypes();
        } else if (auto FuncTy =  dyn_cast<FunctionTy>(
                Call->getIdentifier()->getIdentType()))
            ParamTypes = FuncTy->getParamTypes();
        else
            throw InvalidProcedureCallError(Call, "Incorrect number of arguments");

        auto Args = Call->getArgsList();

        if (ParamTypes.size() != Args->numOfChildren())
            throw InvalidProcedureCallError(Call, "Incorrect number of arguments");

        for (unsigned long I = 0; I < ParamTypes.size(); I++)
            runTypeAnnotator(Args->getExprAtPos(I), ParamTypes[I]);
        ExprAnnotator.setOpaqueTyCastTargetTy(nullptr);
    }

    void visitFunctionCall(FunctionCall *Call) {
        visit(Call->getArgsList());
        visit(Call->getIdentifier());

        vector<Type*> ParamTypes;

        if (auto ProcTy = dyn_cast<FunctionTy>(
                Call->getIdentifier()->getIdentType())) {
            ParamTypes = ProcTy->getParamTypes();
        } else if (auto FuncTy =  dyn_cast<ProcedureTy>(
                Call->getIdentifier()->getIdentType()))
            ParamTypes = FuncTy->getParamTypes();
        else
            throw InvalidProcedureCallError(Call, "Incorrect number of arguments");

        auto Args = Call->getArgsList();

        if (ParamTypes.size() != Args->numOfChildren())
            throw InvalidProcedureCallError(Call, "Incorrect number of arguments");

        for (size_t I = 0; I < ParamTypes.size(); I++)
            runTypeAnnotator(Args->getExprAtPos(I), ParamTypes[I]);
        ExprAnnotator.setOpaqueTyCastTargetTy(nullptr);
    }

    void visitMemberAccess(MemberAccess *Access) {
        visit(Access->getExpr());
    }

    void visitBlock(Block *Blk) {
        auto BlkScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(BlkScope);
        CurrentScope = BlkScope;
        for (auto Child : *Blk)
            visit(Child);
        CurrentScope = cast<ScopeTreeNode>(CurrentScope->getParent());
    }

    Type *runTypeAnnotator(ASTNodeT *Node, Type *Ty = nullptr) {
        ExprAnnotator.setOpaqueTyCastTargetTy(Ty);
        ExprAnnotator.runOnAST(*PM, Node);
        return PM->getAnnotation<ExprTypeAnnotatorPass>(Node);
    }

    void visitExplicitCast(ExplicitCast *Cast) {
        if (auto VecTy = dyn_cast<VectorTy>(Cast->getTargetType())) {
            if (VecTy->getSizeExpr())
                visit(VecTy->getSizeExpr());
            return;
        }

        if (auto MatTy = dyn_cast<MatrixTy>(Cast->getTargetType())) {
            if (MatTy->getColSizeExpr())
                visit(MatTy->getColSizeExpr());

            if (MatTy->getRowSizeExpr())
                visit(MatTy->getRowSizeExpr());
            return;
        }

    }

};

#endif //GAZPREABASE_SCOPERESOLUTIONPASS_H
