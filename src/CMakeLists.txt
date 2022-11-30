# Gather our source files in this directory.
set(
  gazprea_src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/TestPasses.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Types/Type.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Types/ScalarTypes.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Types/CompositeTypes.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Types/TypeRegistry.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Types/TypeHelpers.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/PassManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/PassManagerResource.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/Pass.cpp"
	"Passes/BuildAST/ASTBuilderPass.cpp"
	"Passes/Utils/ASTPrinterPass.cpp"
	"Passes/BuildAST/ExprTypeAnnotatorPass.cpp"
	"Passes/Checks/EnsureReturnPass.cpp"
	"Passes/Checks/ContinueAndBreakCheckPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/VisitorPass.cpp"
	"Codegen/CodeGenPass.cpp"
	"Passes/BuildAST/ScopeResolutionPass.cpp"
	"Passes/BuildAST/ConvertIdentMemberAccessToIdxPass.cpp"
	"Passes/Checks/AssignmentTypeCheckerPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ErrorHandling/exceptions.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Symbol/Symbol.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Symbol/SymbolTable.cpp"
	"Passes/Checks/ExplicitCastCheckPass.cpp"
	"Passes/Transformations/TupleNotEqualTransformationPass.cpp"
	"Passes/Utils/ASTPrinterPassWithTypes.cpp"
	"Passes/Transformations/TupleCompToMemberCompPass.cpp"
	"Passes/Checks/ReturnValuePromotionPass.cpp"
	"Passes/Checks/CallableArgumentTypeCheckingPass.cpp"
	"Passes/Checks/ProcedureCallAliasCheckPass.cpp"
	"Passes/Transformations/SimplifyTupleCasting.cpp"
	"Passes/BuildAST/NullIdentityTypeCastPass.cpp"
	"Passes/Checks/ProcedureExpressionCheckPass.cpp"
	"Passes/BuildAST/ChangeMemAccessToMemRef.cpp"
	"Passes/Checks/BadStreamPass.cpp"
	"Passes/Checks/EnsureDeclInBeginOfBlock.cpp"
	"Passes/Checks/EnsureDefinitionPass.cpp"
	"Passes/Checks/EnsureValidGlobalInitPass.cpp"
	"Passes/BuildAST/ConvertFuncCallNodesToProcCallPass.cpp"
	"Passes/Checks/ProcedureCallStatCheckPass.cpp"
	"Passes/Checks/LoopIfConditionalTypeCheckPass.cpp"
	"Passes/Checks/LValueReferenceCheckPass.cpp"
	"Passes/Transformations/SubExpressionCacheSet.cpp"
	"Passes/Checks/ProcedureCallStatCheckPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Codegen/CastOperation.cpp"
	"Passes/Transformations/BubbleGlobalDeclarationPass.cpp"
	"Passes/BuildAST/AddingFreeNodesPass.cpp"
	"Passes/Transformations/VectorScalarOpTransformationPass.cpp")

# Build our executable from the source files.
add_executable(gazc ${gazprea_src_files})
target_include_directories(gazc PUBLIC ${ANTLR_GEN_DIR})

# Ensure that the antlr4-runtime is available.
add_dependencies(gazc antlr)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs core)

# Add the antlr runtime and parser as libraries to link.
target_link_libraries(gazc parser antlr4-runtime ${llvm_libs})

# Symbolic link our executable to the base directory so we don't have to go searching for it.
symlink_to_bin("gazc")
