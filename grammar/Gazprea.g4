grammar Gazprea;

// -- tokens for AST --
tokens {
}

// --- PARSER RULES ---
file: (global)* EOF;

global
    : identDecl
    | functionDeclr
    | functionDefinition
    | procedureDeclr
    | procedureDefinition;

stmt: simpleStmt | block; // Block should really be called compound statement.

simpleStmt
    : identDecl
    | assignment
    | conditional
    | loop
    | typeDef
    | output
    | input
    | BREAK
    | CONTINUE
    | return
    | functionDeclr
    | functionDefinition
    | procedureDeclr
    | procedureDefinition
    | procedureCall;

identDecl
    : (typeQualifier)? (type)? ID (EQ expr)? SC;

// we cannot assign procedureCall to an ID? I removed it.
assignment
    : ID EQ expr SC;

conditional
    : IF expr stmt              # ifConditional
    | IF expr stmt ELSE stmt    # ifElseConditional;

loop
    : LOOP stmt                 # infiniteLoop
    | LOOP WHILE expr stmt      # whileLoop
    | LOOP iterDomain stmt      # domainLoop;

iterDomain
    : ID GET expr;

//change the first ID token to type
typeDef
    : TYPEDEF type ID SC;

output
    : expr PUT STDOUT SC;

input
    : ID IN STDIN SC;

return
    : RETURN expr SC;

typeQualifier
    : VAR
    | CONST;

// Do we need resolved type?
type
    : ID                #resolvedType
    | tupleTypeDecl     #tupleType
    | ID LSQRPAREN expressionOrWildcard RSQRPAREN   #vectorType
    | ID LSQRPAREN expressionOrWildcard COMMA
    expressionOrWildcard RSQRPAREN                  #matrixType
    | INTEGER           #intType
    | CHARACTER         #charType
    | BOOLEANA          #booleanType
    | REAL              #realType
    ;

expressionOrWildcard:
    (MUL | expr);

tupleTypeDecl
    : TUPLE LPAREN typeOptionalIdentPair COMMA typeOptionalIdentPair
     (COMMA typeOptionalIdentPair)*;


// added (typeQualifier)? because procedure parameters can have type qualifiers
typeOptionalIdentPair
    : (typeQualifier)? type (ID)?;

typeIdentPair
    : (typeQualifier)? type ID;

functionDeclr
    : FUNCTION funcName=ID LPAREN
      (typeOptionalIdentPair (COMMA typeOptionalIdentPair)*)?
      RPAREN RETURNS type SC;

functionDefinition
    : FUNCTION funcName=ID LPAREN
      (typeIdentPair (COMMA typeIdentPair)*)?
      RPAREN RETURNS type (block | EQ expr SC);

procedureDeclr
    : PROCEDURE procName=ID LPAREN
      (typeOptionalIdentPair (COMMA typeOptionalIdentPair)*)?
      RPAREN (RETURNS type)? SC;

procedureDefinition
    : PROCEDURE procName=ID LPAREN
    (typeIdentPair (COMMA typeIdentPair)*)? RPAREN (RETURNS type)? block;

functionCall // Should be an expression.
    : ID LPAREN (expr (COMMA expr)*)? RPAREN;

procedureCall
    : CALL ID LPAREN (expr (COMMA expr)*)? RPAREN SC;

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
    | <assoc=right> expr EXP expr           # expExpr
    | expr op=(MUL | DIV | MOD | SS) expr   # mulDivModSSExpr // A better name perhaps
    | expr op=(ADD | SUB) expr              # addSubExpr
    | expr BY expr                          # byExpr
    | expr op=(LT | GT | LTEQ | GTEQ) expr  # compExpr
    | expr op=(EQEQ | NEQ) expr             # equalExpr
    | expr ANDATOM expr                     # andExpr
    | expr op=(OR | XOR) expr               # orExpr
    | <assoc=right> expr APPENDOP expr      # appendOp
    | AS LT type GT LPAREN expr RPAREN      # explicitCast
    | LSQRPAREN ID IN expr BAR expr RSQRPAREN       # generatorExpr
    | LSQRPAREN ID IN expr AND expr RSQRPAREN       # filterExpr
    | functionCall                          # funcCall
    | LPAREN expr COMMA expr (COMMA expr)* RPAREN   #tupleLiteral
    | ID                                    # identifier
    | NULL_                                 # nullLiteral
    | IDENTITY                              # identityLiteral
    | (TRUE | FALSE)                        # boolLiteral
    | INTLITERAL                            # intLiteral
    | realLit                               # realLiteral
    | CHARLITERAL                           # charLiteral
    ;

realLit : fullRealLiteral | sciRealLiteral ;

sciRealLiteral
    : fullRealLiteral 'e' (ADD | SUB)? INTLITERAL;

fullRealLiteral
    : INTLITERAL PERIOD INTLITERAL    # mainReal
    | INTLITERAL PERIOD               # intReal
    | PERIOD INTLITERAL               # dotReal
    ;

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
E : 'e' ;

// Ops
ADD : '+' ;
SUB : '-' ;
DIV : '/' ;
MUL : '*' ;
SS : '**' ;
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
INTEGER: 'interger';
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

