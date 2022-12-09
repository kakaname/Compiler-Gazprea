// Generated from /home/anxiety/School/CMPUT415/assign/gazprea/grammar/Gazprea.g4 by ANTLR 4.9.2
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class GazpreaParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.9.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		StringLit=1, ExponentialLiteral1=2, ExponentialLiteral2=3, ExponentialLiteral3=4, 
		ExponentialLiteral4=5, LPAREN=6, RPAREN=7, LSQRPAREN=8, RSQRPAREN=9, LBRACE=10, 
		RBRACE=11, BAR=12, APPENDOP=13, SC=14, EQ=15, DD=16, PERIOD=17, AND=18, 
		PUT=19, GET=20, QUOTE=21, COMMA=22, DOUBLEQUOTE=23, ADD=24, SUB=25, DIV=26, 
		MUL=27, DOTPROD=28, LTEQ=29, GTEQ=30, LT=31, GT=32, EQEQ=33, NEQ=34, EXP=35, 
		MOD=36, ANDATOM=37, AS=38, BOOLEANA=39, BREAK=40, BY=41, CALL=42, CHARACTER=43, 
		COLUMNS=44, CONST=45, CONTINUE=46, ELSE=47, FALSE=48, FUNCTION=49, IDENTITY=50, 
		IF=51, IN=52, INTEGER=53, INTERVAL=54, LENGTH=55, LOOP=56, NOT=57, NULL_=58, 
		OR=59, PROCEDURE=60, REAL=61, RETURN=62, RETURNS=63, REVERSE=64, ROWS=65, 
		STDIN=66, STDOUT=67, STRSTA=68, STRINGATOM=69, TRUE=70, TUPLE=71, TYPEDEF=72, 
		VAR=73, WHILE=74, XOR=75, INTLITERAL=76, ID=77, CHARLITERAL=78, BlockComment=79, 
		LineComment=80, WS=81;
	public static final int
		RULE_file = 0, RULE_global = 1, RULE_globalIdentDecl = 2, RULE_stmt = 3, 
		RULE_simpleStmt = 4, RULE_break = 5, RULE_continue = 6, RULE_identDecl = 7, 
		RULE_assignment = 8, RULE_lvalue = 9, RULE_conditional = 10, RULE_loop = 11, 
		RULE_typeDef = 12, RULE_output = 13, RULE_input = 14, RULE_builtInCall = 15, 
		RULE_return = 16, RULE_typeQualifier = 17, RULE_type = 18, RULE_expressionOrWildcard = 19, 
		RULE_tupleTypeDecl = 20, RULE_tupleMemberType = 21, RULE_typeOptionalIdentPair = 22, 
		RULE_typeIdentPair = 23, RULE_functionParameter = 24, RULE_functionDeclr = 25, 
		RULE_functionDefinition = 26, RULE_procedureDeclr = 27, RULE_procedureDefinition = 28, 
		RULE_functionCall = 29, RULE_procedureCall = 30, RULE_block = 31, RULE_expr = 32, 
		RULE_realLit = 33;
	private static String[] makeRuleNames() {
		return new String[] {
			"file", "global", "globalIdentDecl", "stmt", "simpleStmt", "break", "continue", 
			"identDecl", "assignment", "lvalue", "conditional", "loop", "typeDef", 
			"output", "input", "builtInCall", "return", "typeQualifier", "type", 
			"expressionOrWildcard", "tupleTypeDecl", "tupleMemberType", "typeOptionalIdentPair", 
			"typeIdentPair", "functionParameter", "functionDeclr", "functionDefinition", 
			"procedureDeclr", "procedureDefinition", "functionCall", "procedureCall", 
			"block", "expr", "realLit"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, null, null, null, null, null, "'('", "')'", "'['", "']'", "'{'", 
			"'}'", "'|'", "'||'", "';'", "'='", "'..'", "'.'", "'&'", "'->'", "'<-'", 
			"'''", "','", "'\"'", "'+'", "'-'", "'/'", "'*'", "'**'", "'<='", "'>='", 
			"'<'", "'>'", "'=='", "'!='", "'^'", "'%'", "'and'", "'as'", "'boolean'", 
			"'break'", "'by'", "'call'", "'character'", "'columns'", "'const'", "'continue'", 
			"'else'", "'false'", "'function'", "'identity'", "'if'", "'in'", "'integer'", 
			"'interval'", "'length'", "'loop'", "'not'", "'null'", "'or'", "'procedure'", 
			"'real'", "'return'", "'returns'", "'reverse'", "'rows'", "'std_input'", 
			"'std_output'", "'stream_state'", "'string'", "'true'", "'tuple'", "'typedef'", 
			"'var'", "'while'", "'xor'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "StringLit", "ExponentialLiteral1", "ExponentialLiteral2", "ExponentialLiteral3", 
			"ExponentialLiteral4", "LPAREN", "RPAREN", "LSQRPAREN", "RSQRPAREN", 
			"LBRACE", "RBRACE", "BAR", "APPENDOP", "SC", "EQ", "DD", "PERIOD", "AND", 
			"PUT", "GET", "QUOTE", "COMMA", "DOUBLEQUOTE", "ADD", "SUB", "DIV", "MUL", 
			"DOTPROD", "LTEQ", "GTEQ", "LT", "GT", "EQEQ", "NEQ", "EXP", "MOD", "ANDATOM", 
			"AS", "BOOLEANA", "BREAK", "BY", "CALL", "CHARACTER", "COLUMNS", "CONST", 
			"CONTINUE", "ELSE", "FALSE", "FUNCTION", "IDENTITY", "IF", "IN", "INTEGER", 
			"INTERVAL", "LENGTH", "LOOP", "NOT", "NULL_", "OR", "PROCEDURE", "REAL", 
			"RETURN", "RETURNS", "REVERSE", "ROWS", "STDIN", "STDOUT", "STRSTA", 
			"STRINGATOM", "TRUE", "TUPLE", "TYPEDEF", "VAR", "WHILE", "XOR", "INTLITERAL", 
			"ID", "CHARLITERAL", "BlockComment", "LineComment", "WS"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "Gazprea.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public GazpreaParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	public static class FileContext extends ParserRuleContext {
		public TerminalNode EOF() { return getToken(GazpreaParser.EOF, 0); }
		public List<GlobalContext> global() {
			return getRuleContexts(GlobalContext.class);
		}
		public GlobalContext global(int i) {
			return getRuleContext(GlobalContext.class,i);
		}
		public FileContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_file; }
	}

	public final FileContext file() throws RecognitionException {
		FileContext _localctx = new FileContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_file);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(71);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (((((_la - 45)) & ~0x3f) == 0 && ((1L << (_la - 45)) & ((1L << (CONST - 45)) | (1L << (FUNCTION - 45)) | (1L << (PROCEDURE - 45)) | (1L << (TYPEDEF - 45)))) != 0)) {
				{
				{
				setState(68);
				global();
				}
				}
				setState(73);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(74);
			match(EOF);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class GlobalContext extends ParserRuleContext {
		public GlobalContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_global; }
	 
		public GlobalContext() { }
		public void copyFrom(GlobalContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class ProcedureDeclrStmtContext extends GlobalContext {
		public ProcedureDeclrContext procedureDeclr() {
			return getRuleContext(ProcedureDeclrContext.class,0);
		}
		public ProcedureDeclrStmtContext(GlobalContext ctx) { copyFrom(ctx); }
	}
	public static class ProcedureDefStmtContext extends GlobalContext {
		public ProcedureDefinitionContext procedureDefinition() {
			return getRuleContext(ProcedureDefinitionContext.class,0);
		}
		public ProcedureDefStmtContext(GlobalContext ctx) { copyFrom(ctx); }
	}
	public static class FunctionDeclrStmtContext extends GlobalContext {
		public FunctionDeclrContext functionDeclr() {
			return getRuleContext(FunctionDeclrContext.class,0);
		}
		public FunctionDeclrStmtContext(GlobalContext ctx) { copyFrom(ctx); }
	}
	public static class TypeDefStmtContext extends GlobalContext {
		public TypeDefContext typeDef() {
			return getRuleContext(TypeDefContext.class,0);
		}
		public TypeDefStmtContext(GlobalContext ctx) { copyFrom(ctx); }
	}
	public static class FunctionDefStmtContext extends GlobalContext {
		public FunctionDefinitionContext functionDefinition() {
			return getRuleContext(FunctionDefinitionContext.class,0);
		}
		public FunctionDefStmtContext(GlobalContext ctx) { copyFrom(ctx); }
	}
	public static class GlobalIdentDeclStmtContext extends GlobalContext {
		public GlobalIdentDeclContext globalIdentDecl() {
			return getRuleContext(GlobalIdentDeclContext.class,0);
		}
		public GlobalIdentDeclStmtContext(GlobalContext ctx) { copyFrom(ctx); }
	}

	public final GlobalContext global() throws RecognitionException {
		GlobalContext _localctx = new GlobalContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_global);
		try {
			setState(82);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,1,_ctx) ) {
			case 1:
				_localctx = new GlobalIdentDeclStmtContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(76);
				globalIdentDecl();
				}
				break;
			case 2:
				_localctx = new FunctionDeclrStmtContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(77);
				functionDeclr();
				}
				break;
			case 3:
				_localctx = new FunctionDefStmtContext(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(78);
				functionDefinition();
				}
				break;
			case 4:
				_localctx = new ProcedureDeclrStmtContext(_localctx);
				enterOuterAlt(_localctx, 4);
				{
				setState(79);
				procedureDeclr();
				}
				break;
			case 5:
				_localctx = new ProcedureDefStmtContext(_localctx);
				enterOuterAlt(_localctx, 5);
				{
				setState(80);
				procedureDefinition();
				}
				break;
			case 6:
				_localctx = new TypeDefStmtContext(_localctx);
				enterOuterAlt(_localctx, 6);
				{
				setState(81);
				typeDef();
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class GlobalIdentDeclContext extends ParserRuleContext {
		public TerminalNode CONST() { return getToken(GazpreaParser.CONST, 0); }
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TerminalNode EQ() { return getToken(GazpreaParser.EQ, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public GlobalIdentDeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_globalIdentDecl; }
	}

	public final GlobalIdentDeclContext globalIdentDecl() throws RecognitionException {
		GlobalIdentDeclContext _localctx = new GlobalIdentDeclContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_globalIdentDecl);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(84);
			match(CONST);
			setState(86);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,2,_ctx) ) {
			case 1:
				{
				setState(85);
				type(0);
				}
				break;
			}
			setState(88);
			match(ID);
			setState(89);
			match(EQ);
			setState(90);
			expr(0);
			setState(91);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class StmtContext extends ParserRuleContext {
		public SimpleStmtContext simpleStmt() {
			return getRuleContext(SimpleStmtContext.class,0);
		}
		public BlockContext block() {
			return getRuleContext(BlockContext.class,0);
		}
		public StmtContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_stmt; }
	}

	public final StmtContext stmt() throws RecognitionException {
		StmtContext _localctx = new StmtContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_stmt);
		try {
			setState(95);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case StringLit:
			case ExponentialLiteral1:
			case ExponentialLiteral2:
			case ExponentialLiteral3:
			case ExponentialLiteral4:
			case LPAREN:
			case LSQRPAREN:
			case PERIOD:
			case ADD:
			case SUB:
			case AS:
			case BOOLEANA:
			case BREAK:
			case CALL:
			case CHARACTER:
			case COLUMNS:
			case CONST:
			case CONTINUE:
			case FALSE:
			case IDENTITY:
			case IF:
			case INTEGER:
			case LENGTH:
			case LOOP:
			case NOT:
			case NULL_:
			case REAL:
			case RETURN:
			case REVERSE:
			case ROWS:
			case STRSTA:
			case STRINGATOM:
			case TRUE:
			case TUPLE:
			case VAR:
			case INTLITERAL:
			case ID:
			case CHARLITERAL:
				enterOuterAlt(_localctx, 1);
				{
				setState(93);
				simpleStmt();
				}
				break;
			case LBRACE:
				enterOuterAlt(_localctx, 2);
				{
				setState(94);
				block();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class SimpleStmtContext extends ParserRuleContext {
		public SimpleStmtContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_simpleStmt; }
	 
		public SimpleStmtContext() { }
		public void copyFrom(SimpleStmtContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class LoopStmtContext extends SimpleStmtContext {
		public LoopContext loop() {
			return getRuleContext(LoopContext.class,0);
		}
		public LoopStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class ProcedureCallStmtContext extends SimpleStmtContext {
		public ProcedureCallContext procedureCall() {
			return getRuleContext(ProcedureCallContext.class,0);
		}
		public ProcedureCallStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class ConditionalStmtContext extends SimpleStmtContext {
		public ConditionalContext conditional() {
			return getRuleContext(ConditionalContext.class,0);
		}
		public ConditionalStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class BreakStmtContext extends SimpleStmtContext {
		public BreakContext break() {
			return getRuleContext(BreakContext.class,0);
		}
		public BreakStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class OutputStmtContext extends SimpleStmtContext {
		public OutputContext output() {
			return getRuleContext(OutputContext.class,0);
		}
		public OutputStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class ReturnStmtContext extends SimpleStmtContext {
		public ReturnContext return() {
			return getRuleContext(ReturnContext.class,0);
		}
		public ReturnStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class ContinueStmtContext extends SimpleStmtContext {
		public ContinueContext continue() {
			return getRuleContext(ContinueContext.class,0);
		}
		public ContinueStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class AssignmentStmtContext extends SimpleStmtContext {
		public AssignmentContext assignment() {
			return getRuleContext(AssignmentContext.class,0);
		}
		public AssignmentStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class IdentDeclStmtContext extends SimpleStmtContext {
		public IdentDeclContext identDecl() {
			return getRuleContext(IdentDeclContext.class,0);
		}
		public IdentDeclStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}
	public static class InputStmtContext extends SimpleStmtContext {
		public InputContext input() {
			return getRuleContext(InputContext.class,0);
		}
		public InputStmtContext(SimpleStmtContext ctx) { copyFrom(ctx); }
	}

	public final SimpleStmtContext simpleStmt() throws RecognitionException {
		SimpleStmtContext _localctx = new SimpleStmtContext(_ctx, getState());
		enterRule(_localctx, 8, RULE_simpleStmt);
		try {
			setState(107);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,4,_ctx) ) {
			case 1:
				_localctx = new IdentDeclStmtContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(97);
				identDecl();
				}
				break;
			case 2:
				_localctx = new AssignmentStmtContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(98);
				assignment();
				}
				break;
			case 3:
				_localctx = new ConditionalStmtContext(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(99);
				conditional();
				}
				break;
			case 4:
				_localctx = new LoopStmtContext(_localctx);
				enterOuterAlt(_localctx, 4);
				{
				setState(100);
				loop();
				}
				break;
			case 5:
				_localctx = new OutputStmtContext(_localctx);
				enterOuterAlt(_localctx, 5);
				{
				setState(101);
				output();
				}
				break;
			case 6:
				_localctx = new InputStmtContext(_localctx);
				enterOuterAlt(_localctx, 6);
				{
				setState(102);
				input();
				}
				break;
			case 7:
				_localctx = new BreakStmtContext(_localctx);
				enterOuterAlt(_localctx, 7);
				{
				setState(103);
				break();
				}
				break;
			case 8:
				_localctx = new ContinueStmtContext(_localctx);
				enterOuterAlt(_localctx, 8);
				{
				setState(104);
				continue();
				}
				break;
			case 9:
				_localctx = new ReturnStmtContext(_localctx);
				enterOuterAlt(_localctx, 9);
				{
				setState(105);
				return();
				}
				break;
			case 10:
				_localctx = new ProcedureCallStmtContext(_localctx);
				enterOuterAlt(_localctx, 10);
				{
				setState(106);
				procedureCall();
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class BreakContext extends ParserRuleContext {
		public TerminalNode BREAK() { return getToken(GazpreaParser.BREAK, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public BreakContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_break; }
	}

	public final BreakContext break() throws RecognitionException {
		BreakContext _localctx = new BreakContext(_ctx, getState());
		enterRule(_localctx, 10, RULE_break);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(109);
			match(BREAK);
			setState(110);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ContinueContext extends ParserRuleContext {
		public TerminalNode CONTINUE() { return getToken(GazpreaParser.CONTINUE, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public ContinueContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_continue; }
	}

	public final ContinueContext continue() throws RecognitionException {
		ContinueContext _localctx = new ContinueContext(_ctx, getState());
		enterRule(_localctx, 12, RULE_continue);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(112);
			match(CONTINUE);
			setState(113);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class IdentDeclContext extends ParserRuleContext {
		public TypeQualifierContext typeQualifier() {
			return getRuleContext(TypeQualifierContext.class,0);
		}
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode EQ() { return getToken(GazpreaParser.EQ, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public IdentDeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_identDecl; }
	}

	public final IdentDeclContext identDecl() throws RecognitionException {
		IdentDeclContext _localctx = new IdentDeclContext(_ctx, getState());
		enterRule(_localctx, 14, RULE_identDecl);
		int _la;
		try {
			setState(137);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,9,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(115);
				typeQualifier();
				setState(117);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,5,_ctx) ) {
				case 1:
					{
					setState(116);
					type(0);
					}
					break;
				}
				setState(119);
				match(ID);
				setState(122);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==EQ) {
					{
					setState(120);
					match(EQ);
					setState(121);
					expr(0);
					}
				}

				setState(124);
				match(SC);
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(127);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==CONST || _la==VAR) {
					{
					setState(126);
					typeQualifier();
					}
				}

				setState(129);
				type(0);
				setState(130);
				match(ID);
				setState(133);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==EQ) {
					{
					setState(131);
					match(EQ);
					setState(132);
					expr(0);
					}
				}

				setState(135);
				match(SC);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class AssignmentContext extends ParserRuleContext {
		public LvalueContext lvalue() {
			return getRuleContext(LvalueContext.class,0);
		}
		public TerminalNode EQ() { return getToken(GazpreaParser.EQ, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public AssignmentContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_assignment; }
	}

	public final AssignmentContext assignment() throws RecognitionException {
		AssignmentContext _localctx = new AssignmentContext(_ctx, getState());
		enterRule(_localctx, 16, RULE_assignment);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(139);
			lvalue();
			setState(140);
			match(EQ);
			setState(141);
			expr(0);
			setState(142);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class LvalueContext extends ParserRuleContext {
		public LvalueContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_lvalue; }
	 
		public LvalueContext() { }
		public void copyFrom(LvalueContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class TupleUnpackLValueContext extends LvalueContext {
		public List<TerminalNode> ID() { return getTokens(GazpreaParser.ID); }
		public TerminalNode ID(int i) {
			return getToken(GazpreaParser.ID, i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public TupleUnpackLValueContext(LvalueContext ctx) { copyFrom(ctx); }
	}
	public static class IndexLValueContext extends LvalueContext {
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public TerminalNode COMMA() { return getToken(GazpreaParser.COMMA, 0); }
		public IndexLValueContext(LvalueContext ctx) { copyFrom(ctx); }
	}
	public static class MemAccessLValueContext extends LvalueContext {
		public List<TerminalNode> ID() { return getTokens(GazpreaParser.ID); }
		public TerminalNode ID(int i) {
			return getToken(GazpreaParser.ID, i);
		}
		public TerminalNode PERIOD() { return getToken(GazpreaParser.PERIOD, 0); }
		public TerminalNode INTLITERAL() { return getToken(GazpreaParser.INTLITERAL, 0); }
		public MemAccessLValueContext(LvalueContext ctx) { copyFrom(ctx); }
	}
	public static class IdentLValueContext extends LvalueContext {
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public IdentLValueContext(LvalueContext ctx) { copyFrom(ctx); }
	}

	public final LvalueContext lvalue() throws RecognitionException {
		LvalueContext _localctx = new LvalueContext(_ctx, getState());
		enterRule(_localctx, 18, RULE_lvalue);
		int _la;
		try {
			setState(167);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,12,_ctx) ) {
			case 1:
				_localctx = new IndexLValueContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(144);
				expr(0);
				setState(145);
				match(LSQRPAREN);
				setState(146);
				expr(0);
				setState(149);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==COMMA) {
					{
					setState(147);
					match(COMMA);
					setState(148);
					expr(0);
					}
				}

				setState(151);
				match(RSQRPAREN);
				}
				break;
			case 2:
				_localctx = new MemAccessLValueContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(153);
				match(ID);
				setState(154);
				match(PERIOD);
				setState(155);
				_la = _input.LA(1);
				if ( !(_la==INTLITERAL || _la==ID) ) {
				_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				}
				break;
			case 3:
				_localctx = new TupleUnpackLValueContext(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(156);
				match(ID);
				setState(157);
				match(COMMA);
				setState(158);
				match(ID);
				setState(163);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(159);
					match(COMMA);
					setState(160);
					match(ID);
					}
					}
					setState(165);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
				break;
			case 4:
				_localctx = new IdentLValueContext(_localctx);
				enterOuterAlt(_localctx, 4);
				{
				setState(166);
				match(ID);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ConditionalContext extends ParserRuleContext {
		public ConditionalContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_conditional; }
	 
		public ConditionalContext() { }
		public void copyFrom(ConditionalContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class IfConditionalContext extends ConditionalContext {
		public TerminalNode IF() { return getToken(GazpreaParser.IF, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public StmtContext stmt() {
			return getRuleContext(StmtContext.class,0);
		}
		public IfConditionalContext(ConditionalContext ctx) { copyFrom(ctx); }
	}
	public static class IfElseConditionalContext extends ConditionalContext {
		public TerminalNode IF() { return getToken(GazpreaParser.IF, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public List<StmtContext> stmt() {
			return getRuleContexts(StmtContext.class);
		}
		public StmtContext stmt(int i) {
			return getRuleContext(StmtContext.class,i);
		}
		public TerminalNode ELSE() { return getToken(GazpreaParser.ELSE, 0); }
		public IfElseConditionalContext(ConditionalContext ctx) { copyFrom(ctx); }
	}

	public final ConditionalContext conditional() throws RecognitionException {
		ConditionalContext _localctx = new ConditionalContext(_ctx, getState());
		enterRule(_localctx, 20, RULE_conditional);
		try {
			setState(179);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,13,_ctx) ) {
			case 1:
				_localctx = new IfConditionalContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(169);
				match(IF);
				setState(170);
				expr(0);
				setState(171);
				stmt();
				}
				break;
			case 2:
				_localctx = new IfElseConditionalContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(173);
				match(IF);
				setState(174);
				expr(0);
				setState(175);
				stmt();
				setState(176);
				match(ELSE);
				setState(177);
				stmt();
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class LoopContext extends ParserRuleContext {
		public LoopContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_loop; }
	 
		public LoopContext() { }
		public void copyFrom(LoopContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class DomainLoopContext extends LoopContext {
		public TerminalNode LOOP() { return getToken(GazpreaParser.LOOP, 0); }
		public StmtContext stmt() {
			return getRuleContext(StmtContext.class,0);
		}
		public List<TerminalNode> ID() { return getTokens(GazpreaParser.ID); }
		public TerminalNode ID(int i) {
			return getToken(GazpreaParser.ID, i);
		}
		public List<TerminalNode> IN() { return getTokens(GazpreaParser.IN); }
		public TerminalNode IN(int i) {
			return getToken(GazpreaParser.IN, i);
		}
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public DomainLoopContext(LoopContext ctx) { copyFrom(ctx); }
	}
	public static class WhileLoopContext extends LoopContext {
		public TerminalNode LOOP() { return getToken(GazpreaParser.LOOP, 0); }
		public TerminalNode WHILE() { return getToken(GazpreaParser.WHILE, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public StmtContext stmt() {
			return getRuleContext(StmtContext.class,0);
		}
		public WhileLoopContext(LoopContext ctx) { copyFrom(ctx); }
	}
	public static class InfiniteLoopContext extends LoopContext {
		public TerminalNode LOOP() { return getToken(GazpreaParser.LOOP, 0); }
		public StmtContext stmt() {
			return getRuleContext(StmtContext.class,0);
		}
		public InfiniteLoopContext(LoopContext ctx) { copyFrom(ctx); }
	}
	public static class DoWhileLoopContext extends LoopContext {
		public TerminalNode LOOP() { return getToken(GazpreaParser.LOOP, 0); }
		public StmtContext stmt() {
			return getRuleContext(StmtContext.class,0);
		}
		public TerminalNode WHILE() { return getToken(GazpreaParser.WHILE, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public DoWhileLoopContext(LoopContext ctx) { copyFrom(ctx); }
	}

	public final LoopContext loop() throws RecognitionException {
		LoopContext _localctx = new LoopContext(_ctx, getState());
		enterRule(_localctx, 22, RULE_loop);
		int _la;
		try {
			setState(210);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,15,_ctx) ) {
			case 1:
				_localctx = new InfiniteLoopContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(181);
				match(LOOP);
				setState(182);
				stmt();
				}
				break;
			case 2:
				_localctx = new WhileLoopContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(183);
				match(LOOP);
				setState(184);
				match(WHILE);
				setState(185);
				expr(0);
				setState(186);
				stmt();
				}
				break;
			case 3:
				_localctx = new DoWhileLoopContext(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(188);
				match(LOOP);
				setState(189);
				stmt();
				setState(190);
				match(WHILE);
				setState(191);
				expr(0);
				setState(192);
				match(SC);
				}
				break;
			case 4:
				_localctx = new DomainLoopContext(_localctx);
				enterOuterAlt(_localctx, 4);
				{
				setState(194);
				match(LOOP);
				{
				setState(195);
				match(ID);
				setState(196);
				match(IN);
				setState(197);
				expr(0);
				}
				setState(205);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(199);
					match(COMMA);
					setState(200);
					match(ID);
					setState(201);
					match(IN);
					setState(202);
					expr(0);
					}
					}
					setState(207);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(208);
				stmt();
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TypeDefContext extends ParserRuleContext {
		public TerminalNode TYPEDEF() { return getToken(GazpreaParser.TYPEDEF, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public TypeDefContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_typeDef; }
	}

	public final TypeDefContext typeDef() throws RecognitionException {
		TypeDefContext _localctx = new TypeDefContext(_ctx, getState());
		enterRule(_localctx, 24, RULE_typeDef);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(212);
			match(TYPEDEF);
			setState(213);
			type(0);
			setState(214);
			match(ID);
			setState(215);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class OutputContext extends ParserRuleContext {
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode PUT() { return getToken(GazpreaParser.PUT, 0); }
		public TerminalNode STDOUT() { return getToken(GazpreaParser.STDOUT, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public OutputContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_output; }
	}

	public final OutputContext output() throws RecognitionException {
		OutputContext _localctx = new OutputContext(_ctx, getState());
		enterRule(_localctx, 26, RULE_output);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(217);
			expr(0);
			setState(218);
			match(PUT);
			setState(219);
			match(STDOUT);
			setState(220);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class InputContext extends ParserRuleContext {
		public LvalueContext lvalue() {
			return getRuleContext(LvalueContext.class,0);
		}
		public TerminalNode GET() { return getToken(GazpreaParser.GET, 0); }
		public TerminalNode STDIN() { return getToken(GazpreaParser.STDIN, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public InputContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_input; }
	}

	public final InputContext input() throws RecognitionException {
		InputContext _localctx = new InputContext(_ctx, getState());
		enterRule(_localctx, 28, RULE_input);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(222);
			lvalue();
			setState(223);
			match(GET);
			setState(224);
			match(STDIN);
			setState(225);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class BuiltInCallContext extends ParserRuleContext {
		public BuiltInCallContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_builtInCall; }
	 
		public BuiltInCallContext() { }
		public void copyFrom(BuiltInCallContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class BuiltInColContext extends BuiltInCallContext {
		public TerminalNode COLUMNS() { return getToken(GazpreaParser.COLUMNS, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public BuiltInColContext(BuiltInCallContext ctx) { copyFrom(ctx); }
	}
	public static class BuiltInLenContext extends BuiltInCallContext {
		public TerminalNode LENGTH() { return getToken(GazpreaParser.LENGTH, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public BuiltInLenContext(BuiltInCallContext ctx) { copyFrom(ctx); }
	}
	public static class BuiltInRowsContext extends BuiltInCallContext {
		public TerminalNode ROWS() { return getToken(GazpreaParser.ROWS, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public BuiltInRowsContext(BuiltInCallContext ctx) { copyFrom(ctx); }
	}
	public static class BuiltInReverseContext extends BuiltInCallContext {
		public TerminalNode REVERSE() { return getToken(GazpreaParser.REVERSE, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public BuiltInReverseContext(BuiltInCallContext ctx) { copyFrom(ctx); }
	}
	public static class BuiltInStreamStateContext extends BuiltInCallContext {
		public TerminalNode STRSTA() { return getToken(GazpreaParser.STRSTA, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public TerminalNode STDIN() { return getToken(GazpreaParser.STDIN, 0); }
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public BuiltInStreamStateContext(BuiltInCallContext ctx) { copyFrom(ctx); }
	}

	public final BuiltInCallContext builtInCall() throws RecognitionException {
		BuiltInCallContext _localctx = new BuiltInCallContext(_ctx, getState());
		enterRule(_localctx, 30, RULE_builtInCall);
		try {
			setState(251);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case LENGTH:
				_localctx = new BuiltInLenContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(227);
				match(LENGTH);
				setState(228);
				match(LPAREN);
				setState(229);
				expr(0);
				setState(230);
				match(RPAREN);
				}
				break;
			case ROWS:
				_localctx = new BuiltInRowsContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(232);
				match(ROWS);
				setState(233);
				match(LPAREN);
				setState(234);
				expr(0);
				setState(235);
				match(RPAREN);
				}
				break;
			case COLUMNS:
				_localctx = new BuiltInColContext(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(237);
				match(COLUMNS);
				setState(238);
				match(LPAREN);
				setState(239);
				expr(0);
				setState(240);
				match(RPAREN);
				}
				break;
			case REVERSE:
				_localctx = new BuiltInReverseContext(_localctx);
				enterOuterAlt(_localctx, 4);
				{
				setState(242);
				match(REVERSE);
				setState(243);
				match(LPAREN);
				setState(244);
				expr(0);
				setState(245);
				match(RPAREN);
				}
				break;
			case STRSTA:
				_localctx = new BuiltInStreamStateContext(_localctx);
				enterOuterAlt(_localctx, 5);
				{
				setState(247);
				match(STRSTA);
				setState(248);
				match(LPAREN);
				setState(249);
				match(STDIN);
				setState(250);
				match(RPAREN);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ReturnContext extends ParserRuleContext {
		public TerminalNode RETURN() { return getToken(GazpreaParser.RETURN, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public ReturnContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_return; }
	}

	public final ReturnContext return() throws RecognitionException {
		ReturnContext _localctx = new ReturnContext(_ctx, getState());
		enterRule(_localctx, 32, RULE_return);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(253);
			match(RETURN);
			setState(255);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << StringLit) | (1L << ExponentialLiteral1) | (1L << ExponentialLiteral2) | (1L << ExponentialLiteral3) | (1L << ExponentialLiteral4) | (1L << LPAREN) | (1L << LSQRPAREN) | (1L << PERIOD) | (1L << ADD) | (1L << SUB) | (1L << AS) | (1L << COLUMNS) | (1L << FALSE) | (1L << IDENTITY) | (1L << LENGTH) | (1L << NOT) | (1L << NULL_))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (REVERSE - 64)) | (1L << (ROWS - 64)) | (1L << (STRSTA - 64)) | (1L << (TRUE - 64)) | (1L << (INTLITERAL - 64)) | (1L << (ID - 64)) | (1L << (CHARLITERAL - 64)))) != 0)) {
				{
				setState(254);
				expr(0);
				}
			}

			setState(257);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TypeQualifierContext extends ParserRuleContext {
		public TerminalNode VAR() { return getToken(GazpreaParser.VAR, 0); }
		public TerminalNode CONST() { return getToken(GazpreaParser.CONST, 0); }
		public TypeQualifierContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_typeQualifier; }
	}

	public final TypeQualifierContext typeQualifier() throws RecognitionException {
		TypeQualifierContext _localctx = new TypeQualifierContext(_ctx, getState());
		enterRule(_localctx, 34, RULE_typeQualifier);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(259);
			_la = _input.LA(1);
			if ( !(_la==CONST || _la==VAR) ) {
			_errHandler.recoverInline(this);
			}
			else {
				if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
				_errHandler.reportMatch(this);
				consume();
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TypeContext extends ParserRuleContext {
		public TypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_type; }
	 
		public TypeContext() { }
		public void copyFrom(TypeContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class CharTypeContext extends TypeContext {
		public TerminalNode CHARACTER() { return getToken(GazpreaParser.CHARACTER, 0); }
		public CharTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class IntervalTypeContext extends TypeContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode INTERVAL() { return getToken(GazpreaParser.INTERVAL, 0); }
		public IntervalTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class BooleanTypeContext extends TypeContext {
		public TerminalNode BOOLEANA() { return getToken(GazpreaParser.BOOLEANA, 0); }
		public BooleanTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class RealTypeContext extends TypeContext {
		public TerminalNode REAL() { return getToken(GazpreaParser.REAL, 0); }
		public RealTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class ResolvedTypeContext extends TypeContext {
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public ResolvedTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class VectorTypeContext extends TypeContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public ExpressionOrWildcardContext expressionOrWildcard() {
			return getRuleContext(ExpressionOrWildcardContext.class,0);
		}
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public VectorTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class IntTypeContext extends TypeContext {
		public TerminalNode INTEGER() { return getToken(GazpreaParser.INTEGER, 0); }
		public IntTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class StringTypeContext extends TypeContext {
		public TerminalNode STRINGATOM() { return getToken(GazpreaParser.STRINGATOM, 0); }
		public StringTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class TupleTypeContext extends TypeContext {
		public TupleTypeDeclContext tupleTypeDecl() {
			return getRuleContext(TupleTypeDeclContext.class,0);
		}
		public TupleTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}
	public static class MatrixTypeContext extends TypeContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public List<ExpressionOrWildcardContext> expressionOrWildcard() {
			return getRuleContexts(ExpressionOrWildcardContext.class);
		}
		public ExpressionOrWildcardContext expressionOrWildcard(int i) {
			return getRuleContext(ExpressionOrWildcardContext.class,i);
		}
		public TerminalNode COMMA() { return getToken(GazpreaParser.COMMA, 0); }
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public MatrixTypeContext(TypeContext ctx) { copyFrom(ctx); }
	}

	public final TypeContext type() throws RecognitionException {
		return type(0);
	}

	private TypeContext type(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		TypeContext _localctx = new TypeContext(_ctx, _parentState);
		TypeContext _prevctx = _localctx;
		int _startState = 36;
		enterRecursionRule(_localctx, 36, RULE_type, _p);
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(269);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case TUPLE:
				{
				_localctx = new TupleTypeContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;

				setState(262);
				tupleTypeDecl();
				}
				break;
			case INTEGER:
				{
				_localctx = new IntTypeContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(263);
				match(INTEGER);
				}
				break;
			case CHARACTER:
				{
				_localctx = new CharTypeContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(264);
				match(CHARACTER);
				}
				break;
			case BOOLEANA:
				{
				_localctx = new BooleanTypeContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(265);
				match(BOOLEANA);
				}
				break;
			case REAL:
				{
				_localctx = new RealTypeContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(266);
				match(REAL);
				}
				break;
			case ID:
				{
				_localctx = new ResolvedTypeContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(267);
				match(ID);
				}
				break;
			case STRINGATOM:
				{
				_localctx = new StringTypeContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(268);
				match(STRINGATOM);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
			_ctx.stop = _input.LT(-1);
			setState(287);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,20,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(285);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,19,_ctx) ) {
					case 1:
						{
						_localctx = new VectorTypeContext(new TypeContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_type);
						setState(271);
						if (!(precpred(_ctx, 9))) throw new FailedPredicateException(this, "precpred(_ctx, 9)");
						setState(272);
						match(LSQRPAREN);
						setState(273);
						expressionOrWildcard();
						setState(274);
						match(RSQRPAREN);
						}
						break;
					case 2:
						{
						_localctx = new MatrixTypeContext(new TypeContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_type);
						setState(276);
						if (!(precpred(_ctx, 8))) throw new FailedPredicateException(this, "precpred(_ctx, 8)");
						setState(277);
						match(LSQRPAREN);
						setState(278);
						expressionOrWildcard();
						setState(279);
						match(COMMA);
						setState(280);
						expressionOrWildcard();
						setState(281);
						match(RSQRPAREN);
						}
						break;
					case 3:
						{
						_localctx = new IntervalTypeContext(new TypeContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_type);
						setState(283);
						if (!(precpred(_ctx, 7))) throw new FailedPredicateException(this, "precpred(_ctx, 7)");
						setState(284);
						match(INTERVAL);
						}
						break;
					}
					} 
				}
				setState(289);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,20,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public static class ExpressionOrWildcardContext extends ParserRuleContext {
		public TerminalNode MUL() { return getToken(GazpreaParser.MUL, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public ExpressionOrWildcardContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expressionOrWildcard; }
	}

	public final ExpressionOrWildcardContext expressionOrWildcard() throws RecognitionException {
		ExpressionOrWildcardContext _localctx = new ExpressionOrWildcardContext(_ctx, getState());
		enterRule(_localctx, 38, RULE_expressionOrWildcard);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(292);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case MUL:
				{
				setState(290);
				match(MUL);
				}
				break;
			case StringLit:
			case ExponentialLiteral1:
			case ExponentialLiteral2:
			case ExponentialLiteral3:
			case ExponentialLiteral4:
			case LPAREN:
			case LSQRPAREN:
			case PERIOD:
			case ADD:
			case SUB:
			case AS:
			case COLUMNS:
			case FALSE:
			case IDENTITY:
			case LENGTH:
			case NOT:
			case NULL_:
			case REVERSE:
			case ROWS:
			case STRSTA:
			case TRUE:
			case INTLITERAL:
			case ID:
			case CHARLITERAL:
				{
				setState(291);
				expr(0);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TupleTypeDeclContext extends ParserRuleContext {
		public TerminalNode TUPLE() { return getToken(GazpreaParser.TUPLE, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public List<TupleMemberTypeContext> tupleMemberType() {
			return getRuleContexts(TupleMemberTypeContext.class);
		}
		public TupleMemberTypeContext tupleMemberType(int i) {
			return getRuleContext(TupleMemberTypeContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public TupleTypeDeclContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_tupleTypeDecl; }
	}

	public final TupleTypeDeclContext tupleTypeDecl() throws RecognitionException {
		TupleTypeDeclContext _localctx = new TupleTypeDeclContext(_ctx, getState());
		enterRule(_localctx, 40, RULE_tupleTypeDecl);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(294);
			match(TUPLE);
			setState(295);
			match(LPAREN);
			setState(296);
			tupleMemberType();
			setState(297);
			match(COMMA);
			setState(298);
			tupleMemberType();
			setState(303);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==COMMA) {
				{
				{
				setState(299);
				match(COMMA);
				setState(300);
				tupleMemberType();
				}
				}
				setState(305);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(306);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TupleMemberTypeContext extends ParserRuleContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TupleMemberTypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_tupleMemberType; }
	}

	public final TupleMemberTypeContext tupleMemberType() throws RecognitionException {
		TupleMemberTypeContext _localctx = new TupleMemberTypeContext(_ctx, getState());
		enterRule(_localctx, 42, RULE_tupleMemberType);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(308);
			type(0);
			setState(310);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==ID) {
				{
				setState(309);
				match(ID);
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TypeOptionalIdentPairContext extends ParserRuleContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TypeQualifierContext typeQualifier() {
			return getRuleContext(TypeQualifierContext.class,0);
		}
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TypeOptionalIdentPairContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_typeOptionalIdentPair; }
	}

	public final TypeOptionalIdentPairContext typeOptionalIdentPair() throws RecognitionException {
		TypeOptionalIdentPairContext _localctx = new TypeOptionalIdentPairContext(_ctx, getState());
		enterRule(_localctx, 44, RULE_typeOptionalIdentPair);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(313);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==CONST || _la==VAR) {
				{
				setState(312);
				typeQualifier();
				}
			}

			setState(315);
			type(0);
			setState(317);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==ID) {
				{
				setState(316);
				match(ID);
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TypeIdentPairContext extends ParserRuleContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TypeQualifierContext typeQualifier() {
			return getRuleContext(TypeQualifierContext.class,0);
		}
		public TypeIdentPairContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_typeIdentPair; }
	}

	public final TypeIdentPairContext typeIdentPair() throws RecognitionException {
		TypeIdentPairContext _localctx = new TypeIdentPairContext(_ctx, getState());
		enterRule(_localctx, 46, RULE_typeIdentPair);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(320);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==CONST || _la==VAR) {
				{
				setState(319);
				typeQualifier();
				}
			}

			setState(322);
			type(0);
			setState(323);
			match(ID);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FunctionParameterContext extends ParserRuleContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode CONST() { return getToken(GazpreaParser.CONST, 0); }
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public FunctionParameterContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_functionParameter; }
	}

	public final FunctionParameterContext functionParameter() throws RecognitionException {
		FunctionParameterContext _localctx = new FunctionParameterContext(_ctx, getState());
		enterRule(_localctx, 48, RULE_functionParameter);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(326);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==CONST) {
				{
				setState(325);
				match(CONST);
				}
			}

			setState(328);
			type(0);
			setState(330);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==ID) {
				{
				setState(329);
				match(ID);
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FunctionDeclrContext extends ParserRuleContext {
		public Token funcName;
		public TerminalNode FUNCTION() { return getToken(GazpreaParser.FUNCTION, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public TerminalNode RETURNS() { return getToken(GazpreaParser.RETURNS, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public List<FunctionParameterContext> functionParameter() {
			return getRuleContexts(FunctionParameterContext.class);
		}
		public FunctionParameterContext functionParameter(int i) {
			return getRuleContext(FunctionParameterContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public FunctionDeclrContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_functionDeclr; }
	}

	public final FunctionDeclrContext functionDeclr() throws RecognitionException {
		FunctionDeclrContext _localctx = new FunctionDeclrContext(_ctx, getState());
		enterRule(_localctx, 50, RULE_functionDeclr);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(332);
			match(FUNCTION);
			setState(333);
			((FunctionDeclrContext)_localctx).funcName = match(ID);
			setState(334);
			match(LPAREN);
			setState(343);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (((((_la - 39)) & ~0x3f) == 0 && ((1L << (_la - 39)) & ((1L << (BOOLEANA - 39)) | (1L << (CHARACTER - 39)) | (1L << (CONST - 39)) | (1L << (INTEGER - 39)) | (1L << (REAL - 39)) | (1L << (STRINGATOM - 39)) | (1L << (TUPLE - 39)) | (1L << (ID - 39)))) != 0)) {
				{
				setState(335);
				functionParameter();
				setState(340);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(336);
					match(COMMA);
					setState(337);
					functionParameter();
					}
					}
					setState(342);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(345);
			match(RPAREN);
			setState(346);
			match(RETURNS);
			setState(347);
			type(0);
			setState(348);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FunctionDefinitionContext extends ParserRuleContext {
		public Token funcName;
		public TerminalNode FUNCTION() { return getToken(GazpreaParser.FUNCTION, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public TerminalNode RETURNS() { return getToken(GazpreaParser.RETURNS, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public BlockContext block() {
			return getRuleContext(BlockContext.class,0);
		}
		public TerminalNode EQ() { return getToken(GazpreaParser.EQ, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public List<FunctionParameterContext> functionParameter() {
			return getRuleContexts(FunctionParameterContext.class);
		}
		public FunctionParameterContext functionParameter(int i) {
			return getRuleContext(FunctionParameterContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public FunctionDefinitionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_functionDefinition; }
	}

	public final FunctionDefinitionContext functionDefinition() throws RecognitionException {
		FunctionDefinitionContext _localctx = new FunctionDefinitionContext(_ctx, getState());
		enterRule(_localctx, 52, RULE_functionDefinition);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(350);
			match(FUNCTION);
			setState(351);
			((FunctionDefinitionContext)_localctx).funcName = match(ID);
			setState(352);
			match(LPAREN);
			setState(361);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (((((_la - 39)) & ~0x3f) == 0 && ((1L << (_la - 39)) & ((1L << (BOOLEANA - 39)) | (1L << (CHARACTER - 39)) | (1L << (CONST - 39)) | (1L << (INTEGER - 39)) | (1L << (REAL - 39)) | (1L << (STRINGATOM - 39)) | (1L << (TUPLE - 39)) | (1L << (ID - 39)))) != 0)) {
				{
				setState(353);
				functionParameter();
				setState(358);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(354);
					match(COMMA);
					setState(355);
					functionParameter();
					}
					}
					setState(360);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(363);
			match(RPAREN);
			setState(364);
			match(RETURNS);
			setState(365);
			type(0);
			setState(371);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case LBRACE:
				{
				setState(366);
				block();
				}
				break;
			case EQ:
				{
				setState(367);
				match(EQ);
				setState(368);
				expr(0);
				setState(369);
				match(SC);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ProcedureDeclrContext extends ParserRuleContext {
		public Token procName;
		public TerminalNode PROCEDURE() { return getToken(GazpreaParser.PROCEDURE, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public List<TypeOptionalIdentPairContext> typeOptionalIdentPair() {
			return getRuleContexts(TypeOptionalIdentPairContext.class);
		}
		public TypeOptionalIdentPairContext typeOptionalIdentPair(int i) {
			return getRuleContext(TypeOptionalIdentPairContext.class,i);
		}
		public TerminalNode RETURNS() { return getToken(GazpreaParser.RETURNS, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public ProcedureDeclrContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_procedureDeclr; }
	}

	public final ProcedureDeclrContext procedureDeclr() throws RecognitionException {
		ProcedureDeclrContext _localctx = new ProcedureDeclrContext(_ctx, getState());
		enterRule(_localctx, 54, RULE_procedureDeclr);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(373);
			match(PROCEDURE);
			setState(374);
			((ProcedureDeclrContext)_localctx).procName = match(ID);
			setState(375);
			match(LPAREN);
			setState(384);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (((((_la - 39)) & ~0x3f) == 0 && ((1L << (_la - 39)) & ((1L << (BOOLEANA - 39)) | (1L << (CHARACTER - 39)) | (1L << (CONST - 39)) | (1L << (INTEGER - 39)) | (1L << (REAL - 39)) | (1L << (STRINGATOM - 39)) | (1L << (TUPLE - 39)) | (1L << (VAR - 39)) | (1L << (ID - 39)))) != 0)) {
				{
				setState(376);
				typeOptionalIdentPair();
				setState(381);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(377);
					match(COMMA);
					setState(378);
					typeOptionalIdentPair();
					}
					}
					setState(383);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(386);
			match(RPAREN);
			setState(389);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==RETURNS) {
				{
				setState(387);
				match(RETURNS);
				setState(388);
				type(0);
				}
			}

			setState(391);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ProcedureDefinitionContext extends ParserRuleContext {
		public Token procName;
		public TerminalNode PROCEDURE() { return getToken(GazpreaParser.PROCEDURE, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public BlockContext block() {
			return getRuleContext(BlockContext.class,0);
		}
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public List<TypeIdentPairContext> typeIdentPair() {
			return getRuleContexts(TypeIdentPairContext.class);
		}
		public TypeIdentPairContext typeIdentPair(int i) {
			return getRuleContext(TypeIdentPairContext.class,i);
		}
		public TerminalNode RETURNS() { return getToken(GazpreaParser.RETURNS, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public ProcedureDefinitionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_procedureDefinition; }
	}

	public final ProcedureDefinitionContext procedureDefinition() throws RecognitionException {
		ProcedureDefinitionContext _localctx = new ProcedureDefinitionContext(_ctx, getState());
		enterRule(_localctx, 56, RULE_procedureDefinition);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(393);
			match(PROCEDURE);
			setState(394);
			((ProcedureDefinitionContext)_localctx).procName = match(ID);
			setState(395);
			match(LPAREN);
			setState(404);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (((((_la - 39)) & ~0x3f) == 0 && ((1L << (_la - 39)) & ((1L << (BOOLEANA - 39)) | (1L << (CHARACTER - 39)) | (1L << (CONST - 39)) | (1L << (INTEGER - 39)) | (1L << (REAL - 39)) | (1L << (STRINGATOM - 39)) | (1L << (TUPLE - 39)) | (1L << (VAR - 39)) | (1L << (ID - 39)))) != 0)) {
				{
				setState(396);
				typeIdentPair();
				setState(401);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(397);
					match(COMMA);
					setState(398);
					typeIdentPair();
					}
					}
					setState(403);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(406);
			match(RPAREN);
			setState(409);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==RETURNS) {
				{
				setState(407);
				match(RETURNS);
				setState(408);
				type(0);
				}
			}

			setState(411);
			block();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FunctionCallContext extends ParserRuleContext {
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public FunctionCallContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_functionCall; }
	}

	public final FunctionCallContext functionCall() throws RecognitionException {
		FunctionCallContext _localctx = new FunctionCallContext(_ctx, getState());
		enterRule(_localctx, 58, RULE_functionCall);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(413);
			match(ID);
			setState(414);
			match(LPAREN);
			setState(423);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << StringLit) | (1L << ExponentialLiteral1) | (1L << ExponentialLiteral2) | (1L << ExponentialLiteral3) | (1L << ExponentialLiteral4) | (1L << LPAREN) | (1L << LSQRPAREN) | (1L << PERIOD) | (1L << ADD) | (1L << SUB) | (1L << AS) | (1L << COLUMNS) | (1L << FALSE) | (1L << IDENTITY) | (1L << LENGTH) | (1L << NOT) | (1L << NULL_))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (REVERSE - 64)) | (1L << (ROWS - 64)) | (1L << (STRSTA - 64)) | (1L << (TRUE - 64)) | (1L << (INTLITERAL - 64)) | (1L << (ID - 64)) | (1L << (CHARLITERAL - 64)))) != 0)) {
				{
				setState(415);
				expr(0);
				setState(420);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(416);
					match(COMMA);
					setState(417);
					expr(0);
					}
					}
					setState(422);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(425);
			match(RPAREN);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ProcedureCallContext extends ParserRuleContext {
		public TerminalNode CALL() { return getToken(GazpreaParser.CALL, 0); }
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public TerminalNode SC() { return getToken(GazpreaParser.SC, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public ProcedureCallContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_procedureCall; }
	}

	public final ProcedureCallContext procedureCall() throws RecognitionException {
		ProcedureCallContext _localctx = new ProcedureCallContext(_ctx, getState());
		enterRule(_localctx, 60, RULE_procedureCall);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(427);
			match(CALL);
			setState(428);
			match(ID);
			setState(429);
			match(LPAREN);
			setState(438);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << StringLit) | (1L << ExponentialLiteral1) | (1L << ExponentialLiteral2) | (1L << ExponentialLiteral3) | (1L << ExponentialLiteral4) | (1L << LPAREN) | (1L << LSQRPAREN) | (1L << PERIOD) | (1L << ADD) | (1L << SUB) | (1L << AS) | (1L << COLUMNS) | (1L << FALSE) | (1L << IDENTITY) | (1L << LENGTH) | (1L << NOT) | (1L << NULL_))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (REVERSE - 64)) | (1L << (ROWS - 64)) | (1L << (STRSTA - 64)) | (1L << (TRUE - 64)) | (1L << (INTLITERAL - 64)) | (1L << (ID - 64)) | (1L << (CHARLITERAL - 64)))) != 0)) {
				{
				setState(430);
				expr(0);
				setState(435);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(431);
					match(COMMA);
					setState(432);
					expr(0);
					}
					}
					setState(437);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
			}

			setState(440);
			match(RPAREN);
			setState(441);
			match(SC);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class BlockContext extends ParserRuleContext {
		public TerminalNode LBRACE() { return getToken(GazpreaParser.LBRACE, 0); }
		public TerminalNode RBRACE() { return getToken(GazpreaParser.RBRACE, 0); }
		public List<StmtContext> stmt() {
			return getRuleContexts(StmtContext.class);
		}
		public StmtContext stmt(int i) {
			return getRuleContext(StmtContext.class,i);
		}
		public BlockContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_block; }
	}

	public final BlockContext block() throws RecognitionException {
		BlockContext _localctx = new BlockContext(_ctx, getState());
		enterRule(_localctx, 62, RULE_block);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(443);
			match(LBRACE);
			setState(447);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << StringLit) | (1L << ExponentialLiteral1) | (1L << ExponentialLiteral2) | (1L << ExponentialLiteral3) | (1L << ExponentialLiteral4) | (1L << LPAREN) | (1L << LSQRPAREN) | (1L << LBRACE) | (1L << PERIOD) | (1L << ADD) | (1L << SUB) | (1L << AS) | (1L << BOOLEANA) | (1L << BREAK) | (1L << CALL) | (1L << CHARACTER) | (1L << COLUMNS) | (1L << CONST) | (1L << CONTINUE) | (1L << FALSE) | (1L << IDENTITY) | (1L << IF) | (1L << INTEGER) | (1L << LENGTH) | (1L << LOOP) | (1L << NOT) | (1L << NULL_) | (1L << REAL) | (1L << RETURN))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (REVERSE - 64)) | (1L << (ROWS - 64)) | (1L << (STRSTA - 64)) | (1L << (STRINGATOM - 64)) | (1L << (TRUE - 64)) | (1L << (TUPLE - 64)) | (1L << (VAR - 64)) | (1L << (INTLITERAL - 64)) | (1L << (ID - 64)) | (1L << (CHARLITERAL - 64)))) != 0)) {
				{
				{
				setState(444);
				stmt();
				}
				}
				setState(449);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(450);
			match(RBRACE);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ExprContext extends ParserRuleContext {
		public ExprContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expr; }
	 
		public ExprContext() { }
		public void copyFrom(ExprContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class ExplicitCastContext extends ExprContext {
		public TerminalNode AS() { return getToken(GazpreaParser.AS, 0); }
		public TerminalNode LT() { return getToken(GazpreaParser.LT, 0); }
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode GT() { return getToken(GazpreaParser.GT, 0); }
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public ExplicitCastContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class BoolLiteralContext extends ExprContext {
		public TerminalNode TRUE() { return getToken(GazpreaParser.TRUE, 0); }
		public TerminalNode FALSE() { return getToken(GazpreaParser.FALSE, 0); }
		public BoolLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class VectorLiteralContext extends ExprContext {
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public VectorLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class UnaryExprContext extends ExprContext {
		public Token op;
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode ADD() { return getToken(GazpreaParser.ADD, 0); }
		public TerminalNode SUB() { return getToken(GazpreaParser.SUB, 0); }
		public TerminalNode NOT() { return getToken(GazpreaParser.NOT, 0); }
		public UnaryExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class GeneratorExprContext extends ExprContext {
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TerminalNode IN() { return getToken(GazpreaParser.IN, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode BAR() { return getToken(GazpreaParser.BAR, 0); }
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public GeneratorExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class ExpExprContext extends ExprContext {
		public Token op;
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode EXP() { return getToken(GazpreaParser.EXP, 0); }
		public ExpExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class CompExprContext extends ExprContext {
		public Token op;
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode LT() { return getToken(GazpreaParser.LT, 0); }
		public TerminalNode GT() { return getToken(GazpreaParser.GT, 0); }
		public TerminalNode LTEQ() { return getToken(GazpreaParser.LTEQ, 0); }
		public TerminalNode GTEQ() { return getToken(GazpreaParser.GTEQ, 0); }
		public CompExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class MulDivModDotProdExprContext extends ExprContext {
		public Token op;
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode MUL() { return getToken(GazpreaParser.MUL, 0); }
		public TerminalNode DIV() { return getToken(GazpreaParser.DIV, 0); }
		public TerminalNode MOD() { return getToken(GazpreaParser.MOD, 0); }
		public TerminalNode DOTPROD() { return getToken(GazpreaParser.DOTPROD, 0); }
		public MulDivModDotProdExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class IdentityLiteralContext extends ExprContext {
		public TerminalNode IDENTITY() { return getToken(GazpreaParser.IDENTITY, 0); }
		public IdentityLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class MemberAccessContext extends ExprContext {
		public List<TerminalNode> ID() { return getTokens(GazpreaParser.ID); }
		public TerminalNode ID(int i) {
			return getToken(GazpreaParser.ID, i);
		}
		public TerminalNode PERIOD() { return getToken(GazpreaParser.PERIOD, 0); }
		public TerminalNode INTLITERAL() { return getToken(GazpreaParser.INTLITERAL, 0); }
		public MemberAccessContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class IdentifierContext extends ExprContext {
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public IdentifierContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class NullLiteralContext extends ExprContext {
		public TerminalNode NULL_() { return getToken(GazpreaParser.NULL_, 0); }
		public NullLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class AddSubExprContext extends ExprContext {
		public Token op;
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode ADD() { return getToken(GazpreaParser.ADD, 0); }
		public TerminalNode SUB() { return getToken(GazpreaParser.SUB, 0); }
		public AddSubExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class BracketExprContext extends ExprContext {
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public ExprContext expr() {
			return getRuleContext(ExprContext.class,0);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public BracketExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class RealLiteralContext extends ExprContext {
		public RealLitContext realLit() {
			return getRuleContext(RealLitContext.class,0);
		}
		public RealLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class IntLiteralContext extends ExprContext {
		public TerminalNode INTLITERAL() { return getToken(GazpreaParser.INTLITERAL, 0); }
		public IntLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class ByExprContext extends ExprContext {
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode BY() { return getToken(GazpreaParser.BY, 0); }
		public ByExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class OrExprContext extends ExprContext {
		public Token op;
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode OR() { return getToken(GazpreaParser.OR, 0); }
		public TerminalNode XOR() { return getToken(GazpreaParser.XOR, 0); }
		public OrExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class FilterExprContext extends ExprContext {
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public TerminalNode ID() { return getToken(GazpreaParser.ID, 0); }
		public TerminalNode IN() { return getToken(GazpreaParser.IN, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode AND() { return getToken(GazpreaParser.AND, 0); }
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public FilterExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class CharLiteralContext extends ExprContext {
		public TerminalNode CHARLITERAL() { return getToken(GazpreaParser.CHARLITERAL, 0); }
		public CharLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class IndexExprContext extends ExprContext {
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public TerminalNode COMMA() { return getToken(GazpreaParser.COMMA, 0); }
		public IndexExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class TupleLiteralContext extends ExprContext {
		public TerminalNode LPAREN() { return getToken(GazpreaParser.LPAREN, 0); }
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public List<TerminalNode> COMMA() { return getTokens(GazpreaParser.COMMA); }
		public TerminalNode COMMA(int i) {
			return getToken(GazpreaParser.COMMA, i);
		}
		public TerminalNode RPAREN() { return getToken(GazpreaParser.RPAREN, 0); }
		public TupleLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class StringLiteralContext extends ExprContext {
		public TerminalNode StringLit() { return getToken(GazpreaParser.StringLit, 0); }
		public StringLiteralContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class AppendOpContext extends ExprContext {
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode APPENDOP() { return getToken(GazpreaParser.APPENDOP, 0); }
		public AppendOpContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class MatrixGeneratorExprContext extends ExprContext {
		public TerminalNode LSQRPAREN() { return getToken(GazpreaParser.LSQRPAREN, 0); }
		public List<TerminalNode> ID() { return getTokens(GazpreaParser.ID); }
		public TerminalNode ID(int i) {
			return getToken(GazpreaParser.ID, i);
		}
		public List<TerminalNode> IN() { return getTokens(GazpreaParser.IN); }
		public TerminalNode IN(int i) {
			return getToken(GazpreaParser.IN, i);
		}
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode COMMA() { return getToken(GazpreaParser.COMMA, 0); }
		public TerminalNode BAR() { return getToken(GazpreaParser.BAR, 0); }
		public TerminalNode RSQRPAREN() { return getToken(GazpreaParser.RSQRPAREN, 0); }
		public MatrixGeneratorExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class FuncCallContext extends ExprContext {
		public FunctionCallContext functionCall() {
			return getRuleContext(FunctionCallContext.class,0);
		}
		public FuncCallContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class BuiltInFuncCallContext extends ExprContext {
		public BuiltInCallContext builtInCall() {
			return getRuleContext(BuiltInCallContext.class,0);
		}
		public BuiltInFuncCallContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class RangeExprContext extends ExprContext {
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode DD() { return getToken(GazpreaParser.DD, 0); }
		public RangeExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class EqualExprContext extends ExprContext {
		public Token op;
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode EQEQ() { return getToken(GazpreaParser.EQEQ, 0); }
		public TerminalNode NEQ() { return getToken(GazpreaParser.NEQ, 0); }
		public EqualExprContext(ExprContext ctx) { copyFrom(ctx); }
	}
	public static class AndExprContext extends ExprContext {
		public Token op;
		public List<ExprContext> expr() {
			return getRuleContexts(ExprContext.class);
		}
		public ExprContext expr(int i) {
			return getRuleContext(ExprContext.class,i);
		}
		public TerminalNode ANDATOM() { return getToken(GazpreaParser.ANDATOM, 0); }
		public AndExprContext(ExprContext ctx) { copyFrom(ctx); }
	}

	public final ExprContext expr() throws RecognitionException {
		return expr(0);
	}

	private ExprContext expr(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		ExprContext _localctx = new ExprContext(_ctx, _parentState);
		ExprContext _prevctx = _localctx;
		int _startState = 64;
		enterRecursionRule(_localctx, 64, RULE_expr, _p);
		int _la;
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(548);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,49,_ctx) ) {
			case 1:
				{
				_localctx = new BracketExprContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;

				setState(453);
				match(LPAREN);
				setState(454);
				expr(0);
				setState(455);
				match(RPAREN);
				}
				break;
			case 2:
				{
				_localctx = new MemberAccessContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(457);
				match(ID);
				setState(458);
				match(PERIOD);
				setState(459);
				_la = _input.LA(1);
				if ( !(_la==INTLITERAL || _la==ID) ) {
				_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				}
				break;
			case 3:
				{
				_localctx = new UnaryExprContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(460);
				((UnaryExprContext)_localctx).op = _input.LT(1);
				_la = _input.LA(1);
				if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << ADD) | (1L << SUB) | (1L << NOT))) != 0)) ) {
					((UnaryExprContext)_localctx).op = (Token)_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				setState(461);
				expr(27);
				}
				break;
			case 4:
				{
				_localctx = new ExplicitCastContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(462);
				match(AS);
				setState(463);
				match(LT);
				setState(464);
				type(0);
				setState(465);
				match(GT);
				setState(466);
				match(LPAREN);
				setState(467);
				expr(0);
				setState(468);
				match(RPAREN);
				}
				break;
			case 5:
				{
				_localctx = new GeneratorExprContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(470);
				match(LSQRPAREN);
				setState(471);
				match(ID);
				setState(472);
				match(IN);
				setState(473);
				expr(0);
				setState(474);
				match(BAR);
				setState(475);
				expr(0);
				setState(476);
				match(RSQRPAREN);
				}
				break;
			case 6:
				{
				_localctx = new FilterExprContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(478);
				match(LSQRPAREN);
				setState(479);
				match(ID);
				setState(480);
				match(IN);
				setState(481);
				expr(0);
				setState(482);
				match(AND);
				setState(483);
				expr(0);
				setState(484);
				match(RSQRPAREN);
				}
				break;
			case 7:
				{
				_localctx = new MatrixGeneratorExprContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(486);
				match(LSQRPAREN);
				setState(487);
				match(ID);
				setState(488);
				match(IN);
				setState(489);
				expr(0);
				setState(490);
				match(COMMA);
				setState(491);
				match(ID);
				setState(492);
				match(IN);
				setState(493);
				expr(0);
				setState(494);
				match(BAR);
				setState(495);
				expr(0);
				setState(496);
				match(RSQRPAREN);
				}
				break;
			case 8:
				{
				_localctx = new FilterExprContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(498);
				match(LSQRPAREN);
				setState(499);
				match(ID);
				setState(500);
				match(IN);
				setState(501);
				expr(0);
				setState(502);
				match(AND);
				setState(503);
				expr(0);
				setState(508);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(504);
					match(COMMA);
					setState(505);
					expr(0);
					}
					}
					setState(510);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(511);
				match(RSQRPAREN);
				}
				break;
			case 9:
				{
				_localctx = new BuiltInFuncCallContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(513);
				builtInCall();
				}
				break;
			case 10:
				{
				_localctx = new FuncCallContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(514);
				functionCall();
				}
				break;
			case 11:
				{
				_localctx = new TupleLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(515);
				match(LPAREN);
				setState(516);
				expr(0);
				setState(517);
				match(COMMA);
				setState(518);
				expr(0);
				setState(523);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==COMMA) {
					{
					{
					setState(519);
					match(COMMA);
					setState(520);
					expr(0);
					}
					}
					setState(525);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(526);
				match(RPAREN);
				}
				break;
			case 12:
				{
				_localctx = new VectorLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(528);
				match(LSQRPAREN);
				setState(537);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << StringLit) | (1L << ExponentialLiteral1) | (1L << ExponentialLiteral2) | (1L << ExponentialLiteral3) | (1L << ExponentialLiteral4) | (1L << LPAREN) | (1L << LSQRPAREN) | (1L << PERIOD) | (1L << ADD) | (1L << SUB) | (1L << AS) | (1L << COLUMNS) | (1L << FALSE) | (1L << IDENTITY) | (1L << LENGTH) | (1L << NOT) | (1L << NULL_))) != 0) || ((((_la - 64)) & ~0x3f) == 0 && ((1L << (_la - 64)) & ((1L << (REVERSE - 64)) | (1L << (ROWS - 64)) | (1L << (STRSTA - 64)) | (1L << (TRUE - 64)) | (1L << (INTLITERAL - 64)) | (1L << (ID - 64)) | (1L << (CHARLITERAL - 64)))) != 0)) {
					{
					setState(529);
					expr(0);
					setState(534);
					_errHandler.sync(this);
					_la = _input.LA(1);
					while (_la==COMMA) {
						{
						{
						setState(530);
						match(COMMA);
						setState(531);
						expr(0);
						}
						}
						setState(536);
						_errHandler.sync(this);
						_la = _input.LA(1);
					}
					}
				}

				setState(539);
				match(RSQRPAREN);
				}
				break;
			case 13:
				{
				_localctx = new IdentifierContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(540);
				match(ID);
				}
				break;
			case 14:
				{
				_localctx = new NullLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(541);
				match(NULL_);
				}
				break;
			case 15:
				{
				_localctx = new IdentityLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(542);
				match(IDENTITY);
				}
				break;
			case 16:
				{
				_localctx = new BoolLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(543);
				_la = _input.LA(1);
				if ( !(_la==FALSE || _la==TRUE) ) {
				_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				}
				break;
			case 17:
				{
				_localctx = new IntLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(544);
				match(INTLITERAL);
				}
				break;
			case 18:
				{
				_localctx = new RealLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(545);
				realLit();
				}
				break;
			case 19:
				{
				_localctx = new CharLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(546);
				match(CHARLITERAL);
				}
				break;
			case 20:
				{
				_localctx = new StringLiteralContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(547);
				match(StringLit);
				}
				break;
			}
			_ctx.stop = _input.LT(-1);
			setState(591);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,52,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(589);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,51,_ctx) ) {
					case 1:
						{
						_localctx = new RangeExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(550);
						if (!(precpred(_ctx, 28))) throw new FailedPredicateException(this, "precpred(_ctx, 28)");
						setState(551);
						match(DD);
						setState(552);
						expr(29);
						}
						break;
					case 2:
						{
						_localctx = new ExpExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(553);
						if (!(precpred(_ctx, 26))) throw new FailedPredicateException(this, "precpred(_ctx, 26)");
						setState(554);
						((ExpExprContext)_localctx).op = match(EXP);
						setState(555);
						expr(26);
						}
						break;
					case 3:
						{
						_localctx = new MulDivModDotProdExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(556);
						if (!(precpred(_ctx, 25))) throw new FailedPredicateException(this, "precpred(_ctx, 25)");
						setState(557);
						((MulDivModDotProdExprContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << DIV) | (1L << MUL) | (1L << DOTPROD) | (1L << MOD))) != 0)) ) {
							((MulDivModDotProdExprContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(558);
						expr(26);
						}
						break;
					case 4:
						{
						_localctx = new AddSubExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(559);
						if (!(precpred(_ctx, 24))) throw new FailedPredicateException(this, "precpred(_ctx, 24)");
						setState(560);
						((AddSubExprContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==ADD || _la==SUB) ) {
							((AddSubExprContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(561);
						expr(25);
						}
						break;
					case 5:
						{
						_localctx = new ByExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(562);
						if (!(precpred(_ctx, 23))) throw new FailedPredicateException(this, "precpred(_ctx, 23)");
						setState(563);
						match(BY);
						setState(564);
						expr(24);
						}
						break;
					case 6:
						{
						_localctx = new CompExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(565);
						if (!(precpred(_ctx, 22))) throw new FailedPredicateException(this, "precpred(_ctx, 22)");
						setState(566);
						((CompExprContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << LTEQ) | (1L << GTEQ) | (1L << LT) | (1L << GT))) != 0)) ) {
							((CompExprContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(567);
						expr(23);
						}
						break;
					case 7:
						{
						_localctx = new EqualExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(568);
						if (!(precpred(_ctx, 21))) throw new FailedPredicateException(this, "precpred(_ctx, 21)");
						setState(569);
						((EqualExprContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==EQEQ || _la==NEQ) ) {
							((EqualExprContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(570);
						expr(22);
						}
						break;
					case 8:
						{
						_localctx = new AndExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(571);
						if (!(precpred(_ctx, 20))) throw new FailedPredicateException(this, "precpred(_ctx, 20)");
						setState(572);
						((AndExprContext)_localctx).op = match(ANDATOM);
						setState(573);
						expr(21);
						}
						break;
					case 9:
						{
						_localctx = new OrExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(574);
						if (!(precpred(_ctx, 19))) throw new FailedPredicateException(this, "precpred(_ctx, 19)");
						setState(575);
						((OrExprContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==OR || _la==XOR) ) {
							((OrExprContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(576);
						expr(20);
						}
						break;
					case 10:
						{
						_localctx = new AppendOpContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(577);
						if (!(precpred(_ctx, 18))) throw new FailedPredicateException(this, "precpred(_ctx, 18)");
						setState(578);
						match(APPENDOP);
						setState(579);
						expr(18);
						}
						break;
					case 11:
						{
						_localctx = new IndexExprContext(new ExprContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expr);
						setState(580);
						if (!(precpred(_ctx, 29))) throw new FailedPredicateException(this, "precpred(_ctx, 29)");
						setState(581);
						match(LSQRPAREN);
						setState(582);
						expr(0);
						setState(585);
						_errHandler.sync(this);
						_la = _input.LA(1);
						if (_la==COMMA) {
							{
							setState(583);
							match(COMMA);
							setState(584);
							expr(0);
							}
						}

						setState(587);
						match(RSQRPAREN);
						}
						break;
					}
					} 
				}
				setState(593);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,52,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public static class RealLitContext extends ParserRuleContext {
		public RealLitContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_realLit; }
	 
		public RealLitContext() { }
		public void copyFrom(RealLitContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class RealLit7Context extends RealLitContext {
		public List<TerminalNode> INTLITERAL() { return getTokens(GazpreaParser.INTLITERAL); }
		public TerminalNode INTLITERAL(int i) {
			return getToken(GazpreaParser.INTLITERAL, i);
		}
		public TerminalNode PERIOD() { return getToken(GazpreaParser.PERIOD, 0); }
		public RealLit7Context(RealLitContext ctx) { copyFrom(ctx); }
	}
	public static class RealLit6Context extends RealLitContext {
		public TerminalNode PERIOD() { return getToken(GazpreaParser.PERIOD, 0); }
		public TerminalNode INTLITERAL() { return getToken(GazpreaParser.INTLITERAL, 0); }
		public RealLit6Context(RealLitContext ctx) { copyFrom(ctx); }
	}
	public static class RealLit5Context extends RealLitContext {
		public TerminalNode INTLITERAL() { return getToken(GazpreaParser.INTLITERAL, 0); }
		public TerminalNode PERIOD() { return getToken(GazpreaParser.PERIOD, 0); }
		public RealLit5Context(RealLitContext ctx) { copyFrom(ctx); }
	}
	public static class RealLit4Context extends RealLitContext {
		public TerminalNode ExponentialLiteral4() { return getToken(GazpreaParser.ExponentialLiteral4, 0); }
		public RealLit4Context(RealLitContext ctx) { copyFrom(ctx); }
	}
	public static class RealLit3Context extends RealLitContext {
		public TerminalNode ExponentialLiteral3() { return getToken(GazpreaParser.ExponentialLiteral3, 0); }
		public RealLit3Context(RealLitContext ctx) { copyFrom(ctx); }
	}
	public static class RealLit2Context extends RealLitContext {
		public TerminalNode ExponentialLiteral2() { return getToken(GazpreaParser.ExponentialLiteral2, 0); }
		public RealLit2Context(RealLitContext ctx) { copyFrom(ctx); }
	}
	public static class RealLit1Context extends RealLitContext {
		public TerminalNode ExponentialLiteral1() { return getToken(GazpreaParser.ExponentialLiteral1, 0); }
		public RealLit1Context(RealLitContext ctx) { copyFrom(ctx); }
	}

	public final RealLitContext realLit() throws RecognitionException {
		RealLitContext _localctx = new RealLitContext(_ctx, getState());
		enterRule(_localctx, 66, RULE_realLit);
		try {
			setState(605);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,53,_ctx) ) {
			case 1:
				_localctx = new RealLit1Context(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(594);
				match(ExponentialLiteral1);
				}
				break;
			case 2:
				_localctx = new RealLit2Context(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(595);
				match(ExponentialLiteral2);
				}
				break;
			case 3:
				_localctx = new RealLit3Context(_localctx);
				enterOuterAlt(_localctx, 3);
				{
				setState(596);
				match(ExponentialLiteral3);
				}
				break;
			case 4:
				_localctx = new RealLit4Context(_localctx);
				enterOuterAlt(_localctx, 4);
				{
				setState(597);
				match(ExponentialLiteral4);
				}
				break;
			case 5:
				_localctx = new RealLit5Context(_localctx);
				enterOuterAlt(_localctx, 5);
				{
				setState(598);
				match(INTLITERAL);
				setState(599);
				match(PERIOD);
				}
				break;
			case 6:
				_localctx = new RealLit6Context(_localctx);
				enterOuterAlt(_localctx, 6);
				{
				setState(600);
				match(PERIOD);
				setState(601);
				match(INTLITERAL);
				}
				break;
			case 7:
				_localctx = new RealLit7Context(_localctx);
				enterOuterAlt(_localctx, 7);
				{
				setState(602);
				match(INTLITERAL);
				setState(603);
				match(PERIOD);
				setState(604);
				match(INTLITERAL);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 18:
			return type_sempred((TypeContext)_localctx, predIndex);
		case 32:
			return expr_sempred((ExprContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean type_sempred(TypeContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return precpred(_ctx, 9);
		case 1:
			return precpred(_ctx, 8);
		case 2:
			return precpred(_ctx, 7);
		}
		return true;
	}
	private boolean expr_sempred(ExprContext _localctx, int predIndex) {
		switch (predIndex) {
		case 3:
			return precpred(_ctx, 28);
		case 4:
			return precpred(_ctx, 26);
		case 5:
			return precpred(_ctx, 25);
		case 6:
			return precpred(_ctx, 24);
		case 7:
			return precpred(_ctx, 23);
		case 8:
			return precpred(_ctx, 22);
		case 9:
			return precpred(_ctx, 21);
		case 10:
			return precpred(_ctx, 20);
		case 11:
			return precpred(_ctx, 19);
		case 12:
			return precpred(_ctx, 18);
		case 13:
			return precpred(_ctx, 29);
		}
		return true;
	}

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\3S\u0262\4\2\t\2\4"+
		"\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13\t"+
		"\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31\t\31"+
		"\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\4 \t \4!"+
		"\t!\4\"\t\"\4#\t#\3\2\7\2H\n\2\f\2\16\2K\13\2\3\2\3\2\3\3\3\3\3\3\3\3"+
		"\3\3\3\3\5\3U\n\3\3\4\3\4\5\4Y\n\4\3\4\3\4\3\4\3\4\3\4\3\5\3\5\5\5b\n"+
		"\5\3\6\3\6\3\6\3\6\3\6\3\6\3\6\3\6\3\6\3\6\5\6n\n\6\3\7\3\7\3\7\3\b\3"+
		"\b\3\b\3\t\3\t\5\tx\n\t\3\t\3\t\3\t\5\t}\n\t\3\t\3\t\3\t\5\t\u0082\n\t"+
		"\3\t\3\t\3\t\3\t\5\t\u0088\n\t\3\t\3\t\5\t\u008c\n\t\3\n\3\n\3\n\3\n\3"+
		"\n\3\13\3\13\3\13\3\13\3\13\5\13\u0098\n\13\3\13\3\13\3\13\3\13\3\13\3"+
		"\13\3\13\3\13\3\13\3\13\7\13\u00a4\n\13\f\13\16\13\u00a7\13\13\3\13\5"+
		"\13\u00aa\n\13\3\f\3\f\3\f\3\f\3\f\3\f\3\f\3\f\3\f\3\f\5\f\u00b6\n\f\3"+
		"\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r\3\r"+
		"\3\r\3\r\3\r\3\r\7\r\u00ce\n\r\f\r\16\r\u00d1\13\r\3\r\3\r\5\r\u00d5\n"+
		"\r\3\16\3\16\3\16\3\16\3\16\3\17\3\17\3\17\3\17\3\17\3\20\3\20\3\20\3"+
		"\20\3\20\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3"+
		"\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\5\21\u00fe"+
		"\n\21\3\22\3\22\5\22\u0102\n\22\3\22\3\22\3\23\3\23\3\24\3\24\3\24\3\24"+
		"\3\24\3\24\3\24\3\24\5\24\u0110\n\24\3\24\3\24\3\24\3\24\3\24\3\24\3\24"+
		"\3\24\3\24\3\24\3\24\3\24\3\24\3\24\7\24\u0120\n\24\f\24\16\24\u0123\13"+
		"\24\3\25\3\25\5\25\u0127\n\25\3\26\3\26\3\26\3\26\3\26\3\26\3\26\7\26"+
		"\u0130\n\26\f\26\16\26\u0133\13\26\3\26\3\26\3\27\3\27\5\27\u0139\n\27"+
		"\3\30\5\30\u013c\n\30\3\30\3\30\5\30\u0140\n\30\3\31\5\31\u0143\n\31\3"+
		"\31\3\31\3\31\3\32\5\32\u0149\n\32\3\32\3\32\5\32\u014d\n\32\3\33\3\33"+
		"\3\33\3\33\3\33\3\33\7\33\u0155\n\33\f\33\16\33\u0158\13\33\5\33\u015a"+
		"\n\33\3\33\3\33\3\33\3\33\3\33\3\34\3\34\3\34\3\34\3\34\3\34\7\34\u0167"+
		"\n\34\f\34\16\34\u016a\13\34\5\34\u016c\n\34\3\34\3\34\3\34\3\34\3\34"+
		"\3\34\3\34\3\34\5\34\u0176\n\34\3\35\3\35\3\35\3\35\3\35\3\35\7\35\u017e"+
		"\n\35\f\35\16\35\u0181\13\35\5\35\u0183\n\35\3\35\3\35\3\35\5\35\u0188"+
		"\n\35\3\35\3\35\3\36\3\36\3\36\3\36\3\36\3\36\7\36\u0192\n\36\f\36\16"+
		"\36\u0195\13\36\5\36\u0197\n\36\3\36\3\36\3\36\5\36\u019c\n\36\3\36\3"+
		"\36\3\37\3\37\3\37\3\37\3\37\7\37\u01a5\n\37\f\37\16\37\u01a8\13\37\5"+
		"\37\u01aa\n\37\3\37\3\37\3 \3 \3 \3 \3 \3 \7 \u01b4\n \f \16 \u01b7\13"+
		" \5 \u01b9\n \3 \3 \3 \3!\3!\7!\u01c0\n!\f!\16!\u01c3\13!\3!\3!\3\"\3"+
		"\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\""+
		"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3"+
		"\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\""+
		"\7\"\u01fd\n\"\f\"\16\"\u0200\13\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\""+
		"\3\"\7\"\u020c\n\"\f\"\16\"\u020f\13\"\3\"\3\"\3\"\3\"\3\"\3\"\7\"\u0217"+
		"\n\"\f\"\16\"\u021a\13\"\5\"\u021c\n\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\""+
		"\3\"\5\"\u0227\n\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\""+
		"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3\"\3"+
		"\"\3\"\3\"\3\"\3\"\5\"\u024c\n\"\3\"\3\"\7\"\u0250\n\"\f\"\16\"\u0253"+
		"\13\"\3#\3#\3#\3#\3#\3#\3#\3#\3#\3#\3#\5#\u0260\n#\3#\2\4&B$\2\4\6\b\n"+
		"\f\16\20\22\24\26\30\32\34\36 \"$&(*,.\60\62\64\668:<>@BD\2\13\3\2NO\4"+
		"\2//KK\4\2\32\33;;\4\2\62\62HH\4\2\34\36&&\3\2\32\33\3\2\37\"\3\2#$\4"+
		"\2==MM\2\u02ae\2I\3\2\2\2\4T\3\2\2\2\6V\3\2\2\2\ba\3\2\2\2\nm\3\2\2\2"+
		"\fo\3\2\2\2\16r\3\2\2\2\20\u008b\3\2\2\2\22\u008d\3\2\2\2\24\u00a9\3\2"+
		"\2\2\26\u00b5\3\2\2\2\30\u00d4\3\2\2\2\32\u00d6\3\2\2\2\34\u00db\3\2\2"+
		"\2\36\u00e0\3\2\2\2 \u00fd\3\2\2\2\"\u00ff\3\2\2\2$\u0105\3\2\2\2&\u010f"+
		"\3\2\2\2(\u0126\3\2\2\2*\u0128\3\2\2\2,\u0136\3\2\2\2.\u013b\3\2\2\2\60"+
		"\u0142\3\2\2\2\62\u0148\3\2\2\2\64\u014e\3\2\2\2\66\u0160\3\2\2\28\u0177"+
		"\3\2\2\2:\u018b\3\2\2\2<\u019f\3\2\2\2>\u01ad\3\2\2\2@\u01bd\3\2\2\2B"+
		"\u0226\3\2\2\2D\u025f\3\2\2\2FH\5\4\3\2GF\3\2\2\2HK\3\2\2\2IG\3\2\2\2"+
		"IJ\3\2\2\2JL\3\2\2\2KI\3\2\2\2LM\7\2\2\3M\3\3\2\2\2NU\5\6\4\2OU\5\64\33"+
		"\2PU\5\66\34\2QU\58\35\2RU\5:\36\2SU\5\32\16\2TN\3\2\2\2TO\3\2\2\2TP\3"+
		"\2\2\2TQ\3\2\2\2TR\3\2\2\2TS\3\2\2\2U\5\3\2\2\2VX\7/\2\2WY\5&\24\2XW\3"+
		"\2\2\2XY\3\2\2\2YZ\3\2\2\2Z[\7O\2\2[\\\7\21\2\2\\]\5B\"\2]^\7\20\2\2^"+
		"\7\3\2\2\2_b\5\n\6\2`b\5@!\2a_\3\2\2\2a`\3\2\2\2b\t\3\2\2\2cn\5\20\t\2"+
		"dn\5\22\n\2en\5\26\f\2fn\5\30\r\2gn\5\34\17\2hn\5\36\20\2in\5\f\7\2jn"+
		"\5\16\b\2kn\5\"\22\2ln\5> \2mc\3\2\2\2md\3\2\2\2me\3\2\2\2mf\3\2\2\2m"+
		"g\3\2\2\2mh\3\2\2\2mi\3\2\2\2mj\3\2\2\2mk\3\2\2\2ml\3\2\2\2n\13\3\2\2"+
		"\2op\7*\2\2pq\7\20\2\2q\r\3\2\2\2rs\7\60\2\2st\7\20\2\2t\17\3\2\2\2uw"+
		"\5$\23\2vx\5&\24\2wv\3\2\2\2wx\3\2\2\2xy\3\2\2\2y|\7O\2\2z{\7\21\2\2{"+
		"}\5B\"\2|z\3\2\2\2|}\3\2\2\2}~\3\2\2\2~\177\7\20\2\2\177\u008c\3\2\2\2"+
		"\u0080\u0082\5$\23\2\u0081\u0080\3\2\2\2\u0081\u0082\3\2\2\2\u0082\u0083"+
		"\3\2\2\2\u0083\u0084\5&\24\2\u0084\u0087\7O\2\2\u0085\u0086\7\21\2\2\u0086"+
		"\u0088\5B\"\2\u0087\u0085\3\2\2\2\u0087\u0088\3\2\2\2\u0088\u0089\3\2"+
		"\2\2\u0089\u008a\7\20\2\2\u008a\u008c\3\2\2\2\u008bu\3\2\2\2\u008b\u0081"+
		"\3\2\2\2\u008c\21\3\2\2\2\u008d\u008e\5\24\13\2\u008e\u008f\7\21\2\2\u008f"+
		"\u0090\5B\"\2\u0090\u0091\7\20\2\2\u0091\23\3\2\2\2\u0092\u0093\5B\"\2"+
		"\u0093\u0094\7\n\2\2\u0094\u0097\5B\"\2\u0095\u0096\7\30\2\2\u0096\u0098"+
		"\5B\"\2\u0097\u0095\3\2\2\2\u0097\u0098\3\2\2\2\u0098\u0099\3\2\2\2\u0099"+
		"\u009a\7\13\2\2\u009a\u00aa\3\2\2\2\u009b\u009c\7O\2\2\u009c\u009d\7\23"+
		"\2\2\u009d\u00aa\t\2\2\2\u009e\u009f\7O\2\2\u009f\u00a0\7\30\2\2\u00a0"+
		"\u00a5\7O\2\2\u00a1\u00a2\7\30\2\2\u00a2\u00a4\7O\2\2\u00a3\u00a1\3\2"+
		"\2\2\u00a4\u00a7\3\2\2\2\u00a5\u00a3\3\2\2\2\u00a5\u00a6\3\2\2\2\u00a6"+
		"\u00aa\3\2\2\2\u00a7\u00a5\3\2\2\2\u00a8\u00aa\7O\2\2\u00a9\u0092\3\2"+
		"\2\2\u00a9\u009b\3\2\2\2\u00a9\u009e\3\2\2\2\u00a9\u00a8\3\2\2\2\u00aa"+
		"\25\3\2\2\2\u00ab\u00ac\7\65\2\2\u00ac\u00ad\5B\"\2\u00ad\u00ae\5\b\5"+
		"\2\u00ae\u00b6\3\2\2\2\u00af\u00b0\7\65\2\2\u00b0\u00b1\5B\"\2\u00b1\u00b2"+
		"\5\b\5\2\u00b2\u00b3\7\61\2\2\u00b3\u00b4\5\b\5\2\u00b4\u00b6\3\2\2\2"+
		"\u00b5\u00ab\3\2\2\2\u00b5\u00af\3\2\2\2\u00b6\27\3\2\2\2\u00b7\u00b8"+
		"\7:\2\2\u00b8\u00d5\5\b\5\2\u00b9\u00ba\7:\2\2\u00ba\u00bb\7L\2\2\u00bb"+
		"\u00bc\5B\"\2\u00bc\u00bd\5\b\5\2\u00bd\u00d5\3\2\2\2\u00be\u00bf\7:\2"+
		"\2\u00bf\u00c0\5\b\5\2\u00c0\u00c1\7L\2\2\u00c1\u00c2\5B\"\2\u00c2\u00c3"+
		"\7\20\2\2\u00c3\u00d5\3\2\2\2\u00c4\u00c5\7:\2\2\u00c5\u00c6\7O\2\2\u00c6"+
		"\u00c7\7\66\2\2\u00c7\u00c8\5B\"\2\u00c8\u00cf\3\2\2\2\u00c9\u00ca\7\30"+
		"\2\2\u00ca\u00cb\7O\2\2\u00cb\u00cc\7\66\2\2\u00cc\u00ce\5B\"\2\u00cd"+
		"\u00c9\3\2\2\2\u00ce\u00d1\3\2\2\2\u00cf\u00cd\3\2\2\2\u00cf\u00d0\3\2"+
		"\2\2\u00d0\u00d2\3\2\2\2\u00d1\u00cf\3\2\2\2\u00d2\u00d3\5\b\5\2\u00d3"+
		"\u00d5\3\2\2\2\u00d4\u00b7\3\2\2\2\u00d4\u00b9\3\2\2\2\u00d4\u00be\3\2"+
		"\2\2\u00d4\u00c4\3\2\2\2\u00d5\31\3\2\2\2\u00d6\u00d7\7J\2\2\u00d7\u00d8"+
		"\5&\24\2\u00d8\u00d9\7O\2\2\u00d9\u00da\7\20\2\2\u00da\33\3\2\2\2\u00db"+
		"\u00dc\5B\"\2\u00dc\u00dd\7\25\2\2\u00dd\u00de\7E\2\2\u00de\u00df\7\20"+
		"\2\2\u00df\35\3\2\2\2\u00e0\u00e1\5\24\13\2\u00e1\u00e2\7\26\2\2\u00e2"+
		"\u00e3\7D\2\2\u00e3\u00e4\7\20\2\2\u00e4\37\3\2\2\2\u00e5\u00e6\79\2\2"+
		"\u00e6\u00e7\7\b\2\2\u00e7\u00e8\5B\"\2\u00e8\u00e9\7\t\2\2\u00e9\u00fe"+
		"\3\2\2\2\u00ea\u00eb\7C\2\2\u00eb\u00ec\7\b\2\2\u00ec\u00ed\5B\"\2\u00ed"+
		"\u00ee\7\t\2\2\u00ee\u00fe\3\2\2\2\u00ef\u00f0\7.\2\2\u00f0\u00f1\7\b"+
		"\2\2\u00f1\u00f2\5B\"\2\u00f2\u00f3\7\t\2\2\u00f3\u00fe\3\2\2\2\u00f4"+
		"\u00f5\7B\2\2\u00f5\u00f6\7\b\2\2\u00f6\u00f7\5B\"\2\u00f7\u00f8\7\t\2"+
		"\2\u00f8\u00fe\3\2\2\2\u00f9\u00fa\7F\2\2\u00fa\u00fb\7\b\2\2\u00fb\u00fc"+
		"\7D\2\2\u00fc\u00fe\7\t\2\2\u00fd\u00e5\3\2\2\2\u00fd\u00ea\3\2\2\2\u00fd"+
		"\u00ef\3\2\2\2\u00fd\u00f4\3\2\2\2\u00fd\u00f9\3\2\2\2\u00fe!\3\2\2\2"+
		"\u00ff\u0101\7@\2\2\u0100\u0102\5B\"\2\u0101\u0100\3\2\2\2\u0101\u0102"+
		"\3\2\2\2\u0102\u0103\3\2\2\2\u0103\u0104\7\20\2\2\u0104#\3\2\2\2\u0105"+
		"\u0106\t\3\2\2\u0106%\3\2\2\2\u0107\u0108\b\24\1\2\u0108\u0110\5*\26\2"+
		"\u0109\u0110\7\67\2\2\u010a\u0110\7-\2\2\u010b\u0110\7)\2\2\u010c\u0110"+
		"\7?\2\2\u010d\u0110\7O\2\2\u010e\u0110\7G\2\2\u010f\u0107\3\2\2\2\u010f"+
		"\u0109\3\2\2\2\u010f\u010a\3\2\2\2\u010f\u010b\3\2\2\2\u010f\u010c\3\2"+
		"\2\2\u010f\u010d\3\2\2\2\u010f\u010e\3\2\2\2\u0110\u0121\3\2\2\2\u0111"+
		"\u0112\f\13\2\2\u0112\u0113\7\n\2\2\u0113\u0114\5(\25\2\u0114\u0115\7"+
		"\13\2\2\u0115\u0120\3\2\2\2\u0116\u0117\f\n\2\2\u0117\u0118\7\n\2\2\u0118"+
		"\u0119\5(\25\2\u0119\u011a\7\30\2\2\u011a\u011b\5(\25\2\u011b\u011c\7"+
		"\13\2\2\u011c\u0120\3\2\2\2\u011d\u011e\f\t\2\2\u011e\u0120\78\2\2\u011f"+
		"\u0111\3\2\2\2\u011f\u0116\3\2\2\2\u011f\u011d\3\2\2\2\u0120\u0123\3\2"+
		"\2\2\u0121\u011f\3\2\2\2\u0121\u0122\3\2\2\2\u0122\'\3\2\2\2\u0123\u0121"+
		"\3\2\2\2\u0124\u0127\7\35\2\2\u0125\u0127\5B\"\2\u0126\u0124\3\2\2\2\u0126"+
		"\u0125\3\2\2\2\u0127)\3\2\2\2\u0128\u0129\7I\2\2\u0129\u012a\7\b\2\2\u012a"+
		"\u012b\5,\27\2\u012b\u012c\7\30\2\2\u012c\u0131\5,\27\2\u012d\u012e\7"+
		"\30\2\2\u012e\u0130\5,\27\2\u012f\u012d\3\2\2\2\u0130\u0133\3\2\2\2\u0131"+
		"\u012f\3\2\2\2\u0131\u0132\3\2\2\2\u0132\u0134\3\2\2\2\u0133\u0131\3\2"+
		"\2\2\u0134\u0135\7\t\2\2\u0135+\3\2\2\2\u0136\u0138\5&\24\2\u0137\u0139"+
		"\7O\2\2\u0138\u0137\3\2\2\2\u0138\u0139\3\2\2\2\u0139-\3\2\2\2\u013a\u013c"+
		"\5$\23\2\u013b\u013a\3\2\2\2\u013b\u013c\3\2\2\2\u013c\u013d\3\2\2\2\u013d"+
		"\u013f\5&\24\2\u013e\u0140\7O\2\2\u013f\u013e\3\2\2\2\u013f\u0140\3\2"+
		"\2\2\u0140/\3\2\2\2\u0141\u0143\5$\23\2\u0142\u0141\3\2\2\2\u0142\u0143"+
		"\3\2\2\2\u0143\u0144\3\2\2\2\u0144\u0145\5&\24\2\u0145\u0146\7O\2\2\u0146"+
		"\61\3\2\2\2\u0147\u0149\7/\2\2\u0148\u0147\3\2\2\2\u0148\u0149\3\2\2\2"+
		"\u0149\u014a\3\2\2\2\u014a\u014c\5&\24\2\u014b\u014d\7O\2\2\u014c\u014b"+
		"\3\2\2\2\u014c\u014d\3\2\2\2\u014d\63\3\2\2\2\u014e\u014f\7\63\2\2\u014f"+
		"\u0150\7O\2\2\u0150\u0159\7\b\2\2\u0151\u0156\5\62\32\2\u0152\u0153\7"+
		"\30\2\2\u0153\u0155\5\62\32\2\u0154\u0152\3\2\2\2\u0155\u0158\3\2\2\2"+
		"\u0156\u0154\3\2\2\2\u0156\u0157\3\2\2\2\u0157\u015a\3\2\2\2\u0158\u0156"+
		"\3\2\2\2\u0159\u0151\3\2\2\2\u0159\u015a\3\2\2\2\u015a\u015b\3\2\2\2\u015b"+
		"\u015c\7\t\2\2\u015c\u015d\7A\2\2\u015d\u015e\5&\24\2\u015e\u015f\7\20"+
		"\2\2\u015f\65\3\2\2\2\u0160\u0161\7\63\2\2\u0161\u0162\7O\2\2\u0162\u016b"+
		"\7\b\2\2\u0163\u0168\5\62\32\2\u0164\u0165\7\30\2\2\u0165\u0167\5\62\32"+
		"\2\u0166\u0164\3\2\2\2\u0167\u016a\3\2\2\2\u0168\u0166\3\2\2\2\u0168\u0169"+
		"\3\2\2\2\u0169\u016c\3\2\2\2\u016a\u0168\3\2\2\2\u016b\u0163\3\2\2\2\u016b"+
		"\u016c\3\2\2\2\u016c\u016d\3\2\2\2\u016d\u016e\7\t\2\2\u016e\u016f\7A"+
		"\2\2\u016f\u0175\5&\24\2\u0170\u0176\5@!\2\u0171\u0172\7\21\2\2\u0172"+
		"\u0173\5B\"\2\u0173\u0174\7\20\2\2\u0174\u0176\3\2\2\2\u0175\u0170\3\2"+
		"\2\2\u0175\u0171\3\2\2\2\u0176\67\3\2\2\2\u0177\u0178\7>\2\2\u0178\u0179"+
		"\7O\2\2\u0179\u0182\7\b\2\2\u017a\u017f\5.\30\2\u017b\u017c\7\30\2\2\u017c"+
		"\u017e\5.\30\2\u017d\u017b\3\2\2\2\u017e\u0181\3\2\2\2\u017f\u017d\3\2"+
		"\2\2\u017f\u0180\3\2\2\2\u0180\u0183\3\2\2\2\u0181\u017f\3\2\2\2\u0182"+
		"\u017a\3\2\2\2\u0182\u0183\3\2\2\2\u0183\u0184\3\2\2\2\u0184\u0187\7\t"+
		"\2\2\u0185\u0186\7A\2\2\u0186\u0188\5&\24\2\u0187\u0185\3\2\2\2\u0187"+
		"\u0188\3\2\2\2\u0188\u0189\3\2\2\2\u0189\u018a\7\20\2\2\u018a9\3\2\2\2"+
		"\u018b\u018c\7>\2\2\u018c\u018d\7O\2\2\u018d\u0196\7\b\2\2\u018e\u0193"+
		"\5\60\31\2\u018f\u0190\7\30\2\2\u0190\u0192\5\60\31\2\u0191\u018f\3\2"+
		"\2\2\u0192\u0195\3\2\2\2\u0193\u0191\3\2\2\2\u0193\u0194\3\2\2\2\u0194"+
		"\u0197\3\2\2\2\u0195\u0193\3\2\2\2\u0196\u018e\3\2\2\2\u0196\u0197\3\2"+
		"\2\2\u0197\u0198\3\2\2\2\u0198\u019b\7\t\2\2\u0199\u019a\7A\2\2\u019a"+
		"\u019c\5&\24\2\u019b\u0199\3\2\2\2\u019b\u019c\3\2\2\2\u019c\u019d\3\2"+
		"\2\2\u019d\u019e\5@!\2\u019e;\3\2\2\2\u019f\u01a0\7O\2\2\u01a0\u01a9\7"+
		"\b\2\2\u01a1\u01a6\5B\"\2\u01a2\u01a3\7\30\2\2\u01a3\u01a5\5B\"\2\u01a4"+
		"\u01a2\3\2\2\2\u01a5\u01a8\3\2\2\2\u01a6\u01a4\3\2\2\2\u01a6\u01a7\3\2"+
		"\2\2\u01a7\u01aa\3\2\2\2\u01a8\u01a6\3\2\2\2\u01a9\u01a1\3\2\2\2\u01a9"+
		"\u01aa\3\2\2\2\u01aa\u01ab\3\2\2\2\u01ab\u01ac\7\t\2\2\u01ac=\3\2\2\2"+
		"\u01ad\u01ae\7,\2\2\u01ae\u01af\7O\2\2\u01af\u01b8\7\b\2\2\u01b0\u01b5"+
		"\5B\"\2\u01b1\u01b2\7\30\2\2\u01b2\u01b4\5B\"\2\u01b3\u01b1\3\2\2\2\u01b4"+
		"\u01b7\3\2\2\2\u01b5\u01b3\3\2\2\2\u01b5\u01b6\3\2\2\2\u01b6\u01b9\3\2"+
		"\2\2\u01b7\u01b5\3\2\2\2\u01b8\u01b0\3\2\2\2\u01b8\u01b9\3\2\2\2\u01b9"+
		"\u01ba\3\2\2\2\u01ba\u01bb\7\t\2\2\u01bb\u01bc\7\20\2\2\u01bc?\3\2\2\2"+
		"\u01bd\u01c1\7\f\2\2\u01be\u01c0\5\b\5\2\u01bf\u01be\3\2\2\2\u01c0\u01c3"+
		"\3\2\2\2\u01c1\u01bf\3\2\2\2\u01c1\u01c2\3\2\2\2\u01c2\u01c4\3\2\2\2\u01c3"+
		"\u01c1\3\2\2\2\u01c4\u01c5\7\r\2\2\u01c5A\3\2\2\2\u01c6\u01c7\b\"\1\2"+
		"\u01c7\u01c8\7\b\2\2\u01c8\u01c9\5B\"\2\u01c9\u01ca\7\t\2\2\u01ca\u0227"+
		"\3\2\2\2\u01cb\u01cc\7O\2\2\u01cc\u01cd\7\23\2\2\u01cd\u0227\t\2\2\2\u01ce"+
		"\u01cf\t\4\2\2\u01cf\u0227\5B\"\35\u01d0\u01d1\7(\2\2\u01d1\u01d2\7!\2"+
		"\2\u01d2\u01d3\5&\24\2\u01d3\u01d4\7\"\2\2\u01d4\u01d5\7\b\2\2\u01d5\u01d6"+
		"\5B\"\2\u01d6\u01d7\7\t\2\2\u01d7\u0227\3\2\2\2\u01d8\u01d9\7\n\2\2\u01d9"+
		"\u01da\7O\2\2\u01da\u01db\7\66\2\2\u01db\u01dc\5B\"\2\u01dc\u01dd\7\16"+
		"\2\2\u01dd\u01de\5B\"\2\u01de\u01df\7\13\2\2\u01df\u0227\3\2\2\2\u01e0"+
		"\u01e1\7\n\2\2\u01e1\u01e2\7O\2\2\u01e2\u01e3\7\66\2\2\u01e3\u01e4\5B"+
		"\"\2\u01e4\u01e5\7\24\2\2\u01e5\u01e6\5B\"\2\u01e6\u01e7\7\13\2\2\u01e7"+
		"\u0227\3\2\2\2\u01e8\u01e9\7\n\2\2\u01e9\u01ea\7O\2\2\u01ea\u01eb\7\66"+
		"\2\2\u01eb\u01ec\5B\"\2\u01ec\u01ed\7\30\2\2\u01ed\u01ee\7O\2\2\u01ee"+
		"\u01ef\7\66\2\2\u01ef\u01f0\5B\"\2\u01f0\u01f1\7\16\2\2\u01f1\u01f2\5"+
		"B\"\2\u01f2\u01f3\7\13\2\2\u01f3\u0227\3\2\2\2\u01f4\u01f5\7\n\2\2\u01f5"+
		"\u01f6\7O\2\2\u01f6\u01f7\7\66\2\2\u01f7\u01f8\5B\"\2\u01f8\u01f9\7\24"+
		"\2\2\u01f9\u01fe\5B\"\2\u01fa\u01fb\7\30\2\2\u01fb\u01fd\5B\"\2\u01fc"+
		"\u01fa\3\2\2\2\u01fd\u0200\3\2\2\2\u01fe\u01fc\3\2\2\2\u01fe\u01ff\3\2"+
		"\2\2\u01ff\u0201\3\2\2\2\u0200\u01fe\3\2\2\2\u0201\u0202\7\13\2\2\u0202"+
		"\u0227\3\2\2\2\u0203\u0227\5 \21\2\u0204\u0227\5<\37\2\u0205\u0206\7\b"+
		"\2\2\u0206\u0207\5B\"\2\u0207\u0208\7\30\2\2\u0208\u020d\5B\"\2\u0209"+
		"\u020a\7\30\2\2\u020a\u020c\5B\"\2\u020b\u0209\3\2\2\2\u020c\u020f\3\2"+
		"\2\2\u020d\u020b\3\2\2\2\u020d\u020e\3\2\2\2\u020e\u0210\3\2\2\2\u020f"+
		"\u020d\3\2\2\2\u0210\u0211\7\t\2\2\u0211\u0227\3\2\2\2\u0212\u021b\7\n"+
		"\2\2\u0213\u0218\5B\"\2\u0214\u0215\7\30\2\2\u0215\u0217\5B\"\2\u0216"+
		"\u0214\3\2\2\2\u0217\u021a\3\2\2\2\u0218\u0216\3\2\2\2\u0218\u0219\3\2"+
		"\2\2\u0219\u021c\3\2\2\2\u021a\u0218\3\2\2\2\u021b\u0213\3\2\2\2\u021b"+
		"\u021c\3\2\2\2\u021c\u021d\3\2\2\2\u021d\u0227\7\13\2\2\u021e\u0227\7"+
		"O\2\2\u021f\u0227\7<\2\2\u0220\u0227\7\64\2\2\u0221\u0227\t\5\2\2\u0222"+
		"\u0227\7N\2\2\u0223\u0227\5D#\2\u0224\u0227\7P\2\2\u0225\u0227\7\3\2\2"+
		"\u0226\u01c6\3\2\2\2\u0226\u01cb\3\2\2\2\u0226\u01ce\3\2\2\2\u0226\u01d0"+
		"\3\2\2\2\u0226\u01d8\3\2\2\2\u0226\u01e0\3\2\2\2\u0226\u01e8\3\2\2\2\u0226"+
		"\u01f4\3\2\2\2\u0226\u0203\3\2\2\2\u0226\u0204\3\2\2\2\u0226\u0205\3\2"+
		"\2\2\u0226\u0212\3\2\2\2\u0226\u021e\3\2\2\2\u0226\u021f\3\2\2\2\u0226"+
		"\u0220\3\2\2\2\u0226\u0221\3\2\2\2\u0226\u0222\3\2\2\2\u0226\u0223\3\2"+
		"\2\2\u0226\u0224\3\2\2\2\u0226\u0225\3\2\2\2\u0227\u0251\3\2\2\2\u0228"+
		"\u0229\f\36\2\2\u0229\u022a\7\22\2\2\u022a\u0250\5B\"\37\u022b\u022c\f"+
		"\34\2\2\u022c\u022d\7%\2\2\u022d\u0250\5B\"\34\u022e\u022f\f\33\2\2\u022f"+
		"\u0230\t\6\2\2\u0230\u0250\5B\"\34\u0231\u0232\f\32\2\2\u0232\u0233\t"+
		"\7\2\2\u0233\u0250\5B\"\33\u0234\u0235\f\31\2\2\u0235\u0236\7+\2\2\u0236"+
		"\u0250\5B\"\32\u0237\u0238\f\30\2\2\u0238\u0239\t\b\2\2\u0239\u0250\5"+
		"B\"\31\u023a\u023b\f\27\2\2\u023b\u023c\t\t\2\2\u023c\u0250\5B\"\30\u023d"+
		"\u023e\f\26\2\2\u023e\u023f\7\'\2\2\u023f\u0250\5B\"\27\u0240\u0241\f"+
		"\25\2\2\u0241\u0242\t\n\2\2\u0242\u0250\5B\"\26\u0243\u0244\f\24\2\2\u0244"+
		"\u0245\7\17\2\2\u0245\u0250\5B\"\24\u0246\u0247\f\37\2\2\u0247\u0248\7"+
		"\n\2\2\u0248\u024b\5B\"\2\u0249\u024a\7\30\2\2\u024a\u024c\5B\"\2\u024b"+
		"\u0249\3\2\2\2\u024b\u024c\3\2\2\2\u024c\u024d\3\2\2\2\u024d\u024e\7\13"+
		"\2\2\u024e\u0250\3\2\2\2\u024f\u0228\3\2\2\2\u024f\u022b\3\2\2\2\u024f"+
		"\u022e\3\2\2\2\u024f\u0231\3\2\2\2\u024f\u0234\3\2\2\2\u024f\u0237\3\2"+
		"\2\2\u024f\u023a\3\2\2\2\u024f\u023d\3\2\2\2\u024f\u0240\3\2\2\2\u024f"+
		"\u0243\3\2\2\2\u024f\u0246\3\2\2\2\u0250\u0253\3\2\2\2\u0251\u024f\3\2"+
		"\2\2\u0251\u0252\3\2\2\2\u0252C\3\2\2\2\u0253\u0251\3\2\2\2\u0254\u0260"+
		"\7\4\2\2\u0255\u0260\7\5\2\2\u0256\u0260\7\6\2\2\u0257\u0260\7\7\2\2\u0258"+
		"\u0259\7N\2\2\u0259\u0260\7\23\2\2\u025a\u025b\7\23\2\2\u025b\u0260\7"+
		"N\2\2\u025c\u025d\7N\2\2\u025d\u025e\7\23\2\2\u025e\u0260\7N\2\2\u025f"+
		"\u0254\3\2\2\2\u025f\u0255\3\2\2\2\u025f\u0256\3\2\2\2\u025f\u0257\3\2"+
		"\2\2\u025f\u0258\3\2\2\2\u025f\u025a\3\2\2\2\u025f\u025c\3\2\2\2\u0260"+
		"E\3\2\2\28ITXamw|\u0081\u0087\u008b\u0097\u00a5\u00a9\u00b5\u00cf\u00d4"+
		"\u00fd\u0101\u010f\u011f\u0121\u0126\u0131\u0138\u013b\u013f\u0142\u0148"+
		"\u014c\u0156\u0159\u0168\u016b\u0175\u017f\u0182\u0187\u0193\u0196\u019b"+
		"\u01a6\u01a9\u01b5\u01b8\u01c1\u01fe\u020d\u0218\u021b\u0226\u024b\u024f"+
		"\u0251\u025f";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}