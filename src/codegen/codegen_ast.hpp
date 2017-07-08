#ifndef AST_HPP
#define AST_HPP
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
using namespace std;

class Context;
class Node;
class Declarator;
class Expr;
class Parameter;
class Para_List;
class Direct_Declarator;
class Init_Declarator;
class Init_Decl_List;
class Statement;
class DeclStatement;
class ExprStatement;
class JumpStatement;
class SelStatement;
class WhileStatement;
class ForStatement;
class CompoundStatement;
class Decl_List;
class Stat_List;
class Arg_Expr_List;
class Function;
class Programme;


union dnum{
    double d;
    uint64_t i;
};

union fnum{
    float f;
    uint32_t i;
};

class Context{
public:
    Context();
    ~Context();

    string print();

    void inheritContext(Context* _ctxt);
    void addVarDecl(string _id);
    void addParaDecl(string _id);
    void raiseAlloc(int i);
    void raiseArgCount(int i);
    const int initFrameOffset();
    const string getOffset(string _id);
    void mapVars();
    const int ParaCount();
    int pushStack();
    int popStack();
    string genLabel();
    string genConstLabel();
    void setFuncCall();
    const bool getFuncCall() const;
    void addVarType(string _id, string _var_type);
    string getVarType(string _id);
    void addDeclMap(string _id, string _label);
    string getDeclMap(string _id);
    void setResultType(string _type);
    string getResultType() const;
private:
    map<string, string> offset_map;
    map<string, string> type_map;
    map<string, string> init_map;
    vector<string> var_declared;
    vector<string> para_declared;
    int expr_stack_alloc;
    int expr_stack_offset;
    int arg_count;
    int label;
    int const_label;
    bool func_call;
    string res_type;
};


class Node{
public:
    Node();
    virtual ~Node();
    virtual string print() const;
    virtual void genContext(Context* _ctxt);
    virtual string CodeGen(Context* _ctxt) const;
};

class Declarator : public Node{
public:
    Declarator();
    ~Declarator();
    virtual string print() const;
    virtual void genContext(bool _fromParameter, Context* _ctxt);
    virtual string CodeGen(Context* _ctxt) const;
};

//Expr class
class Expr : public Node{
public:
    Expr(string _type, string _op, Expr* _left, Expr* _right);
    ~Expr();

    string print() const;
    //void evalExpr(); 

    void setType(string _type);
    string getType() const;

    void setOp(string _op);
    string getOp() const;

    void setExpr(Expr* _left, Expr* _right);
    Expr* getLeft() const;
    Expr* getRight() const;

    void setID(string _id);
    string getID() const;
    
    void setValue(int _value);
    const int getValue() const;
    
    void setFValue(float _fvalue);
    const float getFValue() const;

    void setDValue(double _dvalue);
    const double getDValue() const;

    void genContext(Context* _ctxt);
    int genStackCount(int& _max);

    string CodeGen(Context* _ctxt) const;

    void loadArguments(Arg_Expr_List* _args);

    void addArrayElement(Expr* _expr);
    const int getArraySize() const;
    Expr* getArrayElement(int i) const;

    const bool containsFP() const;

private:
    Expr* left;
    Expr* right;
    string op;
    string id;
    string type;
    int value;
    float fvalue;
    double dvalue;
    vector<Expr*> arg_list;
};

class Arg_Expr_List : public Node{
public:
    Arg_Expr_List(Expr* _init_expression);
    ~Arg_Expr_List();

    void addArgument(Expr* _expr);
    const int getCount() const;
    Expr* getArgument(int i) const;

private:
    vector<Expr*> arg_list;
};

//Parameter Subclasses

class Parameter : public Node{
public:
    Parameter(string _decl_type, Direct_Declarator* _decl);
    ~Parameter();
    
    string print();
    void genContext(Context* _ctxt);
private:
    string decl_type;
    Direct_Declarator* decl;
};

class Para_List : public Node {
public:
    Para_List(Parameter* _init_para);
    ~Para_List();

    string print();
    
    const int getCount() const;
    Parameter* getParameter(int i) const;
    void addtoList(Parameter* _para);

    void setEllipsis();
    const bool getEllipsis() const;

private:
    bool ellipsis;
    vector<Parameter*> para_list;
};

//Declarator Classes
class Direct_Declarator : public Declarator{
public:
    Direct_Declarator(string _id);
    ~Direct_Declarator();

    string getID() const;
    void setBrackets();
    void setParameters(Para_List* _para_list);
    string print() const;
    void genContext(bool _fromParameter, Context* _ctxt);
    void setArray();
    const bool isArray() const;
    void setArraySize(Expr* _expr);
    const int getArraySize() const;
private:
    bool brackets;
    bool ellipsis;
    bool array;

    string id;
    int size;
    vector<Parameter*> para_list;
};

class Init_Declarator : public Declarator{
public:
    Init_Declarator(Direct_Declarator* _id, Expr* _init_expression);
    ~Init_Declarator();

    string print() const;
    void genContext(string _source, string _var_type, Context* _ctxt);
    string CodeGen(bool _fromGlobal, string _var_type, Context* _ctxt);
private:
    Direct_Declarator* id;
    Expr* init_expression;
};

class Init_Decl_List : public Node{
public:
    Init_Decl_List(Init_Declarator* _init_decl);
    ~Init_Decl_List();

    string print() const;

    const int getCount() const;
    Init_Declarator* getInitDecl(int i) const;
    void addtoList(Init_Declarator* _decl_list);
private:
    vector<Init_Declarator*> init_decl_list;
};


//Statement classes
class Statement : public Node{
public:
    Statement();
    ~Statement();

    virtual string print(string _indent) const;
    virtual void genContext(Context* _ctxt);
    virtual string CodeGen(Context* _ctxt) const;
};

class DeclStatement : public Statement{
public:
    DeclStatement(string _var_type, Init_Decl_List* _init_decl_list);
    ~DeclStatement();

    string getVarType() const;
    const int getCount() const;
    Init_Declarator* getInitDecl(int i) const;
    virtual string print(string _indent) const;
    void convertType();

    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;

private:
    string var_type;
    vector<Init_Declarator*> decl_list;
};

class GlobalDecl : public Statement{
public:
    GlobalDecl(DeclStatement* _decl);
    ~GlobalDecl();

    string print() const;
    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;
private:
    string var_type;
    vector<Init_Declarator*> decl_list;
};

class ExprStatement : public Statement{
public:
    ExprStatement(Expr* _expression);
    ~ExprStatement();

    string print(string _indent) const;
    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;
    string getCondOp() const;

private:
    Expr* assigned_var;
    Expr* expression;

};

class JumpStatement : public Statement{
public:    
    JumpStatement(Expr* _expression);
    ~JumpStatement();

    string print(string _indent) const;

    const Expr* getExpr() const;
    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;

private:
    Expr* expression;
};

class Decl_List : public Node{
public:
    Decl_List(DeclStatement* _init_decl);
    ~Decl_List();

    const int getCount() const;
    DeclStatement* getDecl(int i) const;
    void addtoList(DeclStatement* _decl);
private:
    vector<DeclStatement*> decl_list;
};

class Stat_List : public Node{
public:
    Stat_List(Statement* _init_stat);
    ~Stat_List();

    const int getCount() const;
    Statement* getStat(int i) const;
    void addtoList(Statement* _stat);
private:
    vector<Statement*> stat_list;
};

class SelStatement : public Statement{
public:
    SelStatement(Expr* _cond, Statement* _if_stat, Statement* _else_stat);
    ~SelStatement();

    string print(string _indent) const;
    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;
private:
    Expr* cond;
    Statement* if_stat;
    Statement* else_stat;
};

class WhileStatement : public Statement{
public:
    WhileStatement(Expr* _cond, Statement* _stat, bool _iswhile);
    ~WhileStatement();

    string print(string _indent) const;
    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;
private:
    Expr* cond;
    Statement* stat;
    bool iswhile;

};

class ForStatement : public Statement{
public:
    ForStatement(ExprStatement* _init, ExprStatement* _end, Expr* _step, Statement* _stat);
    ~ForStatement();

    string print(string _indent) const;
    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;
private:
    ExprStatement* init;
    ExprStatement* end;
    Expr* step;
    Statement* stat; 
};

class CompoundStatement : public Statement{
public:
    CompoundStatement(Decl_List* _decl_list, Stat_List* _stat_list);
    ~CompoundStatement();

    string print(string _indent) const;

    const int getDeclCount() const;
    const int getStatCount() const;
    const DeclStatement* getDecl(int i) const;
    const Statement* getStat(int i) const;

    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;

private:
    vector<DeclStatement*> decl_list;
    vector<Statement*> stat_list;
    //Context* ctxt;
};


//Top-level classes
class Function : public Node{
public:
    Function(string _return_type, Direct_Declarator* _declarator, CompoundStatement* _comp_stat);
    ~Function();

    string print() const;
    void genContext(Context* _ctxt);
    string CodeGen(Context* _ctxt) const;
private:
    string return_type;
    Direct_Declarator* declarator;
    CompoundStatement* comp_stat;
    Context* ctxt;
};


class Programme : public Node{
public:
    Programme(Node* _init_node);
    ~Programme();
    string print() const;
    void addtoList(Node* _node);

    void genContext();
    string CodeGen();

private:
    vector<Node*> prgm_list;
    Context* ctxt;
};

#endif