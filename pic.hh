#ifndef PIC_HH
#define PIC_HH
#include <unordered_map>
#include <cstdio>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <stack>
using namespace std;

class express;
class TreeNode;

extern vector<unordered_map<string, express*>> table;
extern stack<express*> returns;

class block;      
class expr_coll; 

union value {
	double a;
	char c;
	bool boolean;
};

struct Expression {
	int etype=0;
	value v;
	Expression();
	~Expression(){};
	Expression(double x = 0) {
		etype = 0;
		v.a = x;
	}
	Expression(int x = 0) {
		etype = 0;
		v.a = x;
	}
	Expression(value val) {  // used only for bool
		etype = 3;
		v = val;
	}
	Expression(char x) {
		etype = 2;
		v.c = x;
	}

	Expression(bool x) {
		etype = 3;
		v.boolean = x;
	}

	void get_value() {
		switch (etype) {
			case 0:
				cout<<v.a;
				break;
			case 2:
				cout<<v.c;
				break;
			case 3:
				cout<<v.boolean;
				break;
			default:
            cout << "Unknown type: " << etype << endl;
			cout<<"this etype is the problem "<<etype<<endl;
			printf("Error: get_value\n");
			exit(1);
		}
	}
};

union object {
    Expression exp;
    string *b;
	TreeNode* node;
	vector<TreeNode*>* vec;
	const char *ch; 
    object() {} 
    ~object() {} 
    object(Expression& e) : exp(e) {}
    object(string *s) : b(s) {}
	object(const char *s) : ch(s) {}
	object(vector<TreeNode*>* v) : vec(v) {}
	object(TreeNode* n) : node(n) {}
	friend object operator/ (const object& lhs, const object& rhs);
	friend object operator- (const object& lhs, const object& rhs);
	friend object operator* (const object& lhs, const object& rhs);
	friend object operator+ (const object& lhs, const object& rhs);
};

enum StatementType {
	START,   // start of program... instantiated only in the beginning and takes vector of
	STMT_BLOCK, // collection of statement/asts, takes vector of statements as input and just holds them
    FUNC_CALL,  // takes vector of arguments and execute as input
	FUNC_DEC,   // func declaration
	STMT_SCOPE, // scope to define stuff inside {}
    STMT_FOR,   //	takes 2 nodes again...condition and execute
    STMT_WHILE,  //  takes 2 nodes again...condition and execute
    STMT_IF_ELSE,  //  takes condition and execute as input (2 tree nodes)..note that presently else has to be included always
	STMT_ASSIGN,	// let statmement
	STMT_PRINT,     // print
	RETURN_STMT,   //used to return in functions
	STMT_PRINT_ARRELEMENT, // to print an indexeed arr element
	EXPR_COLL,		// collection of expression separated by comma
    STMT_EXPRESSION,  // simple expression including integers, stings, sum , ...basically string of operations
};

class TreeNode {
public:
	int type=99;
    object obj; 
	TreeNode* left = NULL;
    TreeNode* right = NULL;
	StatementType stmtType; 
	TreeNode() : left(nullptr), right(nullptr) {}
    ~TreeNode() {}
	virtual void print(){}
	virtual express* code(){return NULL;}
	virtual express* code(vector<express*>* nodes){return NULL;}
};


class assign : public TreeNode{
public:
	assign(){stmtType = STMT_ASSIGN;}
	~assign(){stmtType = STMT_ASSIGN;}
	assign(string* name, TreeNode* expression, int i){
		obj.b = name;
		obj.exp.v.a =i;
		stmtType = STMT_ASSIGN;
		right=expression;
	}
};

class express : public TreeNode{
public:
    express() {type = 0; stmtType = STMT_EXPRESSION;} 
    virtual ~express() { delete obj.b; stmtType = STMT_EXPRESSION;}
    express(Expression e){
		type = 0;
		obj.exp = e;
		stmtType = STMT_EXPRESSION;
	} 
    express(string *s)
	{
		type=1;
		obj.b=s;
		stmtType = STMT_EXPRESSION;
	}  
	express(const char *s)
	{
		type=8;
		obj.ch=s;
		stmtType = STMT_EXPRESSION;
	} 
	express(vector<TreeNode*>* v){
		type = 2;
		obj.vec=v;
		stmtType = STMT_EXPRESSION;
	} 
	express(TreeNode* node){
		type = node->type;
		obj = node->obj;
		left = node->left;
		right = node->right;
		stmtType = STMT_EXPRESSION;
	}
	express(block* blk);
	express(expr_coll* expr);

	express(express* ex){
		type = 3;
		right=ex;
		stmtType = STMT_EXPRESSION;
	}
    void print() override {
        if (type == 0) {
            obj.exp.get_value();
        } else if (type == 1) {
            cout<< *(obj.b);
        } else if(type==2){
			cout << "[";
			for (int i=0; i < obj.vec->size(); i++)
				{(*(obj.vec))[i]->obj.exp.get_value(); if(i<obj.vec->size()-1)cout<<",";}
			cout <<"]";
		}
		else if(type == 8)
		{
			cout<<*obj.ch;
		}
		else 
		{
			obj.node->print();
		}
		cout<<endl;
    }
};

class For : public TreeNode{
public:
	For(){stmtType = STMT_FOR;}
	~For(){stmtType = STMT_FOR;}
	For(TreeNode* initialise,TreeNode* bool_cond,TreeNode* terminate, TreeNode* execute){
	vector<TreeNode*>* temp = new vector<TreeNode*>;
	temp->push_back(initialise); temp->push_back(bool_cond); temp->push_back(terminate); temp->push_back(execute); 
	obj.vec = temp;
	stmtType = STMT_FOR;
	}
};

class return_func : public TreeNode{
public:
	return_func(){stmtType = RETURN_STMT;}
	~return_func(){stmtType = RETURN_STMT;} 
	return_func(express* expression){
	obj.node = expression;
	stmtType = RETURN_STMT;
	}
};

class While : public TreeNode{
public:
	While(){stmtType = STMT_WHILE;}
	~While(){stmtType = STMT_WHILE;}
	While(TreeNode* condition, TreeNode* execute){left=condition; right=execute;stmtType = STMT_WHILE; }
};

class if_else : public TreeNode{
public:
	
	if_else(){stmtType = STMT_IF_ELSE;}
	~if_else(){stmtType = STMT_IF_ELSE;}
	if_else(express* condition, TreeNode* this_exp, TreeNode* that_exp){obj.node=condition; left=this_exp; right=that_exp; stmtType = STMT_IF_ELSE;}
};


class expr_coll : public TreeNode{
public:
	expr_coll(){stmtType = EXPR_COLL;}
	~expr_coll(){stmtType = EXPR_COLL;}
	expr_coll(vector<TreeNode*>* expr){obj.vec=expr; stmtType = EXPR_COLL;}
	void push (TreeNode* expr)
		{obj.vec->push_back(expr);
		stmtType = EXPR_COLL;}
};

class block : public TreeNode{
public:
	block(){stmtType = STMT_BLOCK;}
	~block(){stmtType = STMT_BLOCK;}
	block(vector<TreeNode*>* stmts){obj.vec=stmts; stmtType = STMT_BLOCK;}
	void push (TreeNode* stmt) {obj.vec->push_back(stmt);}
};

class Scope : public TreeNode{
public: 
	Scope(){stmtType = STMT_SCOPE;}
	~Scope(){stmtType = STMT_SCOPE;}
	Scope(block* stmts){obj.node=stmts; stmtType = STMT_SCOPE;}
};

class print_variable : public TreeNode{
public:
	print_variable(){stmtType = STMT_PRINT;}
	~print_variable(){stmtType = STMT_PRINT;}
	print_variable(string* a) {obj.b=a; type = 1; stmtType = STMT_PRINT;}
	print_variable(express* nodee) {obj.node=nodee;type = 2; stmtType = STMT_PRINT;}
};

class print_arrelement: public express{
public:
	print_arrelement(){stmtType = STMT_PRINT_ARRELEMENT;}
	~print_arrelement(){stmtType = STMT_PRINT_ARRELEMENT;}
	print_arrelement(string* name, express* nodee) {
		type = 69; left=nodee;obj.b=name; stmtType = STMT_PRINT_ARRELEMENT;
	}
};

class function: public express{
public:
	function(){stmtType=FUNC_DEC;}
	~function(){stmtType=FUNC_DEC;}
	function(express* expr){
		obj.vec=expr->obj.vec;
		left= expr->left;
		stmtType=FUNC_DEC;
	}
	express* code(vector<express*>* nodes) override;
	function(vector<TreeNode*>* nodee,Scope* scop) {
		obj.vec=nodee;left=scop;stmtType=FUNC_DEC;
	}
};

class func_call : public express{
public:
	string* temporary ;
	func_call(){stmtType=FUNC_CALL;}
	~func_call(){stmtType=FUNC_CALL;}
	func_call(string* name, vector<TreeNode*>* vect){
		temporary=name;obj.vec=vect;
		stmtType=FUNC_CALL;
	}
	express* code() override;
};

class programm : public TreeNode{
public:
	programm(){stmtType = START;}
	~programm(){stmtType = START;}
	programm(block* stmts) { obj.node = stmts; stmtType = START; }
};

// OTHER DECLARATIONS

void bfs(TreeNode* root);

#endif  //PIC_HH