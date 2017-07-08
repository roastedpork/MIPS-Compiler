%{
#include <iostream>
#include <cstdlib>

int yylex();
int yyerror(const char* s);
%}

%union{
    double num;
    char* string;
}



%token  INTEGERN FLOATN OCTALN HEXADECN IDENTIFIER STRING CHARSEQ SIZEOF NEWLINE TIncre TDecre TLBitShift TRBitShift
%token  TCompLE TCompGE TCompEQ TCompNE TAssignAdd TAssignSub TAssignMul TAssignDiv TAssignMod TAssignLBitShift
%token  TAssignRBitShift TAssignBitAnd TAssignBitOr TAssignBitXor TAssign TBoolAnd TBoolOr TBoolNot TLBracket TRBracket
%token  TLSquare TRSquare TLScope TRScope TCompLT TCompGT TPeriod TComma TColon TSemicolon TAdd TSub TMul TDiv TMod 
%token  TBitAnd TBitNot TBitOr TBitXor TQuestion TPtrAccess THash TEllipsis INVALID 
%token  TAuto TBreak TChar TContinue TDefault TDo TDouble TElse TExtern TFloat TFor TGoto TIf TInt TUnion TConst
%token  TRegister TReturn TShort TSigned TStruct TTypedef TUnsigned TStatic TVoid TVolatile TWhile
%type<string>   function datatype parameter params compound_statement

%token <num> NUM
%token <string> IDENTIFIER

%%

function      : datatype IDENTIFIER parameter compound_statement {std::cout << "FUNCTION : " << $2 << std::endl;}
              ;

datatype      : TInt {$$ = $1;}
              ;

parameter     : TLBracket TRBracket
              | TLBracket params TRBracket  
              ;

params        : datatype IDENTIFIER
              | datatype IDENTIFIER TComma params
              ;

compound_statement  : TLScope TRScope {std::cout << "SCOPE" << std::endl;}
                    ;
%%

int yyerror(const char* s){ 
    std::cout << s << std::endl;
    return -1;
}

int main(void) {
  yyparse();
}