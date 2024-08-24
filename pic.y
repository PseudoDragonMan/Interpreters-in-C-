%{
    #include "pic.cc"
    extern "C" void yyerror(const char *s);
    extern int yylex(void);
	extern vector<unordered_map<string, express*>> table;
	extern stack<express*> returns;
%}

%union{
	string *name;	
	express* exp;
	TreeNode* tree;
	expr_coll* wrap_exp;
	Scope* scop;
	block* stmt_wrap;
	vector<TreeNode*>* vector_TreeNodes;
}

%token INT_CONST FLT_CONST NAME LET FOR WHILE IF ELSE FUNC NOT EQUAL RETURN	FN	// Token declarations (lexer uses these declarations to return tokens)

%left '+' '-' 											// Left associative operators '+' and '-'
%left '*' '/' 											// Left associative operators '*' and '/'
%right Uminus NOT										// Right associative unary minus operator
%nonassoc '<' '>' EQUAL NOT_EQUAL

%type <name> INT_CONST FLT_CONST NAME LET '(' ')' '{' '}' '[' ']' 		// Declare token types to be of type *string
%type <exp> expression 								// Declare the expression non-terminal to be of type *TreeNode
%type <tree> stmt 
%type <scop> block
%type <stmt_wrap> stmts
%type <vector_TreeNodes> expressions

%start program 											// Starting rule for the grammar
%%

/* GRAMMAR */

program
	: stmts	{ 
			evaluate($1);
			programm *top = new programm($1); 
			// line commented for(int i=0;i<(*top->obj.node->obj.vec).size();i++){bfs((*top->obj.node->obj.vec)[i]);cout<<endl;}
			}
;

stmts
	: stmts stmt { 
			$1->push($2); 
			$$ = $1;
			} 
	| stmt	{ 
			vector<TreeNode*>* vec = new vector<TreeNode*>();
			vec->push_back($1);
			$$ = new block(vec); 	
			}
 
stmt
	: block												{ $$ = $1; }
	| RETURN expression ';'								{ $$ = new return_func($2);}
	| FOR '(' stmt  expression ';' stmt ')' block		{ $$ = new For($3,$4,$6,$8); }
	| WHILE '(' expression ')' block					{ $$ = new While($3,$5); }
	| IF '(' expression ')' block ELSE block	   		{ $$ = new if_else($3,$5,$7); }
	| LET NAME '=' expression ';'						{ $$ = new assign($2,$4,0); }
	| NAME '=' expression ';'							{ $$ = new assign($1,$3,1); }
	| NAME ';'     	 									{ $$ = new print_variable($1); }
	| expression ';'									{ $$ = new print_variable($1); }
; 
	
block
	:'{' stmts '}' 										{ $$ = new Scope($2); }
;

expressions
    : expressions ',' expression                        { $1->push_back($3); $$ = $1; }
    | expression                                        { vector<TreeNode*>* vec = new vector<TreeNode*>(); vec->push_back($1); $$ = vec; }
;

expression 
	: '(' expression ')'				                { $$ = $2; }
	| NAME '(' expressions ')'							{ $$ = new func_call($1,$3);}
	| FN '(' expressions ')' block		 				{ $$ = new function($3,$5);}
	| '[' expressions ']'                               { $$ = new express($2); }
	| NAME '[' expression ']'							{ $$ = new print_arrelement($1,$3);}
	| expression '/' expression			 				{ auto ret = new express("/"); ret->left = $1; ret->right = $3; $$ = ret;  }
	| expression '*' expression							{ auto ret = new express("*"); ret->left = $1; ret->right = $3; $$ = ret;  }
	| expression '+' expression							{ auto ret = new express("+"); ret->left = $1; ret->right = $3; $$ = ret;  }
	| expression '-' expression					 		{ auto ret = new express("-"); ret->left = $1; ret->right = $3; $$ = ret;  }
	| expression '<' expression							{ auto ret = new express("<"); ret->left = $1; ret->right = $3; $$ = ret;  }
	| expression '>' expression							{ auto ret = new express(">"); ret->left = $1; ret->right = $3; $$ = ret;  }
	| expression EQUAL expression						{ auto ret = new express("=="); ret->left = $1; ret->right = $3; $$ = ret; }
	| expression NOT_EQUAL expression					{ auto ret = new express("!="); ret->left = $1; ret->right = $3; $$ = ret; }
	| NOT expression 									{ auto ret = new express("!"); ret->obj.node=$2; $$ = ret; }
	| '-' expression %prec Uminus						{ $$ = new express(*new Expression(-1 *($2->obj.exp.v.a))); }
	| INT_CONST											{ $$ = new express(*new Expression(atoi($1->c_str()))); }
	| FLT_CONST											{ $$ = new express(*new Expression(atoi($1->c_str()))); }
	| NAME												{ $$ = variable($1); }
;

%%

/* ADDITIONAL C CODE */

int main() {
	table.push_back(unordered_map<string, express*>()); 
	yyparse();
    return 0;
}