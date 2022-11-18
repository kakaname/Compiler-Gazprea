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
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ASTBuilderPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ASTPrinterPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ExprTypeAnnotatorPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/EnsureReturnPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/LoopCheckPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/VisitorPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/CodeGenPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ScopeResolutionPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ConvertIdentMemberAccessToIdxPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/AssignmentTypeCheckerPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ErrorHandling/exceptions.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Symbol/Symbol.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Symbol/SymbolTable.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ExplicitCastCheckPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/TupleNotEqualTransformationPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ASTPrinterPassWithTypes.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/TupleCompToMemberCompPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ReturnValuePromotionPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/CallableArgumentTypeCheckingPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ProcedureCallAliasCheckPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/SimplifyTupleCasting.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/NullIdentityTypeCastPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ProcedureExpressionCheckPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ChangeMemAccessToMemRef.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/BadStreamPass.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Passes/ProcedureCallStatCheckPass.cpp")

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
