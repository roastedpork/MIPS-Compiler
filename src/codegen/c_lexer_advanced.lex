%option noyywrap
%{
#include "c_parser.tab.h"

%}

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

{ID}                    {yylval.string = strdup(yytext); return IDENTIFIER;}

{STRSEQ}                return STRING;

{CHARSEQ}               return CHARSEQ;


{NONZERO}{DIGIT}*{INTSUF}?      return INTEGERN;

{FLOAT}                         return FLOATN;

{PREOCT}{OCTAL}*{INTSUF}?       return OCTALN;

{PREHEX}{HEXADEC}+{INTSUF}?     return HEXADECN;

"->"                    return TPtrAccess;
"++"                    return TIncre;
"--"                    return TDecre;
"<<"                    return TLBitShift;
">>"                    return TRBitShift;
"<="                    return TCompLE;
">="                    return TCompGE;
"=="                    return TCompEQ;
"!="                    return TCompNE;
"+="                    return TAssignAdd;
"-="                    return TAssignSub;
"*="                    return TAssignMul;
"/="                    return TAssignDiv;
"%="                    return TAssignMod;
"<<="                   return TAssignLBitShift;
">>="                   return TAssignRBitShift;
"&="                    return TAssignBitAnd;
"|="                    return TAssignBitOr;
"^="                    return TAssignBitXor;
"="                     return TAssign;
"&&"                    return TBoolAnd;
"||"                    return TBoolOr;
"!"                     return TBoolNot;
"("                     {yylval.string = strdup("("); return TLBracket;}
")"                     {yylval.string = strdup(")"); return TRBracket;}
"["                     return TLSquare;
"]"                     return TRSquare;
"{"                     return TLScope;
"}"                     return TRScope;
"<"                     return TCompLT;
">"                     return TCompGT;
"."                     return TPeriod;
","                     return TComma;
":"                     return TColon;
";"                     return TSemicolon;
"+"                     return TAdd;
"-"                     return TSub;
"*"                     return TMul;
"/"                     return TDiv;
"%"                     return TMod;
"&"                     return TBitAnd;
"~"                     return TBitNot;
"|"                     return TBitOr;
"^"                     return TBitXor;
"?"                     return TQuestion;
"#"                     return THash;
"..."                   return TEllipsis;

\n                      return NEWLINE;
           
.


%%

