%{
#include <cstdlib>
#include "c_parser.tab.h"

%}

%option noyywrap

whitespace	[ ]*[\n]*[\t]*

NONZERO     [1-9]
DIGIT       [0-9]
PREOCT      [0]
OCTAL       [0-7]
PREHEX      [0][xX]
HEXADEC     [0-9a-fA-F]
INTSUF      ([uU][lL]?|[lL][uU]?)
FLOAT       ((([0-9]*)?[.][0-9]+|[0-9]+[.])([eE][+-]?[0-9]*)?|[0-9]*([eE][+-]?[0-9]*))[flFL]?
ID          [_a-zA-Z][_a-zA-Z0-9]*
ESC_SIMPLE  ("\\'"|[\\][\"]|"\\?"|"\\\\"|"\\a"|"\\b"|"\\f"|"\\n"|"\\r"|"\\t"|"\\v")
ESC_OCT     [\\][0-7][0-7]?[0-7]?
ESC_HEX     [\\][x][0-9a-fA-F]+
STR         [^\"\\]|{ESC_SIMPLE}|{ESC_OCT}|{ESC_HEX}
CHAR        [^\'\\]|{ESC_SIMPLE}|{ESC_OCT}|{ESC_HEX}
STRSEQ      [L]?("\""){STR}*("\"")
CHARSEQ     [L]?("\'"){CHAR}*("\'")

%%
"else"					return TElse;
"for"					return TFor;
"if"					return TIf;
"do"					return TDo;
"while"					return TWhile;
"auto"                  return TAuto;
"break"                 return TBreak;
"char"                  return TChar;
"continue"              return TContinue;
"default"               return TDefault;
"double"                return TDouble;
"extern"                return TExtern;
"float"                 return TFloat;
"goto"                  return TGoto;
"union"                 return TUnion;
"const"                 return TConst;
"long"					return TLong;
"register"              return TRegister;
"return"                return TReturn;
"short"                 return TShort;
"signed"                return TSigned;
"struct"                return TStruct;
"typedef"               return TTypedef;
"unsigned"              return TUnsigned;
"static"                return TStatic;
"switch"				return TSwitch;
"void"                  return TVoid;
"volatile"              return TVolatile;
"sizeof"                return SIZEOF;


"int"					return TInt;
"return"				return TReturn;

{ID} 							{ yylval.string = strdup(yytext); return IDENTIFIER; }

{STRSEQ}                		{ yylval.string = strdup(yytext); return STRING; }

{CHARSEQ}               		{ yylval.string = strdup(yytext); return CHARSEQ; }

{NONZERO}{DIGIT}*{INTSUF}?		{ yylval.num = atoi(yytext); return INTEGERN; } 

{FLOAT}                         { yylval.dnum = atof(yytext); return FLOATN; }

{PREOCT}{OCTAL}*{INTSUF}?       { yylval.num = strtol(yytext,NULL,8); return OCTALN; }

{PREHEX}{HEXADEC}+{INTSUF}?     { yylval.num = strtol(yytext,NULL,16); return HEXADECN; }


"..."					return TEllipsis;
"->"					return TPtrAccess;
"++"					return TIncre;
"--"					return TDecre;
"<<"					return TLBitShift;
">>"					return TRBitShift;
"<="					return TCompLE;
">="					return TCompGE;
"=="					return TCompEQ;
"!="					return TCompNE;
"+="					return TAssignAdd;
"-="					return TAssignSub;
"*="					return TAssignMul;
"/="					return TAssignDiv;
"%="					return TAssignMod;
"<<="					return TAssignLBitShift;
">>="					return TAssignRBitShift;
"&="					return TAssignBitAnd;
"|="					return TAssignBitOr;
"^="					return TAssignBitXor;
"&&"					return TBoolAnd;
"||"					return TBoolOr;
"+"						return '+';
"-"						return '-';
"*"						return '*';
"/"						return '/';
"%"						return '%';
"("						return '(';
")"						return ')';
"{"						return '{';
"}"						return '}';
"["						return '[';
"]"						return ']';
","						return ',';
";"						return ';';
"="						return '=';
"<"						return '<';
">"						return '>';
"&"						return '&';
"^"						return '^';
"~"						return '~';
"!"						return '!';
{whitespace}
.
%%
