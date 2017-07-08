%{
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include "codegen_ast.hpp"

int yylex();
int yyerror(const char* s);

Programme* root;

%}

%union{
    double dnum;
    float fnum;
    int   num;
    const char* string;
    class Node*       node;
    class Declarator* declarator_node;
    class Expr*       expr_node;
    class Function*   func_node;
    class Statement*  stat_node;
    class Programme*  prgm_node;
    
    class Parameter*  parameter_node;
    class Para_List*  para_list_node;

    class Direct_Declarator* direct_decl_node;
    class Init_Declarator* init_decl_node;
    class Init_Decl_List* init_decl_list_node;
    class ExprStatement* expr_stat_node;
    class DeclStatement* decl_stat_node;
    class Decl_List* decl_list_node;
    class Stat_List* stat_list_node;
    class Arg_Expr_List* arg_expr_list;
    class SelStatement* sel_stat_node;
    class CompoundStatement* comp_stat_node;
    class JumpStatement* jump_stat_node;
}

%token <num> INTEGERN OCTALN HEXADECN
%token <dnum> FLOATN
%token <string> IDENTIFIER CHARSEQ STRING
%token TInt TReturn TEllipsis TPtrAccess TIncre TDecre TLBitShift TRBitShift TCompLE TCompGE TCompEQ TCompNE
%token TAssignAdd TAssignSub TAssignMul TAssignDiv TAssignMod TAssignLBitShift TAssignRBitShift TAssignBitAnd TAssignBitOr
%token TAssignBitXor TBoolAnd TBoolOr TElse TFor TIf TWhile TDo;
%token TAuto TBreak TChar TContinue TDefault TDouble TExtern TFloat TGoto TUnion TConst TRegister TLong
%token TShort TSigned TStruct TTypedef TUnsigned TStatic TSwitch TVoid TVolatile SIZEOF


%type<string>               type_spec type_qual
%type<prgm_node>            file 
%type<func_node>            function_def 
%type<decl_stat_node>       declaration 
%type<init_decl_node>       init_declarator 
%type<arg_expr_list>        argument_expr_list
%type<comp_stat_node>       comp_statement 
%type<stat_node>            statement  jump_statement sel_statement iter_statement
%type<expr_stat_node>       expr_statement
%type<direct_decl_node>     direct_declarator declarator
%type<parameter_node>       parameter_declaration
%type<init_decl_list_node>  init_decl_list 
%type<para_list_node>       parameter_type_list parameter_list 
%type<decl_list_node>       decl_list 
%type<stat_list_node>       statement_list
%type<expr_node>            constant constantf str_lit
%type<expr_node>            primary_expr postfix_expr unary_expr cast_expr mult_expr add_expr shift_expr relation_expr 
%type<expr_node>            equal_expr and_expr xor_expr or_expr logic_and_expr logic_or_expr cond_expr assign_expr expr
%type<expr_node>            unary_op assign_op initializer constant_expr init_list

%%


file        : file function_def { $1->addtoList($2); $$ = $1; }
            | function_def      { root = new Programme($1); $$ = root; }
            | file declaration  { GlobalDecl* temp = new GlobalDecl($2); $1->addtoList(temp); $$ = $1; }
            | declaration       { GlobalDecl* temp = new GlobalDecl($1); root = new Programme(temp); $$ = root; }
            ;

function_def    : type_spec declarator comp_statement { $$ = new Function($1, $2, $3); }
                ;

type_spec   : TInt          { $$ = "int"; }
            | TVoid         { $$ = "void"; }
            | TChar         { $$ = "char"; }
            | TShort        { $$ = "short"; }
            | TLong         { $$ = "long"; }
            | TFloat        { $$ = "float"; }
            | TDouble       { $$ = "double"; }
            | TSigned       { $$ = "signed"; }
            | TUnsigned     { $$ = "unsigned";}
            ;

type_qual   : TConst        { $$ = "const"; }
            | TVolatile     { $$ = "volatile"; }
            ;

declaration : type_spec init_decl_list ';' {  $$ = new DeclStatement($1, $2); }
            ;

init_decl_list  : init_declarator   { $$ = new Init_Decl_List($1); }
                | init_decl_list ',' init_declarator  { $1->addtoList($3); $$ = $1; }
                ;

init_declarator : declarator { $$ = new Init_Declarator($1, NULL); }
                | declarator '=' initializer { $$ = new Init_Declarator($1, $3); }
                ;

initializer     : assign_expr { $$ = $1; }
                | '{' init_list '}' { $$ = $2; }
                | '{' init_list ',' '}' { $$ = $2; }
                ;

init_list   : initializer   { $$ = new Expr("array", "", NULL, NULL); $$->addArrayElement($1); }
            | init_list ',' initializer { $1->addArrayElement($3); $$ = $1; }
            ;        

declarator      : direct_declarator { $$ = $1; }
                ;

direct_declarator   : IDENTIFIER                                    { $$ = new Direct_Declarator($1); }
                    | direct_declarator '(' parameter_type_list ')' { $1->setParameters($3); $$ = $1; }
                    | direct_declarator '(' ')'                     { $1->setBrackets(); $$ = $1; }
                    | direct_declarator '[' constant_expr ']'       { $1->setArray(); $1->setArraySize($3); $$ = $1; }
                    | direct_declarator '[' ']'                     { $1->setArray(); $$ = $1; }
                    ;

parameter_type_list : parameter_list { $$ = $1; }
                    | parameter_list ',' TEllipsis {$1->setEllipsis(); $$ = $1;}
                    ;

parameter_list      : parameter_declaration { $$ = new Para_List($1); }
                    | parameter_list ',' parameter_declaration { $1->addtoList($3); $$ = $1; }
                    ;

parameter_declaration   : type_spec declarator { $$ = new Parameter($1, $2); }
                        ;


comp_statement  : '{' '}'                       { $$ = new CompoundStatement(NULL, NULL); }
                | '{' statement_list '}'        { $$ = new CompoundStatement(NULL, $2); }  
                | '{' decl_list '}'             { $$ = new CompoundStatement($2, NULL); }
                | '{' decl_list statement_list '}' { $$ = new CompoundStatement($2, $3); }
                ;

decl_list   : declaration   { $$ = new Decl_List($1); }
            | decl_list declaration { $1->addtoList($2); $$ = $1; }
            ;

statement_list  : statement { $$ = new Stat_List($1); }
                | statement_list statement { $1->addtoList($2); $$ = $1; }
                ;

statement   : expr_statement { $$ = $1; }
            | comp_statement { $$ = $1; }
            | jump_statement { $$ = $1; }
            | sel_statement  { $$ = $1; }
            | iter_statement { $$ = $1; }          
            ;

expr_statement  : ';' { $$ = new ExprStatement(NULL); }
                | expr ';' { $$ = new ExprStatement($1); }
                ;

jump_statement  : TReturn ';' { $$ = new JumpStatement(NULL); }
                | TReturn expr ';' { $$ = new JumpStatement($2); }
                ;

sel_statement   : TIf '(' expr ')' statement TElse statement    { $$ = new SelStatement($3, $5, $7); }
                | TIf '(' expr ')' statement                    { $$ = new SelStatement($3, $5, NULL); }
                ;

iter_statement  : TWhile '(' expr ')' statement                                 { $$ = new WhileStatement($3,$5,true); }
                | TDo statement TWhile '(' expr ')' ';'                         { $$ = new WhileStatement($5,$2,false); }
                | TFor '(' expr_statement expr_statement ')' statement          { $$ = new ForStatement($3,$4,NULL,$6);  }
                | TFor '(' expr_statement expr_statement expr ')' statement     { $$ = new ForStatement($3,$4,$5,$7); }
                ;

constant    : INTEGERN  { $$ = new Expr("int", "", NULL, NULL); $$->setValue($1); }
            | OCTALN    { $$ = new Expr("oct", "", NULL, NULL); $$->setValue($1); }
            | HEXADECN  { $$ = new Expr("hex", "", NULL, NULL); $$->setValue($1); }
            ;

constantf   : FLOATN    { $$ = new Expr("double", "", NULL, NULL); $$->setDValue($1); }
            ;

str_lit     : CHARSEQ   { $$ = new Expr("string", "", NULL, NULL); $$->setID($1); }
            | STRING    { $$ = new Expr("char", "", NULL, NULL); $$->setID($1); }
            ;


primary_expr    : IDENTIFIER    { $$ = new Expr("id", "", NULL, NULL); $$->setID($1); }
                | constant      { $$ = $1; }
                | constantf     { $$ = $1; }
                | str_lit       { $$ = $1; }
                | '(' expr ')'  { $$ = $2; }
                ;

postfix_expr    : primary_expr { $$ = $1; }
                | postfix_expr '[' expr ']' { $1->addArrayElement($3); $$ = $1; }
                | postfix_expr TIncre   { $$ = new Expr("unary", "++", $1, NULL); }
                | postfix_expr TDecre   { $$ = new Expr("unary", "--", $1, NULL); }
                | postfix_expr '(' ')' { $1->setType("function"); $$ = $1; }
                | postfix_expr '(' argument_expr_list ')' { $1->setType("function"); $1->loadArguments($3); $$ = $1; }
                ;

argument_expr_list          : assign_expr { $$ = new Arg_Expr_List($1); }
                            | argument_expr_list ',' assign_expr { $1->addArgument($3); $$ = $1; }
                            ;
                            

unary_op    : '&'   { $$ = new Expr("unary", "&", NULL, NULL); }
            | '*'   { $$ = new Expr("unary", "*", NULL, NULL); }
            | '+'   { $$ = new Expr("unary", "+", NULL, NULL); }
            | '-'   { $$ = new Expr("unary", "-", NULL, NULL); }
            | '~'   { $$ = new Expr("unary", "~", NULL, NULL); }
            | '!'   { $$ = new Expr("unary", "!", NULL, NULL); }
            ;

unary_expr  : postfix_expr      { $$ = $1; }
            | TIncre unary_expr { $$ = new Expr("unary", "++", NULL, $2); }
            | TDecre unary_expr { $$ = new Expr("unary", "--", NULL, $2); }
            | unary_op unary_expr   { $1->setExpr(NULL, $2); $$ = $1; }
            ;

cast_expr   : unary_expr { $$ = $1; }
            | '(' type_spec ')' cast_expr { $$ = new Expr("cast", $2, NULL, $4); }
            ;

mult_expr   : cast_expr { $$ = $1; }
            | mult_expr '*' cast_expr   { $$ = new Expr("op", "*", $1, $3); }
            | mult_expr '/' cast_expr   { $$ = new Expr("op", "/", $1, $3); }
            | mult_expr '%' cast_expr   { $$ = new Expr("op", "%", $1, $3); }
            ;

add_expr    : mult_expr { $$ = $1; }
            | add_expr '+' mult_expr    { $$ = new Expr("op", "+", $1, $3); }
            | add_expr '-' mult_expr    { $$ = new Expr("op", "-", $1, $3); }
            ;

shift_expr  : add_expr  { $$ = $1; }
            | shift_expr TLBitShift add_expr { $$ = new Expr("op", "<<", $1, $3); }
            | shift_expr TRBitShift add_expr { $$ = new Expr("op", ">>", $1, $3); }

relation_expr   : shift_expr    { $$ = $1; }
                | relation_expr '<' shift_expr  { $$ = new Expr("op", "<", $1, $3); }
                | relation_expr '>' shift_expr  { $$ = new Expr("op", ">", $1, $3); }
                | relation_expr TCompLE shift_expr  { $$ = new Expr("op", "<=", $1, $3); }
                | relation_expr TCompGE shift_expr  { $$ = new Expr("op", ">=", $1, $3); }
                ;

equal_expr  : relation_expr { $$ = $1; }
            | equal_expr TCompEQ relation_expr  { $$ = new Expr("op", "==", $1, $3); }
            | equal_expr TCompNE relation_expr  { $$ = new Expr("op", "!=", $1, $3); }
            ;

and_expr    : equal_expr    { $$ = $1; }
            | and_expr '&' equal_expr   { $$ = new Expr("op", "&", $1, $3); }
            ;

xor_expr    : and_expr  { $$ = $1; }
            | xor_expr '^' and_expr { $$ = new Expr("op", "^", $1, $3); }
            ;

or_expr     : xor_expr  { $$ = $1; }
            | or_expr '|' xor_expr  { $$ = new Expr("op", "|", $1, $3); }
            ;

logic_and_expr  : or_expr   { $$ = $1; }
                | logic_and_expr TBoolAnd or_expr   { $$ = new Expr("op", "&&", $1, $3); }
                ;

logic_or_expr   : logic_and_expr    { $$ = $1; }
                | logic_or_expr TBoolOr logic_and_expr  { $$ = new Expr("op", "||", $1, $3); }
                ;

cond_expr   : logic_or_expr { $$ = $1; }
            ;

constant_expr   : cond_expr { $$ = $1; }
                ;

assign_expr : cond_expr { $$ = $1; }
            | unary_expr assign_op assign_expr  { $2->setExpr($1, $3); $$ = $2; }
            ;

assign_op   : '='               { $$ = new Expr("assign", "=", NULL, NULL); }
            | TAssignAdd        { $$ = new Expr("assign", "+=", NULL, NULL); }
            | TAssignSub        { $$ = new Expr("assign", "-=", NULL, NULL); }
            | TAssignMul        { $$ = new Expr("assign", "*=", NULL, NULL); }
            | TAssignDiv        { $$ = new Expr("assign", "/=", NULL, NULL); }
            | TAssignMod        { $$ = new Expr("assign", "%=", NULL, NULL); }
            | TAssignLBitShift  { $$ = new Expr("assign", "<<=", NULL, NULL); }
            | TAssignRBitShift  { $$ = new Expr("assign", ">>=", NULL, NULL); }
            | TAssignBitAnd     { $$ = new Expr("assign", "&=", NULL, NULL); }
            | TAssignBitOr      { $$ = new Expr("assign", "|=", NULL, NULL); }
            | TAssignBitXor     { $$ = new Expr("assign", "^=", NULL, NULL); }
            ;   

expr    : assign_expr  { $$ = $1; }
        ;
            
%%


int yyerror(const char* s){ 
    std::cout << s << std::endl;
    return -1;
}

int main(void) {
    yyparse();

    root->genContext();
    cout << root->print();
    cout << root->CodeGen();

}
