grammar Gazprea;

// -- tokens for AST --
tokens {
}

// --- PARSER RULES ---
file: (global)* EOF;

global : globalIdentDecl        # globalIdentDeclStmt
       | functionDeclr          # functionDeclrStmt
       | functionDefinition     # functionDefStmt
       | procedureDeclr         # procedureDeclrStmt
       | procedureDefinition    # procedureDefStmt
       | typeDef                # typeDefStmt
       ;

globalIdentDecl : CONST (type)? ID EQ expr SC;

stmt: simpleStmt | block; // Block should really be called compound statement.

// typeDef can only appear at global scope, so I removed typeDef to global
// Also, function/procedure declaration/definition are also removed for similar reason
simpleStmt : identDecl          # identDeclStmt
           | assignment         # assignmentStmt
           | conditional        # conditionalStmt
           | loop               # loopStmt
           | output             # outputStmt
           | input              # inputStmt
           | break              # breakStmt
           | continue           # continueStmt
           | return             # returnStmt
           | procedureCall      # procedureCallStmt
           ;

break : BREAK SC;

continue : CONTINUE SC;

identDecl
    : typeQualifier (type)? ID (EQ expr)? SC
    | (typeQualifier)? type ID (EQ expr)? SC;

// we cannot assign procedureCall to an ID? I removed it.
// Here procedureCall specifically means `call precodurename()`
assignment : lvalue EQ expr SC;

lvalue
    : expr LSQRPAREN expr RSQRPAREN         # indexLValue
    | ID PERIOD (ID | INTLITERAL)           # memAccessLValue
    | lvalue COMMA lvalue (COMMA lvalue)*   # tupleUnpackLValue
    | ID                                    # identLValue
    ;

conditional : IF expr stmt              # ifConditional
            | IF expr stmt ELSE stmt    # ifElseConditional;

loop : LOOP stmt                 #infiniteLoop
     | LOOP WHILE expr stmt      #whileLoop
     | LOOP stmt WHILE expr SC   #doWhileLoop
     | LOOP iterDomain stmt      #domainLoop; // I don't quite understand this one

// I don't quite understand this one
iterDomain : ID GET expr;

typeDef : TYPEDEF type ID SC;

output : expr PUT STDOUT SC;

input : lvalue GET STDIN SC;


return : RETURN (expr)? SC;

typeQualifier : VAR
              | CONST;

type
     : tupleTypeDecl                                    #tupleType
     | type LSQRPAREN expressionOrWildcard RSQRPAREN    #vectorType
     | type LSQRPAREN expressionOrWildcard COMMA
     expressionOrWildcard RSQRPAREN                     #matrixType
     | INTEGER INTERVAL                                 #intervalType
     | INTEGER                                          #intType
     | CHARACTER                                        #charType
     | BOOLEANA                                         #booleanType
     | REAL                                             #realType
     | ID                                               #resolvedType
     ;

expressionOrWildcard: (MUL | expr);

tupleTypeDecl
    : TUPLE LPAREN tupleMemberType COMMA tupleMemberType
    (COMMA tupleMemberType)* RPAREN;

tupleMemberType
    : type (ID)?;

// added (typeQualifier)? because procedure parameters can have type qualifiers
typeOptionalIdentPair : (typeQualifier)? type (ID)?;

typeIdentPair : (typeQualifier)? type ID;

functionParameter : (CONST)? type (ID)?;

functionDeclr
    : FUNCTION funcName=ID LPAREN
      (functionParameter (COMMA functionParameter)*)?
      RPAREN RETURNS type SC;

functionDefinition
    : FUNCTION funcName=ID LPAREN
      (functionParameter (COMMA functionParameter)*)?
      RPAREN RETURNS type (block | EQ expr SC);

procedureDeclr
    : PROCEDURE procName=ID LPAREN
      (typeOptionalIdentPair (COMMA typeOptionalIdentPair)*)?
      RPAREN (RETURNS type)? SC;

procedureDefinition
    : PROCEDURE procName=ID LPAREN
    (typeIdentPair (COMMA typeIdentPair)*)? RPAREN (RETURNS type)? block;


// Should be an expression.
functionCall : ID LPAREN (expr (COMMA expr)*)? RPAREN;

// added a SC token at the end
procedureCall : CALL ID LPAREN (expr (COMMA expr)*)? RPAREN SC;

block : LBRACE (stmt)* RBRACE ;

// TODO: Check precedence(done) and add matrix literals, vector literals,
//  real literals(done), string literals, tuple literals(done).

// 1. promote bracketExpr to the highest priority
// 2. change notExpr to unaryExpr
// 3. expExpr should be right-associative
// 4. appendOp should be right-associative
// 5. And expression operator should be 'and' instead of '&'

expr: LPAREN expr RPAREN                    # bracketExpr
    | ID PERIOD (ID | INTLITERAL)           # memberAccess
    | expr LSQRPAREN expr RSQRPAREN         # indexExpr
    | expr DD expr (BY expr)?               # rangeExpr
    | <assoc=right> op=(ADD | SUB | NOT) expr       # unaryExpr
    | <assoc=right> expr op=EXP expr        # expExpr
    | expr op=(MUL | DIV | MOD | DOTPROD) expr   # mulDivModDotProdExpr // A better name perhaps
    | expr op=(ADD | SUB) expr              # addSubExpr
    | expr BY expr                          # byExpr
    | expr op=(LT | GT | LTEQ | GTEQ) expr  # compExpr
    | expr op=(EQEQ | NEQ) expr             # equalExpr
    | expr op=ANDATOM expr                  # andExpr
    | expr op=(OR | XOR) expr               # orExpr
    | <assoc=right> expr APPENDOP expr      # appendOp
    | AS LT type GT LPAREN expr RPAREN      # explicitCast
    | LSQRPAREN ID IN expr BAR expr RSQRPAREN       # generatorExpr
    | LSQRPAREN ID IN expr AND expr RSQRPAREN       # filterExpr
    | LSQRPAREN ID IN expr COMMA ID IN expr BAR expr RSQRPAREN  #matrixGeneratorExpr
    | LSQRPAREN ID IN expr AND expr (COMMA expr)* RSQRPAREN #filterExpr
    | functionCall                          # funcCall
    | LPAREN expr COMMA expr (COMMA expr)* RPAREN   #tupleLiteral
    | LSQRPAREN expr (COMMA expr)* RSQRPAREN        #vectorLiteral
    | ID                                    # identifier
    | NULL_                                 # nullLiteral
    | IDENTITY                              # identityLiteral
    | (TRUE | FALSE)                        # boolLiteral
    | INTLITERAL                            # intLiteral
    | realLit                               # realLiteral
    | CHARLITERAL                           # charLiteral
    ;


realLit : INTLITERAL? PERIOD INTLITERAL ExponentialLiteral? #realLit1
        | INTLITERAL PERIOD ExponentialLiteral?             #realLit2
        | INTLITERAL ExponentialLiteral                     #realLit3
        ;

// --- LEXER RULES ---

ExponentialLiteral: 'e' (ADD | SUB)? INTLITERAL;

// --- LEXER RULES ---

// Characters ++
LPAREN : '(' ;
RPAREN : ')' ;
LSQRPAREN : '[' ;
RSQRPAREN : ']' ;
LBRACE : '{' ;
RBRACE : '}' ;
BAR : '|' ;
APPENDOP : '||' ;
SC : ';' ;
EQ : '=' ;
DD : '..' ;
PERIOD: '.' ;
AND : '&' ;
PUT : '->' ;
GET : '<-' ;
QUOTE : '\'' ;
COMMA : ',' ;

// Ops
ADD : '+' ;
SUB : '-' ;
DIV : '/' ;
MUL : '*' ;
DOTPROD : '**' ;
LTEQ : '<=' ;
GTEQ : '>=' ;
LT : '<' ;
GT : '>' ;
EQEQ : '==';
NEQ : '!=' ;
EXP : '^' ;
MOD : '%' ;

// Reserved
ANDATOM : 'and' ;
AS : 'as' ;
BOOLEANA : 'boolean' ;
BREAK : 'break' ;
BY : 'by' ;
CALL : 'call' ;
CHARACTER : 'character' ;
COLUMNS : 'columns' ;
CONST : 'const' ;
CONTINUE : 'continue' ;
ELSE : 'else' ;
FALSE : 'false' ;
FUNCTION : 'function' ;
IDENTITY : 'identity' ;
IF : 'if' ;
IN : 'in' ;
INTEGER: 'integer';
INTERVAL : 'interval' ;
LENGTH : 'length' ;
LOOP : 'loop' ;
NOT : 'not' ;
NULL_ : 'null' ;
OR : 'or' ;
PROCEDURE : 'procedure' ;
REAL: 'real';
RETURN : 'return' ;
RETURNS : 'returns' ;
REVERSE : 'reverse' ;
ROWS : 'rows' ;
STDIN : 'std_input' ;
STDOUT : 'std_output' ;
STRSTA : 'stream_state' ;
STRINGATOM : 'string' ;
TRUE : 'true' ;
TUPLE : 'tuple' ;
TYPEDEF : 'typedef' ;
VAR : 'var' ;
WHILE : 'while' ;
XOR : 'xor' ;


INTLITERAL : [0-9]+ ;
ID : [_a-zA-Z][_a-zA-Z0-9]* ;
CHARLITERAL : '\'' . '\''
            | '\'' '\\' [0abtnr"'\\] '\''
            ;
// Skip comments and whitespace
BlockComment : '/*' .*? '*/' -> skip ;
LineComment : '//' ~[\r\n]* -> skip ;
WS : [ \t\r\n]+ -> skip ;