# AST Nodes

## Imported from VCalc
- Program
- Identifier
- Assignment
- Declaration (will always include a value, if none, then null)
- Block
- ComparisonOp
- ArithmaticOp
- Index
- Loop
- IntLiteral


## New Stuff
- NullLiteral
- IdentityLiteral
- RealLiteral
- BoolLiteral
- CharLiteral
- Tuple? (don't know how many tuple type AST nodes we need)
- TupleAccess?
- TupleDecl/TupleDef?
- Conditional (two types of AST nodes)
- Cast
- LogicalOp (and, or, xor)
- UnaryOp (+, -, not)
- FunctionDef/FunctionDecl + FunctionCall
- ProdedureDef/ProcedureDecl + ProcedureCall
- ArgsList
- ParasList
- Return
- Break
- Continue
