#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <stdint.h>
#include "codegen_ast.hpp"

using namespace std;

Context::Context(){
    expr_stack_alloc = 0;
    arg_count = 0;
    label = 2;
    const_label = 0;
    func_call = false;
}

Context::~Context(){
}

const int Context::initFrameOffset(){
    int count = 2;
    if(func_call){
        count += 2;
        if(arg_count <= 4) count += 4;
        else count += (arg_count+1)/2*2;
    }
    else if(var_declared.size() != 0) count += 2;

    int fp_count = 0;
    for(int i = 0; i < var_declared.size(); i++){
        if(type_map[var_declared[i]] != "int") fp_count += 1;
    }

    count += (var_declared.size()+fp_count+1)/2*2;
    count += expr_stack_alloc;

    return count *4;
}

void Context::mapVars(){
    int temp = this->initFrameOffset();
    stringstream ss;
    for(int i = 0; i < para_declared.size(); i++){
        ss << (temp + 4*i);
        offset_map[para_declared[i]] = ss.str();
        ss.str(string());
    }
    
    if(arg_count == 0){
        temp = 8;
        expr_stack_offset = ((var_declared.size()+1)/2*2 + 1)*4;
    }
    else if(arg_count <= 4){
        temp = 24;
        expr_stack_offset = ((var_declared.size()+1)/2*2 + 6)*4;
    }
    else{
        temp = ((arg_count+1)/2*2 + 2) * 4;
        expr_stack_offset = ((var_declared.size()+1)/2*2 + 2 + (arg_count+1)/2*2)*4;
    } 
    for(int i = 0; i < var_declared.size(); i++){
        ss << temp;
        if(type_map[var_declared[i]] != "int") temp += 8;
        else temp += 4;
        offset_map[var_declared[i]] = ss.str();
        ss.str(string());
    }
}

string Context::print(){
    stringstream ss;
    ss << "function call present: ";
    if(func_call) ss << "true\n";
    else ss << "false\n";
    ss << "max expr stack needed: " << expr_stack_alloc << endl;
    ss << "parameters used in scope: "<< endl;
    for(int i= 0; i < para_declared.size(); i++){
        ss << para_declared[i] << endl;
    }
    ss << "variables declared in scope: "<< endl;
    for(int i= 0; i < var_declared.size(); i++){
        ss << var_declared[i] << endl;
    }

        this->mapVars();
    ss << "variable mapping:" << endl;
    for(map<string,string>::iterator it = offset_map.begin(); it != offset_map.end(); it++){
        ss << it->first << ": " << it->second << endl;
    }
    ss << "var_type mapping:" << endl;
    
    for(map<string,string>::iterator it = type_map.begin(); it != type_map.end(); it++){
        ss << it->first << ": " << it->second << endl;
    }

    ss << "float const mapping:" << endl;
    
    for(map<string,string>::iterator it = init_map.begin(); it != init_map.end(); it++){
        ss << it->first << ": " << it->second << endl;
    }
    return ss.str();
}

void Context::inheritContext(Context* _ctxt){
    for(int i = 0; i < _ctxt->var_declared.size(); i++){
        var_declared.push_back(_ctxt->var_declared[i]);
    }
}

void Context::addVarDecl(string _id){
    bool found = false;

        for (int i = 0; i < var_declared.size(); i++){
        if(_id == var_declared[i]){
            found = true;
            break;
        }
    }
    if(!found) var_declared.push_back(_id);
}

void Context::addParaDecl(string _id){
    bool found = false;

        for (int i = 0; i < para_declared.size(); i++){
        if(_id == para_declared[i]){
            found = true;
            break;
        }
    }
    if(!found) para_declared.push_back(_id);
}

void Context::raiseAlloc(int i){
    if(i > expr_stack_alloc) expr_stack_alloc = i;
}

void Context::raiseArgCount(int i){
    if(i > arg_count) arg_count = i;
}

const string Context::getOffset(string _id){
    return offset_map[_id];
}

const int Context::ParaCount(){
    return para_declared.size();
}

int Context::pushStack(){
    expr_stack_offset += 4;
    return expr_stack_offset -4;
}

int Context::popStack(){
    expr_stack_offset -= 4;
    return expr_stack_offset;
}

string Context::genLabel(){
    stringstream ss;
    ss << "L" << label;
    label++;
    return ss.str();
}

string Context::genConstLabel(){
    stringstream ss;
    ss << "$LC" << const_label;
    const_label++;
    return ss.str();
}

void Context::setFuncCall(){
    func_call = true;
}

const bool Context::getFuncCall() const{
    return func_call;
}

void Context::addVarType(string _id, string _var_type){
    type_map[_id] = _var_type;
}

string Context::getVarType(string _id){
    return type_map[_id];
}

void Context::addDeclMap(string _id, string _label){
    init_map[_label] = _id;
}
string Context::getDeclMap(string _label){
    return init_map[_label];
}

void Context::setResultType(string _type){
    res_type = _type;
}

string Context::getResultType() const{
    return res_type;
}

Node::Node(){
}

Node::~Node(){
}

string Node::print() const{
    string temp = "";
    return temp;
}

void Node::genContext(Context* _ctxt){
}

string Node::CodeGen(Context* _ctxt) const{
    string temp = "";
    return temp;
}

//Expression class functions
Expr::Expr(string _type, string _op, Expr* _left, Expr* _right) :
    type(_type), op(_op), left(_left), right(_right) {

}

Expr::~Expr(){
}

string Expr::print() const{
    stringstream ss;
    if((left == NULL) && (right == NULL)){
        if (type == "function") {
            ss << id << "(";
            if(arg_list.size() != 0){
                for(int i = 0; i < arg_list.size()-1; i++){
                    ss << arg_list[i]->print() << ", ";
                }
                ss << arg_list[arg_list.size()-1]->print();
            }
            ss << ")";
        }
        else if(type == "array"){
            ss << "{";
            for(int i = 0; i < arg_list.size()-1; i++){
                ss << arg_list[i]->print() << ", ";
            }
            if(arg_list.size() != 0){
                ss << arg_list[arg_list.size()-1]->print();
            }
            ss << "}";
        }
        else if(type == "id") {
            ss << id;
            if(arg_list.size() != 0) ss << "[" << arg_list[0]->print() << "]";
        }
        else if(type == "int") ss << value;
        else if(type == "oct") ss << value;
        else if(type == "hex") ss << value;
        else if(type == "float") ss << fvalue;
        else if(type == "double") ss << dvalue;
        else if(type == "char") ss << id;
        else if(type == "string") ss << id;
    }

        else if(type == "unary"){
        if(left == NULL) ss << op << "(" << right->print() << ")";
        else ss << "(" << left->print() << ")" << op;
    }
    
    else if(type == "op") ss << "(" << left->print() << " " << op << " " << right->print() << ")";
    else if(type == "assign") ss << "(" << left->print() << " " << op << " " << right->print() << ")";
    else if (type == "cast") ss << "(" << op << ")" << right->print();
    return ss.str();
}

void Expr::setType(string _type){
    type = _type;
}

string Expr::getType() const{
    return type;
}

void Expr::setOp(string _op){
    op = _op;
}

string Expr::getOp() const{
    return op;
}

void Expr::setExpr(Expr* _left, Expr* _right){
    left = _left;
    right = _right;
}

Expr* Expr::getLeft() const{
    return left;
}

Expr* Expr::getRight() const{
    return right;
}

void Expr::setID(string _id){
    id = _id;
}

string Expr::getID() const{
    if(arg_list.size() != 0){
        stringstream ss;
        if((arg_list[0]->getType() == "int")||(arg_list[0]->getType() == "oct")||(arg_list[0]->getType() == "hex")){
            ss << id << "[" << arg_list[0]->getValue() << "]";
            return ss.str();
        }
        else return id;
    }
    return id;
}

void Expr::setValue(int _value){
    value = _value;
}

const int Expr::getValue() const{
    return value;
}

void Expr::setFValue(float _fvalue){
    fvalue = _fvalue;
}

const float Expr::getFValue() const{
    return fvalue;
}

void Expr::setDValue(double _dvalue){
    dvalue = _dvalue;
}

const double Expr::getDValue() const{
    return dvalue;
}

void Expr::genContext(Context* _ctxt){
    bool leftisint = false, rightisint = false; 
    bool leftisval = false, rightisval = false;
    if(left != NULL) {
        left->genContext(_ctxt);
        string nodetype = left->getType();
        leftisint = ((nodetype == "int")||(nodetype == "oct")||(nodetype == "hex"));
        leftisval = ((nodetype == "double")||(nodetype == "float"));
    }
    if(right != NULL) {
        right->genContext(_ctxt);
        string nodetype = right->getType();
        rightisint = ((nodetype == "int")||(nodetype == "oct")||(nodetype == "hex"));
        rightisval = ((nodetype == "double")||(nodetype == "float"));

    }
    if((type == "op") && leftisint && rightisint){
        if(op == "+") value = left->getValue() + right->getValue();
        else if(op == "-") value = left->getValue() - right->getValue(); 
        else if(op == "*") value = left->getValue() * right->getValue(); 
        else if(op == "/") value = left->getValue() / right->getValue(); 
        else if(op == "%") value = left->getValue() % right->getValue();
        else if(op == "<<") value = left->getValue() << right->getValue();
        else if(op == ">>") value = left->getValue() >> right->getValue();
        type = "int";
        op = "";
        delete left;
        left = NULL;
        delete right;
        right = NULL;
    }
    else if((type == "op") && (leftisint||leftisval) && (rightisint||rightisval)){
        double lhs, rhs;
        
        if(left->getType() == "double") lhs = left->getDValue();
        else if(left->getType() == "float") lhs = left->getFValue();
        else if(left->getType() == "int") lhs = left->getValue();

        if(right->getType() == "double") rhs = right->getDValue();
        else if(right->getType() == "float") rhs = right->getFValue();
        else if(right->getType() == "int") rhs = right->getValue();

        if(op == "+") dvalue = lhs + rhs;
        else if(op == "-") dvalue = lhs - rhs; 
        else if(op == "*") dvalue = lhs * rhs; 
        else if(op == "/") dvalue = lhs / rhs; 
        type = "double";
        op = "";
        id = _ctxt->genConstLabel();
        delete left;
        left = NULL;
        delete right;
        right = NULL;
    }
    else if(type == "function"){
        _ctxt->setFuncCall();
        for(int i = 0; i < arg_list.size(); i++){
            arg_list[i]->genContext(_ctxt);

            int max = 0;
            arg_list[i]->genStackCount(max);
            _ctxt->raiseAlloc(max);
        }
        _ctxt->raiseArgCount(arg_list.size());
    }
    else if(type == "array"){ // denotes array list
        for(int i = 0; i < arg_list.size(); i++){
            arg_list[i]->genContext(_ctxt);

            int max = 0;
            arg_list[i]->genStackCount(max);
            _ctxt->raiseAlloc(max);
        }
    }
    else if(arg_list.size() != 0){ // denotes identifier accessing specific element
        arg_list[0]->genContext(_ctxt);

        int max = 0;
        arg_list[0]->genStackCount(max);
        
        _ctxt->raiseAlloc(max);
    }
    else if((type == "float")||(type == "double")){
        id = _ctxt->genConstLabel();
    }
}

int Expr::genStackCount(int& _max){
    int leftcount = 0;
    int rightcount = 0;
    if(left != NULL) leftcount = left->genStackCount(_max);
    if(right != NULL) rightcount = right->genStackCount(_max);

    if(_max < leftcount) _max = leftcount;
    if(_max < rightcount) _max = rightcount;
    if((right != NULL) && (right->getType() == "op")){
        //_max += 1;
        return 1+rightcount;
    }
    else if((right != NULL) && (right->getType() == "function")) _max += 1;
    return 0;
}

string Expr::CodeGen(Context* _ctxt) const{
    stringstream ss;
    if((type == "int") || (type == "oct") || (type == "hex")){
        ss << "    li\t\t$2," << value << endl;
    }
    else if(type == "float"){
        fnum fval;
        fval.f = fvalue;
        cout << "    .align\t2\n";
        cout << id << ":\n";
        cout << "    .word\t" << fval.i << "\n";
        ss << "    lui\t\t$2,%hi(" << id << ")\n";
        ss << "    lwc1\t$f0,%lo(" << id << ")($2)\n";
        ss << "    swc1\t$f0," << _ctxt->getOffset(_ctxt->getDeclMap(id)) <<"($fp)\n";
    }
    else if(type == "double"){
        dnum dval;
        dval.d = dvalue;
        cout << "    .align\t3\n";
        cout << id << ":\n";
        cout << "    .word\t" << (dval.i >> 32) << "\n";
        cout << "    .word\t" << ((dval.i << 32) >> 32) << "\n";
        ss << "    lui\t\t$2,%hi(" << id << ")\n";
        ss << "    ldc1\t$f0,%lo(" << id << ")($2)\n";
        ss << "    sdc1\t$f0," <<  _ctxt->getOffset(_ctxt->getDeclMap(id))  <<"($fp)\n";
    }
    else if(type == "id"){
        if(arg_list.size() != 0){
            if((arg_list[0]->getType() == "int") || (arg_list[0]->getType() == "oct") || (arg_list[0]->getType() == "hex")){
                stringstream offset;
                offset << id << "[" << arg_list[0]->getValue() << "]";
                ss << "    lw\t\t$2," << _ctxt->getOffset(offset.str()) << "($fp)\n";
            }
            else{
                ss << arg_list[0]->CodeGen(_ctxt);
                ss << "    sll\t\t$2,$2,2\n";
                //right now $2 contains the offset wrt a[0];
                stringstream offset;
                offset << id << "[0]";
                ss << "    addiu\t$3,$fp," << _ctxt->getOffset(offset.str()) << "\n";
                ss << "    addu\t$2,$2,$3\n";
                ss << "    lw\t\t$2,0($2)\n";
            }
        }
        else{
            ss << "    lw\t\t$2," << _ctxt->getOffset(id) << "($fp)" << endl;
        }
    }
    else if(type == "op"){
        bool fp = (_ctxt->getResultType() != "int");

        if(fp){
            string res_type = _ctxt->getResultType();
            //load LHS
            if(res_type == "float"){
                if(left != NULL){
                    if((left->getType() == "int")||(left->getType() == "oct")||(left->getType() == "hex")){
                        //ss << "    li\t\t$2," << left->getValue() << endl;
                    }
                    else if(left->getType() == "float"){

                    }
                    else if (left->getType() == "double"){
                        //ss << "    lwc1\t$f0," << _ctxt->getDeclMap(left->getID());
                    }
                    else if(left->getType() == "id"){
                        ss << "    lwc1\t$f0," << _ctxt->getOffset(left->getID()) << "($fp)" << endl;
                    }
                    else if((left->getType() == "op")||(left->getType() == "function")) {
                        //ss << left->CodeGen(_ctxt);
                    }
                }
                if(right != NULL){
                    if((right->getType() == "int")||(right->getType() == "oct")||(right->getType() == "hex")){

                    }
                    else if(right->getType() == "float"){

                    }
                    else if (right->getType() == "double"){
                        //ss << "    lwc1\t$f0," << _ctxt->getDeclMap(right->getID());
                    }
                    else if(right->getType() == "id"){
                        ss << "    lwc1\t$f2," << _ctxt->getOffset(right->getID()) << "($fp)" << endl;
                    }
                    else if((right->getType() == "op")||(right->getType() == "function")) {
                        //ss << "    sw\t\t$2," << _ctxt->pushStack() <<"($fp)\n";
                        //ss << right->CodeGen(_ctxt);
                        //ss << "    move\t$3,$2\n";
                        //ss << "    lw\t\t$2," << _ctxt->popStack() << "($fp)\n";
                    }
                }
                if(op == "+") ss << "    add.s\t$f0,$f0,$f2\n";
                else if(op == "-") ss << "    sub.s\t$f0,$f0,$f2\n";
                else if(op == "*") ss << "    mul.s\t$f0,$f0,$f2\n";
                else if(op == "/") ss << "    div.s\t$f0,$f0,$f2\n";

            }
            else if(res_type == "double"){
                if(left != NULL){
                    if((left->getType() == "int")||(left->getType() == "oct")||(left->getType() == "hex")){
                        //ss << "    li\t\t$2," << left->getValue() << endl;
                    }
                    else if(left->getType() == "float"){

                    }
                    else if (left->getType() == "double"){
                        ss << "    ldc1\t$f0," << _ctxt->getDeclMap(left->getID());
                    }
                    else if(left->getType() == "id"){
                        ss << "    ldc1\t$f0," << _ctxt->getOffset(left->getID()) << "($fp)" << endl;
                    }
                    else if((left->getType() == "op")||(left->getType() == "function")) {
                        //ss << left->CodeGen(_ctxt);
                    }
                }
                if(right != NULL){
                    if((right->getType() == "int")||(right->getType() == "oct")||(right->getType() == "hex")){

                    }
                    else if(right->getType() == "float"){

                    }
                    else if (right->getType() == "double"){
                        //ss << "    ldc1\t$f0," << _ctxt->getDeclMap(right->getID());
                    }
                    else if(right->getType() == "id"){
                        ss << "    ldc1\t$f2," << _ctxt->getOffset(right->getID()) << "($fp)" << endl;
                    }
                    else if((right->getType() == "op")||(right->getType() == "function")) {
                        //ss << "    sw\t\t$2," << _ctxt->pushStack() <<"($fp)\n";
                        //ss << right->CodeGen(_ctxt);
                        //ss << "    move\t$3,$2\n";
                        //ss << "    lw\t\t$2," << _ctxt->popStack() << "($fp)\n";
                    }
                }
                if(op == "+") ss << "    add.d\t$f0,$f0,$f2\n";
                else if(op == "-") ss << "    sub.d\t$f0,$f0,$f2\n";
                else if(op == "*") ss << "    mul.d\t$f0,$f0,$f2\n";
                else if(op == "/") ss << "    div.d\t$f0,$f0,$f2\n";

            }
            else if(res_type == "int"){

            }

        }
        else{
            cout << "hihi" << endl;
            //load LHS
            if(left != NULL){
                if((left->getType() == "int")||(left->getType() == "oct")||(left->getType() == "hex")){
                    ss << "    li\t\t$2," << left->getValue() << endl;
                }
                else if(left->getType() == "id"){
                    ss << "    lw\t\t$2," << _ctxt->getOffset(left->getID()) << "($fp)" << endl;
                }
                else if((left->getType() == "op")||(left->getType() == "function")) {
                    ss << left->CodeGen(_ctxt);
                }
            }
            //load RHS
            if(right != NULL){
                if((right->getType() == "int")||(right->getType() == "oct")||(right->getType() == "hex")){
                    ss << "    li\t\t$3," << right->getValue() << endl;
                }
                else if(right->getType() == "id"){
                    ss << "    lw\t\t$3," << _ctxt->getOffset(right->getID()) << "($fp)" << endl;
                }
                else if((right->getType() == "op")||(right->getType() == "function")) {
                    ss << "    sw\t\t$2," << _ctxt->pushStack() <<"($fp)\n";
                    ss << right->CodeGen(_ctxt);
                    ss << "    move\t$3,$2\n";
                    ss << "    lw\t\t$2," << _ctxt->popStack() << "($fp)\n";
                }
            }
            //perform op and move result onto $2
            if(op == "+") ss << "    addu\t$2,$2,$3\n";
            else if(op == "-") ss << "    subu\t$2,$2,$3\n";
            else if(op == "*") ss << "    mul\t\t$2,$2,$3\n";
            else if(op == "/"){
                ss << "    div\t\t$0,$2,$3\n";
                ss << "    mflo\t$2\n";
            }
            else if(op == "%"){
                ss << "    div\t\t$0,$2,$3\n";
                ss << "    mfhi\t$2\n";
            }

            else if(op == ">") ss << "    slt\t\t$2,$3,$2\n";
            else if(op == "<") ss << "    slt\t\t$2,$2,$3\n";
            else if(op == "<=") ss << "    slt\t\t$2,$3,$2\n";
            else if(op == ">=") ss << "    slt\t\t$2,$2,$3\n";
            else if(op == "<<") ss << "    sll\t\t$2,$2,$3\n";
            else if(op == ">>") ss << "    sra\t\t$2,$2,$3\n";
            else if(op == "&") ss << "    and\t\t$2,$2,$3\n";
            else if(op == "|") ss << "    or\t\t$2,$2,$3\n";
            else if(op == "^") ss << "    xor\t\t$2,$2,$3\n";
        }
    }
    else if(type == "unary"){
        if(op == "-"){
            ss << right->CodeGen(_ctxt);
            ss << "    subu\t$2,$0,$2\n";
        }
        else if((op == "++")||(op == "--")){
            if((left != NULL) && (left->getType() == "id")){
                ss << "    lw\t\t$2," << _ctxt->getOffset(left->getID()) << "($fp)\n";
                if(op == "++") ss << "    addi\t$2,$2,1\n";
                else ss << "    subi\t$2,$2,1\n";
                ss << "    sw\t\t$2," << _ctxt->getOffset(left->getID()) << "($fp)\n";
            }
            else if ((right != NULL) && (right->getType() == "id")){
                ss << "    lw\t\t$2," << _ctxt->getOffset(right->getID()) << "($fp)\n";
                if(op == "++") ss << "    addi\t$2,$2,1\n";
                else ss << "    subi\t$2,$2,1\n";
                ss << "    sw\t\t$2," << _ctxt->getOffset(right->getID()) << "($fp)\n";
            }
        }
        else if(op == "~"){
            ss << right->CodeGen(_ctxt);
            ss << "    nor\t\t$2,$0,$2\n";
        }
    }
    else if(type == "function"){
        int args;
        if (arg_list.size() <= 4){
            args = arg_list.size();
        }
        else args = 4;

        for(int i = 0; i < args; i++){
            ss << arg_list[i]->CodeGen(_ctxt);
            ss << "    move\t$" << (i+4) << ",$2\n";
        }
        if(arg_list.size() > 4){
            for(int i = 4; i < arg_list.size(); i++){
                ss << arg_list[i]->CodeGen(_ctxt);
                ss << "    lw\t\t$2," << (i*4) << "($fp)\n";
            }
        }

        ss << "    .option\tpic0\n";
        ss << "    jal\t\t" << id << endl;
        ss << "    nop\n\n";
        ss << "    .option\tpic2\n";
    }


    return ss.str();
}

void Expr::loadArguments(Arg_Expr_List* _args){
    int size = _args->getCount();
    for(int i = 0; i < size; i++){
        arg_list.push_back(_args->getArgument(i));
    }
    delete _args;
}

void Expr::addArrayElement(Expr* _expr){
    arg_list.push_back(_expr);
}
const int Expr::getArraySize() const{
    return arg_list.size();
}
Expr* Expr::getArrayElement(int i) const{
    return arg_list[i];
}

const bool Expr::containsFP() const{
    bool contains = false;
    if(left != NULL) contains |= left->containsFP();
    if(right != NULL) contains |= right->containsFP();
    contains |= (type == "float")||(type == "double");
    return contains;
}

Arg_Expr_List::Arg_Expr_List(Expr* _init_expression){
    arg_list.push_back(_init_expression);
}

Arg_Expr_List::~Arg_Expr_List(){
}

void Arg_Expr_List::addArgument(Expr* _expr){
    arg_list.push_back(_expr);
}

const int Arg_Expr_List::getCount() const{
    return arg_list.size();
}

Expr* Arg_Expr_List::getArgument(int i) const{
    return arg_list[i];
}



//Parameter class functions
Parameter::Parameter(string _decl_type, Direct_Declarator* _decl) :
    decl_type(_decl_type), decl(_decl){
}
Parameter::~Parameter(){
    delete decl;
}

string Parameter::print(){
    stringstream ss;
    ss << decl_type << " " << decl->print();
    return ss.str();
}

Para_List::Para_List(Parameter* _init_para){
    para_list.push_back(_init_para);
    ellipsis = false;
}
Para_List::~Para_List(){
}

const int Para_List::getCount() const{
    return para_list.size();
}

Parameter* Para_List::getParameter(int i) const{
    return para_list[i];
}

void Parameter::genContext(Context* _ctxt){
    decl->genContext(true, _ctxt);
}

void Para_List::setEllipsis(){
    ellipsis = true;
}

const bool Para_List::getEllipsis() const{
    return ellipsis;
}

void Para_List::addtoList(Parameter* _para){
    para_list.push_back(_para);
}

string Para_List::print(){
    stringstream ss;
    vector<Parameter*>::iterator it = para_list.begin();

        if(para_list.size() != 0){
        while(it != para_list.end()){
            vector<Parameter*>::iterator it2 = it;
            it2++;
            if(it2 == para_list.end()){
                ss << (*it)->print();
            }
            else{ 
                ss << (*it)->print() << ", ";
            }
            it++;
        }
    }

        if (ellipsis) ss << ", ...";
    return ss.str();
}

//Declarator class functions

Declarator::Declarator(){
}

Declarator::~Declarator(){
}

string Declarator::print() const{
    string temp = "";
    return temp;
}

void Declarator::genContext(bool _fromParameter, Context* _ctxt){
}

string Declarator::CodeGen(Context* _ctxt) const{
    string temp = "";
    return temp;
}

Direct_Declarator::Direct_Declarator(string _id) : id(_id) {
    ellipsis = false;
    brackets = false;
    array = false;
    size = 0;
}

Direct_Declarator::~Direct_Declarator(){
    for (int i = 0; i < para_list.size(); i++){
        delete para_list[i];
    }
}

string Direct_Declarator::getID() const{
    return id;
}

void Direct_Declarator::setBrackets(){
    brackets = true;
}

void Direct_Declarator::setParameters(Para_List* _para_list){
    int size = _para_list->getCount();
    for(int i = 0; i < size; i++){
        para_list.push_back(_para_list->getParameter(i));
    }
    brackets = true;
    ellipsis = _para_list->getEllipsis();
    delete _para_list;
}

string Direct_Declarator::print() const{
    stringstream ss;
    ss << id;
    if(array){
        ss << "[";
        if (size != 0) ss << size;
        ss << "]";
    }
    if(brackets){
        ss << "(";

        vector<Parameter*>::const_iterator it = para_list.begin();

        while(it != para_list.end()){
            ss << (*it)->print();

            vector<Parameter*>::const_iterator it2 = it;
            it2++;
            if(it2 != para_list.end()) ss << ", ";
                it++;
        }
        if(ellipsis) ss << ", ...";
        ss << ")";
    }
    return ss.str();
}

void Direct_Declarator::genContext(bool _fromParameter, Context* _ctxt){
    if(_fromParameter){
        _ctxt->addParaDecl(id);
    }
    else{
        for(int i = 0; i < para_list.size(); i++){
            para_list[i]->genContext(_ctxt);
        }
    }
}

void Direct_Declarator::setArray(){
    array = true;
}

const bool Direct_Declarator::isArray() const{
    return array;
}

void Direct_Declarator::setArraySize(Expr* _expr){
    if(_expr->getType() == "int") size = _expr->getValue();
}

const int Direct_Declarator::getArraySize() const{
    return size;
}

Init_Declarator::Init_Declarator(Direct_Declarator* _id, Expr* _init_expression) :
    id(_id), init_expression(_init_expression) {
}

Init_Declarator::~Init_Declarator(){
    delete id;
    delete init_expression;
}

string Init_Declarator::print() const{
    stringstream ss;
    ss << id->print();
    if(init_expression != NULL) ss << " = " << init_expression->print();
    return ss.str();
}

void Init_Declarator::genContext(string _source, string _var_type, Context* _ctxt){
    if(init_expression != NULL){
        init_expression->genContext(_ctxt);
        int max = 0;
        if(init_expression->getType() == "array"){
            for(int i = 0; i < init_expression->getArraySize(); i++){
                max = 0;
                init_expression->getArrayElement(i)->genStackCount(max);
                _ctxt->raiseAlloc(max);
            }
        }
        else{
            init_expression->genStackCount(max);
            _ctxt->raiseAlloc(max);
        } 
    }
    if(_source == "parameter") _ctxt->addParaDecl(id->getID());
    else if(_source == "declaration"){
        if(id->isArray()){
            if(id->getArraySize() != 0){
                for(int i = 0; i < id->getArraySize(); i++){
                    stringstream ss;
                    ss << id->getID() << "[" << i << "]";
                    _ctxt->addVarDecl(ss.str());
                }
            }
            else{
                for(int i = 0; i < init_expression->getArraySize(); i++){
                    stringstream ss;
                    ss << id->getID() << "[" << i << "]";
                    _ctxt->addVarDecl(ss.str());
                }
            }
        }
        else if(init_expression != NULL){
            string expr_type = init_expression->getType();

            if((expr_type == "double")&&(_var_type == "float")){
                _ctxt->addVarDecl(id->getID());
                _ctxt->addVarType(id->getID(), _var_type);

                if(init_expression != NULL){
                    float temp = init_expression->getDValue();
                    init_expression->setType("float");
                    init_expression->setDValue(0);
                    init_expression->setFValue(temp);
                        
                    _ctxt->addDeclMap(id->getID(), init_expression->getID());
                }

            }
            else if((expr_type == "int")&&(_var_type == "float")){
                _ctxt->addVarDecl(id->getID());
                _ctxt->addVarType(id->getID(), _var_type);
                string const_label = _ctxt->genConstLabel();

                if(init_expression != NULL){
                    float temp = init_expression->getValue();
                    init_expression->setType("float");
                    init_expression->setValue(0);
                    init_expression->setFValue(temp);
                    init_expression->setID(const_label);

                    _ctxt->addDeclMap(id->getID(), const_label);
                }
            }
            else if((expr_type == "int")&&(_var_type == "double")){
                _ctxt->addVarDecl(id->getID());
                _ctxt->addVarType(id->getID(), _var_type);
                string const_label = _ctxt->genConstLabel();

                if(init_expression != NULL){
                double temp = init_expression->getValue();
                    init_expression->setType("double");
                    init_expression->setValue(0);
                    init_expression->setDValue(temp);
                    init_expression->setID(const_label);

                    _ctxt->addDeclMap(id->getID(), const_label);
                }
            }

            else if((expr_type == "double")&&(_var_type == "int")){
                _ctxt->addVarDecl(id->getID());
                _ctxt->addVarType(id->getID(), _var_type);

                if(init_expression != NULL){
                    int temp = init_expression->getDValue();
                    init_expression->setType("int");
                    init_expression->setDValue(0);
                    init_expression->setValue(temp);
                    init_expression->setID("");
                }
            }

            else{
                _ctxt->addVarDecl(id->getID());
                _ctxt->addVarType(id->getID(), _var_type);
                if((_var_type != "int") &&(init_expression != NULL)){
                    _ctxt->addDeclMap(id->getID(), init_expression->getID());
                }
            } 
        }
        else{
            _ctxt->addVarDecl(id->getID());
            _ctxt->addVarType(id->getID(), _var_type);
            if((_var_type != "int") &&(init_expression != NULL)){
                _ctxt->addDeclMap(id->getID(), init_expression->getID());
            }
        } 
    }
}

string Init_Declarator::CodeGen(bool _fromGlobal, string _var_type, Context* _ctxt){
    stringstream ss;
    string name = id->getID();
    string expr_type;
    if(init_expression != NULL) expr_type = init_expression->getType();
    

    if(_fromGlobal){
        ss << name << ":\n";
        ss << "    .word\t" << init_expression->getValue() << endl;
    }
    else if(expr_type == "array"){
        int alloc;
        if(id->getArraySize() >= init_expression->getArraySize()) alloc = id->getArraySize();
        else alloc = init_expression->getArraySize();

        for(int i = 0; i < alloc; i++){
            stringstream ss2;
            ss2 << id->getID() << "[" << i << "]";
            ss << "    sw\t\t$0," << _ctxt->getOffset(ss2.str()) << "($fp)\n";
        }

        for(int i = 0; i < init_expression->getArraySize(); i++){
            stringstream ss2;
            ss2 << id->getID() << "[" << i << "]";
            ss << init_expression->getArrayElement(i)->CodeGen(_ctxt);
            ss << "    sw\t\t$2," << _ctxt->getOffset(ss2.str()) << "($fp)\n";
        }
    }
    else{
        string offset = _ctxt->getOffset(name);
        if(init_expression != NULL) ss << init_expression->CodeGen(_ctxt);
        if(_var_type == "int") ss << "    sw\t\t$2," << offset << "($fp)\n";
    }
    return ss.str();
}

Init_Decl_List::Init_Decl_List(Init_Declarator* _init_decl){
    init_decl_list.push_back(_init_decl);
}

Init_Decl_List::~Init_Decl_List(){
}

const int Init_Decl_List::getCount() const{
    return init_decl_list.size();
}

Init_Declarator* Init_Decl_List::getInitDecl(int i) const{
    return init_decl_list[i];
}

void Init_Decl_List::addtoList(Init_Declarator* _decl_list){
    init_decl_list.push_back(_decl_list);
}

string Init_Decl_List::print() const{
    stringstream ss;
    vector<Init_Declarator*>::const_iterator it = init_decl_list.begin();

        if(init_decl_list.size() != 0){
        while(it != init_decl_list.end()){
            vector<Init_Declarator*>::const_iterator it2 = it;
            it2++;
            if(it2 == init_decl_list.end()){
                ss << (*it)->print();
            }
            else{
                ss << (*it)->print() << ", ";
            }
            it++;
        }
    }
    return ss.str();
}

//Statement class functions
Statement::Statement(){
}

Statement::~Statement(){
}

string Statement::print(string _indent) const{
    string temp = "";
    return temp;
}

void Statement::genContext(Context* _ctxt){
}

string Statement::CodeGen(Context* _ctxt) const{
    string temp = "";
    return temp;
}

DeclStatement::DeclStatement(string _var_type, Init_Decl_List* _init_decl_list) : var_type(_var_type) {
    int size = _init_decl_list->getCount();
    for(int i = 0; i < size; i++){
        decl_list.push_back(_init_decl_list->getInitDecl(i));
    }
    delete _init_decl_list;
}

DeclStatement::~DeclStatement(){
}

string DeclStatement::print(string _indent) const{
    stringstream ss;
    ss << _indent << var_type << " ";

        vector<Init_Declarator*>::const_iterator it = decl_list.begin();

        while(it != decl_list.end()){
        ss << (*it)->print();

                vector<Init_Declarator*>::const_iterator it2 = it;
        it2++;
        if(it2 != decl_list.end()) ss << ", ";

                it++;
    }

        ss << ";";
    return ss.str();
}

string DeclStatement::getVarType() const{
    return var_type;
}

const int DeclStatement::getCount() const{
    return decl_list.size();
}

void DeclStatement::genContext(Context* _ctxt){
    vector<Init_Declarator*>::iterator it;
    for(it = decl_list.begin(); it != decl_list.end(); it++){
        (*it)->genContext("declaration", var_type,_ctxt);
    }
}

string DeclStatement::CodeGen(Context* _ctxt) const{
    stringstream ss;
    _ctxt->setResultType(var_type);
    for(int i = 0; i < decl_list.size(); i++){
        ss << decl_list[i]->CodeGen(false, var_type, _ctxt);
    }
    return ss.str();
}

Init_Declarator* DeclStatement::getInitDecl(int i) const{
    return decl_list[i];
}


GlobalDecl::GlobalDecl(DeclStatement* _decl){
    var_type = _decl->getVarType();
    for(int i = 0; i < _decl->getCount(); i++){
        decl_list.push_back(_decl->getInitDecl(i));
    }
    delete _decl;
}

GlobalDecl::~GlobalDecl(){
}

string GlobalDecl::print() const{
    stringstream ss;
    ss << var_type << " ";

        vector<Init_Declarator*>::const_iterator it = decl_list.begin();

        while(it != decl_list.end()){
        ss << (*it)->print();

                vector<Init_Declarator*>::const_iterator it2 = it;
        it2++;
        if(it2 != decl_list.end()) ss << ", ";

                it++;
    }

        ss << ";\n";
    return ss.str();
}

void GlobalDecl::genContext(Context* _ctxt){
    vector<Init_Declarator*>::iterator it;
    for(it = decl_list.begin(); it != decl_list.end(); it++){
        (*it)->genContext("global", var_type, _ctxt);
    }
}

string GlobalDecl::CodeGen(Context* _ctxt) const{
    stringstream ss;
    for(int i = 0; i < decl_list.size(); i++){
        ss << decl_list[i]->CodeGen(true, var_type, _ctxt);
    }
    return ss.str();
}

ExprStatement::ExprStatement(Expr* _expression){
    if (_expression->getType() == "assign"){
        if(_expression->getOp() != "="){
            assigned_var = _expression->getLeft();

            Expr* temp = new Expr(assigned_var->getType(), assigned_var->getOp(), assigned_var->getLeft(), assigned_var->getRight());
            temp->setID(assigned_var->getID());
            
            if(_expression->getOp() == "+=") expression = new Expr("op", "+", temp, _expression->getRight());
            else if(_expression->getOp() == "-=") expression = new Expr("op", "-", temp, _expression->getRight());
            else if(_expression->getOp() == "*=") expression = new Expr("op", "*", temp, _expression->getRight());
            else if(_expression->getOp() == "/=") expression = new Expr("op", "/", temp, _expression->getRight());
            else if(_expression->getOp() == "%=") expression = new Expr("op", "%", temp, _expression->getRight());
            else if(_expression->getOp() == "<<=") expression = new Expr("op", "<<", temp, _expression->getRight());
            else if(_expression->getOp() == ">>=") expression = new Expr("op", ">>", temp, _expression->getRight());
            else if(_expression->getOp() == "&=") expression = new Expr("op", "&", temp, _expression->getRight());
            else if(_expression->getOp() == "|=") expression = new Expr("op", "|", temp, _expression->getRight());
            else if(_expression->getOp() == "^=") expression = new Expr("op", "^", temp, _expression->getRight());
        }
        else{
            assigned_var = _expression->getLeft();
            expression = _expression->getRight();
        }

                delete _expression;
    }
    else {
        expression = _expression;
        assigned_var = NULL;
    }
}

ExprStatement::~ExprStatement(){
    delete assigned_var;
    delete expression;
}

string ExprStatement::print(string _indent) const{
    stringstream ss;
    ss << _indent;
    if(assigned_var != NULL) ss << assigned_var->print() << " = ";
    ss << expression->print() << ";";
    return ss.str();
}

void ExprStatement::genContext(Context* _ctxt){
    int max = 0;
    expression->genContext(_ctxt);
    expression->genStackCount(max);
    _ctxt->raiseAlloc(max);
}

string ExprStatement::CodeGen(Context* _ctxt) const{
    stringstream ss;
    bool compare = (expression->getOp() == "==") || (expression->getOp() == ">") || (expression->getOp() == ">=");
    compare |= (expression->getOp() == "!=") || (expression->getOp() == "<") || (expression->getOp() == "<=");
    if(!compare){
        ss << expression->CodeGen(_ctxt);
        if(assigned_var != NULL){
            if((assigned_var->getType() == "id")&&(assigned_var->getArraySize() != 0)){
                ss << "    move\t$4,$2\n";
                ss << assigned_var->getArrayElement(0)->CodeGen(_ctxt);
                ss << "    sll\t\t$2,$2,2\n";

                stringstream name;
                name << assigned_var->getID() << "[0]";
                ss << "    addiu\t$3,$fp," << _ctxt->getOffset(name.str()) << "\n";
                ss << "    addu\t$2,$2,$3\n";
                ss << "    sw\t\t$4,0($2)\n";
            }
            else{
                string assign_offset = _ctxt->getOffset(assigned_var->getID());
                if(_ctxt->getResultType() == "double") ss << "    sdc1\t$f0," << assign_offset << "($fp)\n";
                else if(_ctxt->getResultType() == "float") ss << "    swc1\t$f0," << assign_offset << "($fp)\n";
                else ss << "    sw\t\t$2," << assign_offset << "($fp)\n";
            }
        }
        else _ctxt->setResultType("int");
    }
    else{
        ss << expression->CodeGen(_ctxt);
    }
    return ss.str();
}

string ExprStatement::getCondOp() const{
    return expression->getOp();
}

JumpStatement::JumpStatement(Expr* _expression) : expression(_expression){
}
 
JumpStatement::~JumpStatement(){
    delete expression;
}

string JumpStatement::print(string _indent) const{
    stringstream ss;
    ss << _indent << "return ";
    if(expression != NULL) ss << expression->print();
    ss << ";";
    return ss.str();
}

const Expr* JumpStatement::getExpr() const{
    return expression;
}

void JumpStatement::genContext(Context* _ctxt){
    if(expression != NULL){
        expression->genContext(_ctxt);
        int max = 0;
        expression->genStackCount(max);
        _ctxt->raiseAlloc(max);
    }
}

string JumpStatement::CodeGen(Context* _ctxt) const{
    stringstream ss;
    _ctxt->setResultType("int");

    ss << expression->CodeGen(_ctxt);
    ss << "    move\t$sp,$fp\n";
    if(_ctxt->getFuncCall()){
        ss << "    lw\t\t$31," << (_ctxt->initFrameOffset() - 4) << "($sp)\n";
        ss << "    lw\t\t$fp," << (_ctxt->initFrameOffset() - 8) << "($sp)\n";
    }
    else ss << "    lw\t\t$fp," << (_ctxt->initFrameOffset() - 4) << "($sp)\n";

    ss << "    addiu\t$sp,$sp," << _ctxt->initFrameOffset() << "\n";
    ss << "    j\t\t$31\n";
    ss << "    nop\n";
    return ss.str();
}

Decl_List::Decl_List(DeclStatement* _init_decl){
    decl_list.push_back(_init_decl);
}

Decl_List::~Decl_List(){
}

const int Decl_List::getCount() const{
    return decl_list.size();
}

DeclStatement* Decl_List::getDecl(int i) const{
    return decl_list[i];
}

void Decl_List::addtoList(DeclStatement* _decl){
    decl_list.push_back(_decl);
}

Stat_List::Stat_List(Statement* _init_stat){
    stat_list.push_back(_init_stat);
}

Stat_List::~Stat_List(){
}

const int Stat_List::getCount() const{
    return stat_list.size();
}

Statement* Stat_List::getStat(int i) const{
    return stat_list[i];
}

void Stat_List::addtoList(Statement* _stat){
    stat_list.push_back(_stat);
}

SelStatement::SelStatement(Expr* _cond, Statement* _if_stat, Statement* _else_stat) : 
    cond(_cond), if_stat(_if_stat), else_stat(_else_stat){        
}

SelStatement::~SelStatement(){
}

string SelStatement::print(string _indent) const{
    stringstream ss;
    string temp_indent = _indent;
    ss << _indent << "if" << cond->print() << endl;
    ss << if_stat->print(_indent);
    if(else_stat != NULL){
        ss << _indent << "else" << endl;
        ss << else_stat->print(_indent);
    }

    return ss.str();
}

void SelStatement::genContext(Context* _ctxt){
    int max = 0;
    cond->genContext(_ctxt);
    cond->genStackCount(max);
    _ctxt->raiseAlloc(max);
    if_stat->genContext(_ctxt);
    if(else_stat != NULL) else_stat->genContext(_ctxt);
}

string SelStatement::CodeGen(Context* _ctxt) const{
    stringstream ss;
    string else_label, end_label;
    _ctxt->setResultType("int");
    if (else_stat != NULL) else_label = _ctxt->genLabel();
    end_label = _ctxt->genLabel();

    //test for else condition
    ss << cond->CodeGen(_ctxt);
    string op = cond->getOp();
    if(op == "==") ss << "    bne\t\t$2,$3,$";
    else if(op == "!=") ss << "    beq\t\t$2,$3,$";
    else if(op == ">") ss << "    beq\t\t$2,$0,$";
    else if(op == "<") ss << "    beq\t\t$2,$0,$";
    else if(op == "<=") ss << "    bne\t\t$2,$0,$";  
    else if(op == ">=") ss << "    bne\t\t$2,$0,$";

    if(else_stat != NULL) ss << else_label << endl;
    else ss << end_label << endl;

    //print if code
    ss << endl;
    ss << if_stat->CodeGen(_ctxt);
    ss << "    b\t\t$" << end_label << endl;
    ss << "    nop\n";

    //print else code
    if(else_stat != NULL){
        ss << "$" << else_label << ":\n";
        ss << else_stat->CodeGen(_ctxt);
    }
    //label to escape if block
    ss << "$" << end_label << ":\n";

    return ss.str();
}

WhileStatement::WhileStatement(Expr* _cond, Statement* _stat, bool _iswhile) : 
    cond(_cond), stat(_stat), iswhile(_iswhile){
}

WhileStatement::~WhileStatement(){
}

string WhileStatement::print(string _indent) const{
    stringstream ss;
    ss << _indent << "while" << cond->print() << endl;
    ss << stat->print(_indent);
    return ss.str();
}

void WhileStatement::genContext(Context* _ctxt){
    int max = 0;
    cond->genContext(_ctxt);
    cond->genStackCount(max);
    _ctxt->raiseAlloc(max);
    stat->genContext(_ctxt);
}

string WhileStatement::CodeGen(Context* _ctxt) const{
    stringstream ss;
    _ctxt->setResultType("int");

    string cond_label = _ctxt->genLabel();
    string loop_label;
    if(iswhile){
        loop_label = _ctxt->genLabel();
        ss << "    b\t\t$" << cond_label << endl;
        ss << "    nop\n" << endl;
        ss << "$" << loop_label << ":\n";
        ss << stat->CodeGen(_ctxt);
        ss << "$" << cond_label << ":\n";
        ss << cond->CodeGen(_ctxt);
        string op = cond->getOp();
    }
    else{
        ss << "$" << cond_label << ":\n";
        ss << stat->CodeGen(_ctxt);
        ss << cond->CodeGen(_ctxt);
        }

    string op = cond->getOp();
    if(op == "==") ss << "    beq\t\t$2,$3,$";
    else if(op == "!=") ss << "    bne\t\t$2,$3,$";
    else if(op == ">") ss << "    bne\t\t$2,$0,$";
    else if(op == "<") ss << "    bne\t\t$2,$0,$";
    else if(op == "<=") ss << "    beq\t\t$2,$0,$";  
    else if(op == ">=") ss << "    beq\t\t$2,$0,$";
    
    if(iswhile) ss << loop_label << endl;
    else ss << cond_label << endl;
    
    ss << "    nop\n" << endl;
    return ss.str();
}

ForStatement::ForStatement(ExprStatement* _init, ExprStatement* _end, Expr* _step, Statement* _stat) :
    init(_init), end(_end), step(_step), stat(_stat){       
}

ForStatement::~ForStatement(){
}

string ForStatement::print(string _indent) const{
    stringstream ss;
    ss << _indent << "for(" << init->print("") << end->print(" ");
    if(step != NULL) ss << step->print();
    ss << ")\n";
    ss << stat->print(_indent);
    return ss.str();
}

void ForStatement::genContext(Context* _ctxt){
    init->genContext(_ctxt);
    end->genContext(_ctxt);
    step->genContext(_ctxt);

    int max = 0;
    step->genStackCount(max);
    _ctxt->raiseAlloc(max);

    stat->genContext(_ctxt);
}

string ForStatement::CodeGen(Context* _ctxt) const{
    stringstream ss;
    _ctxt->setResultType("int");
    
    string cond_label = _ctxt->genLabel();
    string loop_label = _ctxt->genLabel();
    //init loop var
    ss << init->CodeGen(_ctxt);
    ss << "    b\t\t$" << cond_label << "\n";
    ss << "    nop\n\n";
    //perform statements
    ss << "$" << loop_label << ":\n";
    ss << stat->CodeGen(_ctxt);

    //step loop var
    if(step->getType() == "unary"){
        ss << step->CodeGen(_ctxt);
    } 
    else{
        ExprStatement* temp = new ExprStatement(step);
        ss << temp->CodeGen(_ctxt);
        delete temp;
    }
    //check end condition
    ss << "$" << cond_label << ":\n";
    ss << end->CodeGen(_ctxt);
    string op = end->getCondOp();
    if(op == "==") ss << "    beq\t\t$2,$3,$";
    else if(op == "!=") ss << "    bne\t\t$2,$3,$";
    else if(op == ">") ss << "    bne\t\t$2,$0,$";
    else if(op == "<") ss << "    bne\t\t$2,$0,$";
    else if(op == "<=") ss << "    beq\t\t$2,$0,$";  
    else if(op == ">=") ss << "    beq\t\t$2,$0,$";
    ss << loop_label << endl;
    ss << "    nop\n\n";
    return ss.str();
}

CompoundStatement::CompoundStatement(Decl_List* _decl_list, Stat_List* _stat_list){
    if(_decl_list != NULL){
        int declsize = _decl_list->getCount();
        for (int i = 0; i < declsize; i++){
            decl_list.push_back(_decl_list->getDecl(i));
        }
        delete _decl_list;
    }   
    if(_stat_list != NULL){
        int statsize = _stat_list->getCount();
        for(int i = 0; i < statsize; i++){
            stat_list.push_back(_stat_list->getStat(i));
        }
        delete _stat_list;
    }
    //ctxt = new Context();
}   

CompoundStatement::~CompoundStatement(){
    for (int i = 0; i < decl_list.size(); i++){
        delete decl_list[i];
    }
    for (int i = 0; i < stat_list.size(); i++){
        delete stat_list[i];
    }
}

string CompoundStatement::print(string _indent) const{
    stringstream ss;
    string temp_indent = _indent;
    temp_indent.append("    ");

        ss << _indent << "{\n";
    for(int i = 0; i < decl_list.size(); i++){
        ss << decl_list[i]->print(temp_indent) << endl;
    }
    for(int i = 0; i < stat_list.size(); i++){
        ss << stat_list[i]->print(temp_indent) << endl;
    }
    ss << _indent << "}\n";

    return ss.str();
}

const int CompoundStatement::getDeclCount() const{
    return decl_list.size();
}

const int CompoundStatement::getStatCount() const{
    return stat_list.size();
}

const DeclStatement* CompoundStatement::getDecl(int i) const{
    return decl_list[i];
}

const Statement* CompoundStatement::getStat(int i) const{
    return stat_list[i];
}

void CompoundStatement::genContext(Context* _ctxt){
    vector<DeclStatement*>::iterator it;
    for(it = decl_list.begin(); it != decl_list.end(); it++){
        (*it)->genContext(_ctxt);
    }
    vector<Statement*>::iterator it2;
    for(it2 = stat_list.begin(); it2 != stat_list.end(); it2++){
        (*it2)->genContext(_ctxt);
    }
}

string CompoundStatement::CodeGen(Context* _ctxt) const{
    stringstream ss;
    for(int i = 0; i < decl_list.size(); i++){
        ss << decl_list[i]->CodeGen(_ctxt);
    }
    for(int i = 0; i < stat_list.size(); i++){
        ss << stat_list[i]->CodeGen(_ctxt);
    }

        return ss.str();
}

Function::Function(string _return_type, Direct_Declarator* _declarator, CompoundStatement* _comp_stat) :
    return_type(_return_type), declarator(_declarator), comp_stat(_comp_stat) {
    ctxt = new Context();        
}

Function::~Function(){
    delete declarator;
    delete comp_stat;
    delete ctxt;
}

string Function::print() const{
    stringstream ss;
    string temp_indent = "";
    ss << return_type << " " << declarator->print() << "\n" << comp_stat->print(temp_indent);
    ss << ctxt->print();
    return ss.str();
}

void Function::genContext(Context* _ctxt){
    ctxt->inheritContext(_ctxt);
    declarator->genContext(false, ctxt);
    comp_stat->genContext(ctxt);
}

string Function::CodeGen(Context* _ctxt) const{
    stringstream ss;
    string name = declarator->getID();
    int temp = ctxt->initFrameOffset();
    ctxt->mapVars();

    ss << "    .text\n";
    ss << "    .align\t2\n";
    ss << "    .globl\t" << name << "\n";
    ss << "    .set\tnomips16\n";
    ss << "    .set\tnomicromips\n";
    ss << "    .ent\t" << name << "\n";
    ss << "    .type\t" << name << ", @function\n";
    ss << name << ":\n";
    ss << "    .frame\t$fp," << temp << ",$31\n"; 
    ss << "    .mask\t0x40000000,-4\n";
    ss << "    .fmask\t0x00000000,0\n";
    ss << "    .set\tnoreorder\n";
    ss << "    .set\tnomacro\n";
    ss << "    addiu\t$sp,$sp,-" << temp << "\n";
    
    if(ctxt->getFuncCall()){
        ss << "    sw\t\t$31," << (temp - 4) << "($sp)\n";
        ss << "    sw\t\t$fp," << (temp - 8) << "($sp)\n";
    }
    else ss << "    sw\t\t$fp," << (temp - 4) << "($sp)\n";
    ss << "    move\t$fp,$sp\n";

    int max_para;
    if(ctxt->ParaCount() <= 4) max_para = ctxt->ParaCount();
    else max_para = 4;
    for(int i = 0; i < max_para; i++){
        ss << "    sw\t\t$" << i+4 << "," << temp + 4*i << "($fp)\n";
    }

    ss << comp_stat->CodeGen(ctxt);
    ss << endl;
    ss << "    .set\tmacro\n";
    ss << "    .set\treorder\n";
    ss << "    .end\t" << name << endl;
    ss << "    .size\t" << name << ", .-" << name << "\n";

        return ss.str();
}

Programme::Programme(Node* _init_node){
    prgm_list.push_back(_init_node);
    ctxt = new Context();
}

Programme::~Programme(){
    for(int i = 0; i < prgm_list.size(); i++){
        delete prgm_list[i];
    } 
}

string Programme::print() const{
    stringstream ss;
    vector<Node*>::const_iterator it;
    for(it = prgm_list.begin(); it != prgm_list.end(); it++){
        ss << (*it)->print() << "\n";
    }

        ss << ctxt->print();
    return ss.str();
}

void Programme::addtoList(Node* _node){
    prgm_list.push_back(_node);
}

void Programme::genContext(){
    vector<Node*>::iterator it;
    for(it = prgm_list.begin(); it != prgm_list.end(); it++){
        (*it)->genContext(ctxt);
    }
}

string Programme::CodeGen(){
    stringstream ss;
    for (vector<Node*>::iterator it = prgm_list.begin(); it != prgm_list.end(); it++){
       ss << (*it)->CodeGen(ctxt);
    }
    return ss.str();
}