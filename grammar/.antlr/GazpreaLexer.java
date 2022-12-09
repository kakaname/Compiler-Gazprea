// Generated from /home/anxiety/School/CMPUT415/assign/gazprea/grammar/Gazprea.g4 by ANTLR 4.9.2
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class GazpreaLexer extends Lexer {
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
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"StringLit", "ExponentialLiteral1", "ExponentialLiteral2", "ExponentialLiteral3", 
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


	public GazpreaLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "Gazprea.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2S\u026b\b\1\4\2\t"+
		"\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13"+
		"\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31\t\31"+
		"\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\4 \t \4!"+
		"\t!\4\"\t\"\4#\t#\4$\t$\4%\t%\4&\t&\4\'\t\'\4(\t(\4)\t)\4*\t*\4+\t+\4"+
		",\t,\4-\t-\4.\t.\4/\t/\4\60\t\60\4\61\t\61\4\62\t\62\4\63\t\63\4\64\t"+
		"\64\4\65\t\65\4\66\t\66\4\67\t\67\48\t8\49\t9\4:\t:\4;\t;\4<\t<\4=\t="+
		"\4>\t>\4?\t?\4@\t@\4A\tA\4B\tB\4C\tC\4D\tD\4E\tE\4F\tF\4G\tG\4H\tH\4I"+
		"\tI\4J\tJ\4K\tK\4L\tL\4M\tM\4N\tN\4O\tO\4P\tP\4Q\tQ\4R\tR\3\2\3\2\3\2"+
		"\3\2\7\2\u00aa\n\2\f\2\16\2\u00ad\13\2\3\2\3\2\3\3\6\3\u00b2\n\3\r\3\16"+
		"\3\u00b3\3\3\3\3\5\3\u00b8\n\3\3\3\6\3\u00bb\n\3\r\3\16\3\u00bc\3\4\6"+
		"\4\u00c0\n\4\r\4\16\4\u00c1\3\4\3\4\3\4\5\4\u00c7\n\4\3\4\6\4\u00ca\n"+
		"\4\r\4\16\4\u00cb\3\5\3\5\6\5\u00d0\n\5\r\5\16\5\u00d1\3\5\3\5\5\5\u00d6"+
		"\n\5\3\5\6\5\u00d9\n\5\r\5\16\5\u00da\3\6\6\6\u00de\n\6\r\6\16\6\u00df"+
		"\3\6\3\6\6\6\u00e4\n\6\r\6\16\6\u00e5\3\6\3\6\5\6\u00ea\n\6\3\6\6\6\u00ed"+
		"\n\6\r\6\16\6\u00ee\3\7\3\7\3\b\3\b\3\t\3\t\3\n\3\n\3\13\3\13\3\f\3\f"+
		"\3\r\3\r\3\16\3\16\3\16\3\17\3\17\3\20\3\20\3\21\3\21\3\21\3\22\3\22\3"+
		"\23\3\23\3\24\3\24\3\24\3\25\3\25\3\25\3\26\3\26\3\27\3\27\3\30\3\30\3"+
		"\31\3\31\3\32\3\32\3\33\3\33\3\34\3\34\3\35\3\35\3\35\3\36\3\36\3\36\3"+
		"\37\3\37\3\37\3 \3 \3!\3!\3\"\3\"\3\"\3#\3#\3#\3$\3$\3%\3%\3&\3&\3&\3"+
		"&\3\'\3\'\3\'\3(\3(\3(\3(\3(\3(\3(\3(\3)\3)\3)\3)\3)\3)\3*\3*\3*\3+\3"+
		"+\3+\3+\3+\3,\3,\3,\3,\3,\3,\3,\3,\3,\3,\3-\3-\3-\3-\3-\3-\3-\3-\3.\3"+
		".\3.\3.\3.\3.\3/\3/\3/\3/\3/\3/\3/\3/\3/\3\60\3\60\3\60\3\60\3\60\3\61"+
		"\3\61\3\61\3\61\3\61\3\61\3\62\3\62\3\62\3\62\3\62\3\62\3\62\3\62\3\62"+
		"\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\63\3\64\3\64\3\64\3\65\3\65"+
		"\3\65\3\66\3\66\3\66\3\66\3\66\3\66\3\66\3\66\3\67\3\67\3\67\3\67\3\67"+
		"\3\67\3\67\3\67\3\67\38\38\38\38\38\38\38\39\39\39\39\39\3:\3:\3:\3:\3"+
		";\3;\3;\3;\3;\3<\3<\3<\3=\3=\3=\3=\3=\3=\3=\3=\3=\3=\3>\3>\3>\3>\3>\3"+
		"?\3?\3?\3?\3?\3?\3?\3@\3@\3@\3@\3@\3@\3@\3@\3A\3A\3A\3A\3A\3A\3A\3A\3"+
		"B\3B\3B\3B\3B\3C\3C\3C\3C\3C\3C\3C\3C\3C\3C\3D\3D\3D\3D\3D\3D\3D\3D\3"+
		"D\3D\3D\3E\3E\3E\3E\3E\3E\3E\3E\3E\3E\3E\3E\3E\3F\3F\3F\3F\3F\3F\3F\3"+
		"G\3G\3G\3G\3G\3H\3H\3H\3H\3H\3H\3I\3I\3I\3I\3I\3I\3I\3I\3J\3J\3J\3J\3"+
		"K\3K\3K\3K\3K\3K\3L\3L\3L\3L\3M\6M\u0238\nM\rM\16M\u0239\3N\3N\7N\u023e"+
		"\nN\fN\16N\u0241\13N\3O\3O\3O\3O\3O\3O\3O\5O\u024a\nO\3P\3P\3P\3P\7P\u0250"+
		"\nP\fP\16P\u0253\13P\3P\3P\3P\3P\3P\3Q\3Q\3Q\3Q\7Q\u025e\nQ\fQ\16Q\u0261"+
		"\13Q\3Q\3Q\3R\6R\u0266\nR\rR\16R\u0267\3R\3R\4\u00ab\u0251\2S\3\3\5\4"+
		"\7\5\t\6\13\7\r\b\17\t\21\n\23\13\25\f\27\r\31\16\33\17\35\20\37\21!\22"+
		"#\23%\24\'\25)\26+\27-\30/\31\61\32\63\33\65\34\67\359\36;\37= ?!A\"C"+
		"#E$G%I&K\'M(O)Q*S+U,W-Y.[/]\60_\61a\62c\63e\64g\65i\66k\67m8o9q:s;u<w"+
		"=y>{?}@\177A\u0081B\u0083C\u0085D\u0087E\u0089F\u008bG\u008dH\u008fI\u0091"+
		"J\u0093K\u0095L\u0097M\u0099N\u009bO\u009dP\u009fQ\u00a1R\u00a3S\3\2\t"+
		"\3\2\62;\4\2--//\5\2C\\aac|\6\2\62;C\\aac|\n\2$$))\62\62^^cdppttvv\4\2"+
		"\f\f\17\17\5\2\13\f\17\17\"\"\2\u027f\2\3\3\2\2\2\2\5\3\2\2\2\2\7\3\2"+
		"\2\2\2\t\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2\2\2\2\21\3\2\2\2\2"+
		"\23\3\2\2\2\2\25\3\2\2\2\2\27\3\2\2\2\2\31\3\2\2\2\2\33\3\2\2\2\2\35\3"+
		"\2\2\2\2\37\3\2\2\2\2!\3\2\2\2\2#\3\2\2\2\2%\3\2\2\2\2\'\3\2\2\2\2)\3"+
		"\2\2\2\2+\3\2\2\2\2-\3\2\2\2\2/\3\2\2\2\2\61\3\2\2\2\2\63\3\2\2\2\2\65"+
		"\3\2\2\2\2\67\3\2\2\2\29\3\2\2\2\2;\3\2\2\2\2=\3\2\2\2\2?\3\2\2\2\2A\3"+
		"\2\2\2\2C\3\2\2\2\2E\3\2\2\2\2G\3\2\2\2\2I\3\2\2\2\2K\3\2\2\2\2M\3\2\2"+
		"\2\2O\3\2\2\2\2Q\3\2\2\2\2S\3\2\2\2\2U\3\2\2\2\2W\3\2\2\2\2Y\3\2\2\2\2"+
		"[\3\2\2\2\2]\3\2\2\2\2_\3\2\2\2\2a\3\2\2\2\2c\3\2\2\2\2e\3\2\2\2\2g\3"+
		"\2\2\2\2i\3\2\2\2\2k\3\2\2\2\2m\3\2\2\2\2o\3\2\2\2\2q\3\2\2\2\2s\3\2\2"+
		"\2\2u\3\2\2\2\2w\3\2\2\2\2y\3\2\2\2\2{\3\2\2\2\2}\3\2\2\2\2\177\3\2\2"+
		"\2\2\u0081\3\2\2\2\2\u0083\3\2\2\2\2\u0085\3\2\2\2\2\u0087\3\2\2\2\2\u0089"+
		"\3\2\2\2\2\u008b\3\2\2\2\2\u008d\3\2\2\2\2\u008f\3\2\2\2\2\u0091\3\2\2"+
		"\2\2\u0093\3\2\2\2\2\u0095\3\2\2\2\2\u0097\3\2\2\2\2\u0099\3\2\2\2\2\u009b"+
		"\3\2\2\2\2\u009d\3\2\2\2\2\u009f\3\2\2\2\2\u00a1\3\2\2\2\2\u00a3\3\2\2"+
		"\2\3\u00a5\3\2\2\2\5\u00b1\3\2\2\2\7\u00bf\3\2\2\2\t\u00cd\3\2\2\2\13"+
		"\u00dd\3\2\2\2\r\u00f0\3\2\2\2\17\u00f2\3\2\2\2\21\u00f4\3\2\2\2\23\u00f6"+
		"\3\2\2\2\25\u00f8\3\2\2\2\27\u00fa\3\2\2\2\31\u00fc\3\2\2\2\33\u00fe\3"+
		"\2\2\2\35\u0101\3\2\2\2\37\u0103\3\2\2\2!\u0105\3\2\2\2#\u0108\3\2\2\2"+
		"%\u010a\3\2\2\2\'\u010c\3\2\2\2)\u010f\3\2\2\2+\u0112\3\2\2\2-\u0114\3"+
		"\2\2\2/\u0116\3\2\2\2\61\u0118\3\2\2\2\63\u011a\3\2\2\2\65\u011c\3\2\2"+
		"\2\67\u011e\3\2\2\29\u0120\3\2\2\2;\u0123\3\2\2\2=\u0126\3\2\2\2?\u0129"+
		"\3\2\2\2A\u012b\3\2\2\2C\u012d\3\2\2\2E\u0130\3\2\2\2G\u0133\3\2\2\2I"+
		"\u0135\3\2\2\2K\u0137\3\2\2\2M\u013b\3\2\2\2O\u013e\3\2\2\2Q\u0146\3\2"+
		"\2\2S\u014c\3\2\2\2U\u014f\3\2\2\2W\u0154\3\2\2\2Y\u015e\3\2\2\2[\u0166"+
		"\3\2\2\2]\u016c\3\2\2\2_\u0175\3\2\2\2a\u017a\3\2\2\2c\u0180\3\2\2\2e"+
		"\u0189\3\2\2\2g\u0192\3\2\2\2i\u0195\3\2\2\2k\u0198\3\2\2\2m\u01a0\3\2"+
		"\2\2o\u01a9\3\2\2\2q\u01b0\3\2\2\2s\u01b5\3\2\2\2u\u01b9\3\2\2\2w\u01be"+
		"\3\2\2\2y\u01c1\3\2\2\2{\u01cb\3\2\2\2}\u01d0\3\2\2\2\177\u01d7\3\2\2"+
		"\2\u0081\u01df\3\2\2\2\u0083\u01e7\3\2\2\2\u0085\u01ec\3\2\2\2\u0087\u01f6"+
		"\3\2\2\2\u0089\u0201\3\2\2\2\u008b\u020e\3\2\2\2\u008d\u0215\3\2\2\2\u008f"+
		"\u021a\3\2\2\2\u0091\u0220\3\2\2\2\u0093\u0228\3\2\2\2\u0095\u022c\3\2"+
		"\2\2\u0097\u0232\3\2\2\2\u0099\u0237\3\2\2\2\u009b\u023b\3\2\2\2\u009d"+
		"\u0249\3\2\2\2\u009f\u024b\3\2\2\2\u00a1\u0259\3\2\2\2\u00a3\u0265\3\2"+
		"\2\2\u00a5\u00ab\7$\2\2\u00a6\u00a7\7^\2\2\u00a7\u00aa\7$\2\2\u00a8\u00aa"+
		"\13\2\2\2\u00a9\u00a6\3\2\2\2\u00a9\u00a8\3\2\2\2\u00aa\u00ad\3\2\2\2"+
		"\u00ab\u00ac\3\2\2\2\u00ab\u00a9\3\2\2\2\u00ac\u00ae\3\2\2\2\u00ad\u00ab"+
		"\3\2\2\2\u00ae\u00af\7$\2\2\u00af\4\3\2\2\2\u00b0\u00b2\t\2\2\2\u00b1"+
		"\u00b0\3\2\2\2\u00b2\u00b3\3\2\2\2\u00b3\u00b1\3\2\2\2\u00b3\u00b4\3\2"+
		"\2\2\u00b4\u00b5\3\2\2\2\u00b5\u00b7\7g\2\2\u00b6\u00b8\t\3\2\2\u00b7"+
		"\u00b6\3\2\2\2\u00b7\u00b8\3\2\2\2\u00b8\u00ba\3\2\2\2\u00b9\u00bb\t\2"+
		"\2\2\u00ba\u00b9\3\2\2\2\u00bb\u00bc\3\2\2\2\u00bc\u00ba\3\2\2\2\u00bc"+
		"\u00bd\3\2\2\2\u00bd\6\3\2\2\2\u00be\u00c0\t\2\2\2\u00bf\u00be\3\2\2\2"+
		"\u00c0\u00c1\3\2\2\2\u00c1\u00bf\3\2\2\2\u00c1\u00c2\3\2\2\2\u00c2\u00c3"+
		"\3\2\2\2\u00c3\u00c4\7\60\2\2\u00c4\u00c6\7g\2\2\u00c5\u00c7\t\3\2\2\u00c6"+
		"\u00c5\3\2\2\2\u00c6\u00c7\3\2\2\2\u00c7\u00c9\3\2\2\2\u00c8\u00ca\t\2"+
		"\2\2\u00c9\u00c8\3\2\2\2\u00ca\u00cb\3\2\2\2\u00cb\u00c9\3\2\2\2\u00cb"+
		"\u00cc\3\2\2\2\u00cc\b\3\2\2\2\u00cd\u00cf\7\60\2\2\u00ce\u00d0\t\2\2"+
		"\2\u00cf\u00ce\3\2\2\2\u00d0\u00d1\3\2\2\2\u00d1\u00cf\3\2\2\2\u00d1\u00d2"+
		"\3\2\2\2\u00d2\u00d3\3\2\2\2\u00d3\u00d5\7g\2\2\u00d4\u00d6\t\3\2\2\u00d5"+
		"\u00d4\3\2\2\2\u00d5\u00d6\3\2\2\2\u00d6\u00d8\3\2\2\2\u00d7\u00d9\t\2"+
		"\2\2\u00d8\u00d7\3\2\2\2\u00d9\u00da\3\2\2\2\u00da\u00d8\3\2\2\2\u00da"+
		"\u00db\3\2\2\2\u00db\n\3\2\2\2\u00dc\u00de\t\2\2\2\u00dd\u00dc\3\2\2\2"+
		"\u00de\u00df\3\2\2\2\u00df\u00dd\3\2\2\2\u00df\u00e0\3\2\2\2\u00e0\u00e1"+
		"\3\2\2\2\u00e1\u00e3\7\60\2\2\u00e2\u00e4\t\2\2\2\u00e3\u00e2\3\2\2\2"+
		"\u00e4\u00e5\3\2\2\2\u00e5\u00e3\3\2\2\2\u00e5\u00e6\3\2\2\2\u00e6\u00e7"+
		"\3\2\2\2\u00e7\u00e9\7g\2\2\u00e8\u00ea\t\3\2\2\u00e9\u00e8\3\2\2\2\u00e9"+
		"\u00ea\3\2\2\2\u00ea\u00ec\3\2\2\2\u00eb\u00ed\t\2\2\2\u00ec\u00eb\3\2"+
		"\2\2\u00ed\u00ee\3\2\2\2\u00ee\u00ec\3\2\2\2\u00ee\u00ef\3\2\2\2\u00ef"+
		"\f\3\2\2\2\u00f0\u00f1\7*\2\2\u00f1\16\3\2\2\2\u00f2\u00f3\7+\2\2\u00f3"+
		"\20\3\2\2\2\u00f4\u00f5\7]\2\2\u00f5\22\3\2\2\2\u00f6\u00f7\7_\2\2\u00f7"+
		"\24\3\2\2\2\u00f8\u00f9\7}\2\2\u00f9\26\3\2\2\2\u00fa\u00fb\7\177\2\2"+
		"\u00fb\30\3\2\2\2\u00fc\u00fd\7~\2\2\u00fd\32\3\2\2\2\u00fe\u00ff\7~\2"+
		"\2\u00ff\u0100\7~\2\2\u0100\34\3\2\2\2\u0101\u0102\7=\2\2\u0102\36\3\2"+
		"\2\2\u0103\u0104\7?\2\2\u0104 \3\2\2\2\u0105\u0106\7\60\2\2\u0106\u0107"+
		"\7\60\2\2\u0107\"\3\2\2\2\u0108\u0109\7\60\2\2\u0109$\3\2\2\2\u010a\u010b"+
		"\7(\2\2\u010b&\3\2\2\2\u010c\u010d\7/\2\2\u010d\u010e\7@\2\2\u010e(\3"+
		"\2\2\2\u010f\u0110\7>\2\2\u0110\u0111\7/\2\2\u0111*\3\2\2\2\u0112\u0113"+
		"\7)\2\2\u0113,\3\2\2\2\u0114\u0115\7.\2\2\u0115.\3\2\2\2\u0116\u0117\7"+
		"$\2\2\u0117\60\3\2\2\2\u0118\u0119\7-\2\2\u0119\62\3\2\2\2\u011a\u011b"+
		"\7/\2\2\u011b\64\3\2\2\2\u011c\u011d\7\61\2\2\u011d\66\3\2\2\2\u011e\u011f"+
		"\7,\2\2\u011f8\3\2\2\2\u0120\u0121\7,\2\2\u0121\u0122\7,\2\2\u0122:\3"+
		"\2\2\2\u0123\u0124\7>\2\2\u0124\u0125\7?\2\2\u0125<\3\2\2\2\u0126\u0127"+
		"\7@\2\2\u0127\u0128\7?\2\2\u0128>\3\2\2\2\u0129\u012a\7>\2\2\u012a@\3"+
		"\2\2\2\u012b\u012c\7@\2\2\u012cB\3\2\2\2\u012d\u012e\7?\2\2\u012e\u012f"+
		"\7?\2\2\u012fD\3\2\2\2\u0130\u0131\7#\2\2\u0131\u0132\7?\2\2\u0132F\3"+
		"\2\2\2\u0133\u0134\7`\2\2\u0134H\3\2\2\2\u0135\u0136\7\'\2\2\u0136J\3"+
		"\2\2\2\u0137\u0138\7c\2\2\u0138\u0139\7p\2\2\u0139\u013a\7f\2\2\u013a"+
		"L\3\2\2\2\u013b\u013c\7c\2\2\u013c\u013d\7u\2\2\u013dN\3\2\2\2\u013e\u013f"+
		"\7d\2\2\u013f\u0140\7q\2\2\u0140\u0141\7q\2\2\u0141\u0142\7n\2\2\u0142"+
		"\u0143\7g\2\2\u0143\u0144\7c\2\2\u0144\u0145\7p\2\2\u0145P\3\2\2\2\u0146"+
		"\u0147\7d\2\2\u0147\u0148\7t\2\2\u0148\u0149\7g\2\2\u0149\u014a\7c\2\2"+
		"\u014a\u014b\7m\2\2\u014bR\3\2\2\2\u014c\u014d\7d\2\2\u014d\u014e\7{\2"+
		"\2\u014eT\3\2\2\2\u014f\u0150\7e\2\2\u0150\u0151\7c\2\2\u0151\u0152\7"+
		"n\2\2\u0152\u0153\7n\2\2\u0153V\3\2\2\2\u0154\u0155\7e\2\2\u0155\u0156"+
		"\7j\2\2\u0156\u0157\7c\2\2\u0157\u0158\7t\2\2\u0158\u0159\7c\2\2\u0159"+
		"\u015a\7e\2\2\u015a\u015b\7v\2\2\u015b\u015c\7g\2\2\u015c\u015d\7t\2\2"+
		"\u015dX\3\2\2\2\u015e\u015f\7e\2\2\u015f\u0160\7q\2\2\u0160\u0161\7n\2"+
		"\2\u0161\u0162\7w\2\2\u0162\u0163\7o\2\2\u0163\u0164\7p\2\2\u0164\u0165"+
		"\7u\2\2\u0165Z\3\2\2\2\u0166\u0167\7e\2\2\u0167\u0168\7q\2\2\u0168\u0169"+
		"\7p\2\2\u0169\u016a\7u\2\2\u016a\u016b\7v\2\2\u016b\\\3\2\2\2\u016c\u016d"+
		"\7e\2\2\u016d\u016e\7q\2\2\u016e\u016f\7p\2\2\u016f\u0170\7v\2\2\u0170"+
		"\u0171\7k\2\2\u0171\u0172\7p\2\2\u0172\u0173\7w\2\2\u0173\u0174\7g\2\2"+
		"\u0174^\3\2\2\2\u0175\u0176\7g\2\2\u0176\u0177\7n\2\2\u0177\u0178\7u\2"+
		"\2\u0178\u0179\7g\2\2\u0179`\3\2\2\2\u017a\u017b\7h\2\2\u017b\u017c\7"+
		"c\2\2\u017c\u017d\7n\2\2\u017d\u017e\7u\2\2\u017e\u017f\7g\2\2\u017fb"+
		"\3\2\2\2\u0180\u0181\7h\2\2\u0181\u0182\7w\2\2\u0182\u0183\7p\2\2\u0183"+
		"\u0184\7e\2\2\u0184\u0185\7v\2\2\u0185\u0186\7k\2\2\u0186\u0187\7q\2\2"+
		"\u0187\u0188\7p\2\2\u0188d\3\2\2\2\u0189\u018a\7k\2\2\u018a\u018b\7f\2"+
		"\2\u018b\u018c\7g\2\2\u018c\u018d\7p\2\2\u018d\u018e\7v\2\2\u018e\u018f"+
		"\7k\2\2\u018f\u0190\7v\2\2\u0190\u0191\7{\2\2\u0191f\3\2\2\2\u0192\u0193"+
		"\7k\2\2\u0193\u0194\7h\2\2\u0194h\3\2\2\2\u0195\u0196\7k\2\2\u0196\u0197"+
		"\7p\2\2\u0197j\3\2\2\2\u0198\u0199\7k\2\2\u0199\u019a\7p\2\2\u019a\u019b"+
		"\7v\2\2\u019b\u019c\7g\2\2\u019c\u019d\7i\2\2\u019d\u019e\7g\2\2\u019e"+
		"\u019f\7t\2\2\u019fl\3\2\2\2\u01a0\u01a1\7k\2\2\u01a1\u01a2\7p\2\2\u01a2"+
		"\u01a3\7v\2\2\u01a3\u01a4\7g\2\2\u01a4\u01a5\7t\2\2\u01a5\u01a6\7x\2\2"+
		"\u01a6\u01a7\7c\2\2\u01a7\u01a8\7n\2\2\u01a8n\3\2\2\2\u01a9\u01aa\7n\2"+
		"\2\u01aa\u01ab\7g\2\2\u01ab\u01ac\7p\2\2\u01ac\u01ad\7i\2\2\u01ad\u01ae"+
		"\7v\2\2\u01ae\u01af\7j\2\2\u01afp\3\2\2\2\u01b0\u01b1\7n\2\2\u01b1\u01b2"+
		"\7q\2\2\u01b2\u01b3\7q\2\2\u01b3\u01b4\7r\2\2\u01b4r\3\2\2\2\u01b5\u01b6"+
		"\7p\2\2\u01b6\u01b7\7q\2\2\u01b7\u01b8\7v\2\2\u01b8t\3\2\2\2\u01b9\u01ba"+
		"\7p\2\2\u01ba\u01bb\7w\2\2\u01bb\u01bc\7n\2\2\u01bc\u01bd\7n\2\2\u01bd"+
		"v\3\2\2\2\u01be\u01bf\7q\2\2\u01bf\u01c0\7t\2\2\u01c0x\3\2\2\2\u01c1\u01c2"+
		"\7r\2\2\u01c2\u01c3\7t\2\2\u01c3\u01c4\7q\2\2\u01c4\u01c5\7e\2\2\u01c5"+
		"\u01c6\7g\2\2\u01c6\u01c7\7f\2\2\u01c7\u01c8\7w\2\2\u01c8\u01c9\7t\2\2"+
		"\u01c9\u01ca\7g\2\2\u01caz\3\2\2\2\u01cb\u01cc\7t\2\2\u01cc\u01cd\7g\2"+
		"\2\u01cd\u01ce\7c\2\2\u01ce\u01cf\7n\2\2\u01cf|\3\2\2\2\u01d0\u01d1\7"+
		"t\2\2\u01d1\u01d2\7g\2\2\u01d2\u01d3\7v\2\2\u01d3\u01d4\7w\2\2\u01d4\u01d5"+
		"\7t\2\2\u01d5\u01d6\7p\2\2\u01d6~\3\2\2\2\u01d7\u01d8\7t\2\2\u01d8\u01d9"+
		"\7g\2\2\u01d9\u01da\7v\2\2\u01da\u01db\7w\2\2\u01db\u01dc\7t\2\2\u01dc"+
		"\u01dd\7p\2\2\u01dd\u01de\7u\2\2\u01de\u0080\3\2\2\2\u01df\u01e0\7t\2"+
		"\2\u01e0\u01e1\7g\2\2\u01e1\u01e2\7x\2\2\u01e2\u01e3\7g\2\2\u01e3\u01e4"+
		"\7t\2\2\u01e4\u01e5\7u\2\2\u01e5\u01e6\7g\2\2\u01e6\u0082\3\2\2\2\u01e7"+
		"\u01e8\7t\2\2\u01e8\u01e9\7q\2\2\u01e9\u01ea\7y\2\2\u01ea\u01eb\7u\2\2"+
		"\u01eb\u0084\3\2\2\2\u01ec\u01ed\7u\2\2\u01ed\u01ee\7v\2\2\u01ee\u01ef"+
		"\7f\2\2\u01ef\u01f0\7a\2\2\u01f0\u01f1\7k\2\2\u01f1\u01f2\7p\2\2\u01f2"+
		"\u01f3\7r\2\2\u01f3\u01f4\7w\2\2\u01f4\u01f5\7v\2\2\u01f5\u0086\3\2\2"+
		"\2\u01f6\u01f7\7u\2\2\u01f7\u01f8\7v\2\2\u01f8\u01f9\7f\2\2\u01f9\u01fa"+
		"\7a\2\2\u01fa\u01fb\7q\2\2\u01fb\u01fc\7w\2\2\u01fc\u01fd\7v\2\2\u01fd"+
		"\u01fe\7r\2\2\u01fe\u01ff\7w\2\2\u01ff\u0200\7v\2\2\u0200\u0088\3\2\2"+
		"\2\u0201\u0202\7u\2\2\u0202\u0203\7v\2\2\u0203\u0204\7t\2\2\u0204\u0205"+
		"\7g\2\2\u0205\u0206\7c\2\2\u0206\u0207\7o\2\2\u0207\u0208\7a\2\2\u0208"+
		"\u0209\7u\2\2\u0209\u020a\7v\2\2\u020a\u020b\7c\2\2\u020b\u020c\7v\2\2"+
		"\u020c\u020d\7g\2\2\u020d\u008a\3\2\2\2\u020e\u020f\7u\2\2\u020f\u0210"+
		"\7v\2\2\u0210\u0211\7t\2\2\u0211\u0212\7k\2\2\u0212\u0213\7p\2\2\u0213"+
		"\u0214\7i\2\2\u0214\u008c\3\2\2\2\u0215\u0216\7v\2\2\u0216\u0217\7t\2"+
		"\2\u0217\u0218\7w\2\2\u0218\u0219\7g\2\2\u0219\u008e\3\2\2\2\u021a\u021b"+
		"\7v\2\2\u021b\u021c\7w\2\2\u021c\u021d\7r\2\2\u021d\u021e\7n\2\2\u021e"+
		"\u021f\7g\2\2\u021f\u0090\3\2\2\2\u0220\u0221\7v\2\2\u0221\u0222\7{\2"+
		"\2\u0222\u0223\7r\2\2\u0223\u0224\7g\2\2\u0224\u0225\7f\2\2\u0225\u0226"+
		"\7g\2\2\u0226\u0227\7h\2\2\u0227\u0092\3\2\2\2\u0228\u0229\7x\2\2\u0229"+
		"\u022a\7c\2\2\u022a\u022b\7t\2\2\u022b\u0094\3\2\2\2\u022c\u022d\7y\2"+
		"\2\u022d\u022e\7j\2\2\u022e\u022f\7k\2\2\u022f\u0230\7n\2\2\u0230\u0231"+
		"\7g\2\2\u0231\u0096\3\2\2\2\u0232\u0233\7z\2\2\u0233\u0234\7q\2\2\u0234"+
		"\u0235\7t\2\2\u0235\u0098\3\2\2\2\u0236\u0238\t\2\2\2\u0237\u0236\3\2"+
		"\2\2\u0238\u0239\3\2\2\2\u0239\u0237\3\2\2\2\u0239\u023a\3\2\2\2\u023a"+
		"\u009a\3\2\2\2\u023b\u023f\t\4\2\2\u023c\u023e\t\5\2\2\u023d\u023c\3\2"+
		"\2\2\u023e\u0241\3\2\2\2\u023f\u023d\3\2\2\2\u023f\u0240\3\2\2\2\u0240"+
		"\u009c\3\2\2\2\u0241\u023f\3\2\2\2\u0242\u0243\7)\2\2\u0243\u0244\13\2"+
		"\2\2\u0244\u024a\7)\2\2\u0245\u0246\7)\2\2\u0246\u0247\7^\2\2\u0247\u0248"+
		"\t\6\2\2\u0248\u024a\7)\2\2\u0249\u0242\3\2\2\2\u0249\u0245\3\2\2\2\u024a"+
		"\u009e\3\2\2\2\u024b\u024c\7\61\2\2\u024c\u024d\7,\2\2\u024d\u0251\3\2"+
		"\2\2\u024e\u0250\13\2\2\2\u024f\u024e\3\2\2\2\u0250\u0253\3\2\2\2\u0251"+
		"\u0252\3\2\2\2\u0251\u024f\3\2\2\2\u0252\u0254\3\2\2\2\u0253\u0251\3\2"+
		"\2\2\u0254\u0255\7,\2\2\u0255\u0256\7\61\2\2\u0256\u0257\3\2\2\2\u0257"+
		"\u0258\bP\2\2\u0258\u00a0\3\2\2\2\u0259\u025a\7\61\2\2\u025a\u025b\7\61"+
		"\2\2\u025b\u025f\3\2\2\2\u025c\u025e\n\7\2\2\u025d\u025c\3\2\2\2\u025e"+
		"\u0261\3\2\2\2\u025f\u025d\3\2\2\2\u025f\u0260\3\2\2\2\u0260\u0262\3\2"+
		"\2\2\u0261\u025f\3\2\2\2\u0262\u0263\bQ\2\2\u0263\u00a2\3\2\2\2\u0264"+
		"\u0266\t\b\2\2\u0265\u0264\3\2\2\2\u0266\u0267\3\2\2\2\u0267\u0265\3\2"+
		"\2\2\u0267\u0268\3\2\2\2\u0268\u0269\3\2\2\2\u0269\u026a\bR\2\2\u026a"+
		"\u00a4\3\2\2\2\30\2\u00a9\u00ab\u00b3\u00b7\u00bc\u00c1\u00c6\u00cb\u00d1"+
		"\u00d5\u00da\u00df\u00e5\u00e9\u00ee\u0239\u023f\u0249\u0251\u025f\u0267"+
		"\3\b\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}