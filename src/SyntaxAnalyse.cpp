#include "SyntaxAnalyse.h"
// 错误标记，若errors>0返回报错 
int errors = 0;
// 用于保存关键字kind
int keyidx;
// 用于保存文本值 
string text;
Word t;
int formflag = 0;
// 记录已定义的函数用于后期调用 
vector<string> function;
int constflag = 0;
extern int count;
extern int num_lbp, num_rbp;
extern int num_lba, num_rba;
extern int num_lp, num_rp;

// 源程序分析入口 
TreeNode* Program(TreeNode*& root)
{
	cout << endl << "程序:" << endl;
	t = GetToken();
	// 调用外部定义序列 
	if (Extdeflist(root)||t.kind==eof)
		return root;
	else{
		cout << "Wrong Syntax." << endl;
		DestroyTree(root);
		return NULL;
	}
}

// 外部定义序列
TreeNode* Extdeflist(TreeNode*& root)
{
	// 预判断括号是否对齐 
	if (num_lbp!=num_rbp){
		cout << "Missing '{' or '}'" << endl;
		return NULL;
	}
	if (num_lba!=num_rba){
		cout << "Missing '/*' or '*/'" << endl;
		return NULL;
	}
	if (num_lp!=num_rp){
		cout << "Missing '(' or ')'" << endl;
		return NULL;
	}
	// 判断到达文件尾 
	if (t.kind == eof){
		cout << endl << "End of File" << endl;
		return NULL;
	}
	root = new TreeNode;
	// 调用外部定义 
	TreeNode *p = Extdef(root->child);
	if (!p){
		DestroyTree(root);
		return NULL;
	}
	// 递归调用外部定义序列 
	TreeNode *q = Extdeflist(root->next);
	if (!q){
		DestroyTree(root);
		return NULL;
	}
	return root;
}

// 外部定义分析
TreeNode* Extdef(TreeNode*& root)
{
	TreeNode* p = NULL;
	while (t.kind == PRE){				// # 
		t = GetToken();
		cout << "  预编译: " << endl;
		if (t.text != "define" && t.text != "include"){
			cout << "Precompiled Error, line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		if (t.text == "define"){		// 宏定义 
			cout << "    宏定义: " << endl;
			t = GetToken();
			if (t.kind != IDENT){
				cout << "Not Identifier, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "      变量名: " << t.text;
			t = GetToken();
			if (t.kind!=INT_CONST && t.kind!=DOUBLE_CONST){
				cout << endl << "Precomplied Error, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << ", 参数值: " << t.text << endl;
			p = new TreeNode;
			t = GetToken();
			return p;
		}
		else{							// include, 文件包含 
			cout << "    文件包含: " << endl;
			t = GetToken();
			if (t.kind != LESS){
				cout << endl << "Precomplied Error, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();				// 文件名 
			string s = t.text;
			t = GetToken();				// .
			s += t.text;
			t = GetToken();				// 拓展名 
			s += t.text;
			t = GetToken();
			if (t.kind != MORE){
				cout << endl << "Precomplied Error, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "      文件名: " << s << endl;
			t = GetToken();
			p = new TreeNode;
			return p;
		}
	}
	// 函数调用或变量初始化
	while (t.kind == IDENT){ 
		// 函数调用，从已定义函数中查找调用是否存在 
		for (vector<string>::iterator i=function.begin(); i<function.end(); i++){ 
			if (*i == t.text){
				cout << endl << "    函数调用:" << endl;
				cout << "      函数名: " << *i << endl;
				t = GetToken();
				if (t.kind != LP){
					cout << "Expected '(', line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				t = GetToken();
				if (t.kind != IDENT){
					cout << "Not Identifier, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				cout << "      参数: ";
				while(1){
					cout << t.text << " ";
					t = GetToken();
					if (t.kind == RP)
						break;
					else if (t.kind == COMMA){
						t = GetToken();
						continue;
					}
					else{
						cout << "Wrong Syntax, line:";
						if (t.line > t.preln)
							cout << t.preln << endl;
						else
							cout << t.line << endl;
						return NULL;
					}
				}
				cout << endl;
				t = GetToken();
				if (t.kind != SEMI){
					cout << "Expected ';', line:";
					//判断是否换行
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				p = new TreeNode;
				t = GetToken();
				return p;
			}
		}
		// 调用表达式处理初始化表达式 
		p = Expression(SEMI);
		if (!p){
			cout << "Wrong Expression, line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		t = GetToken();
	}
	// 处理注释(包括行注释和块注释) 
	while (t.kind == LANNO){		// 行注释 
		cout << "------------------------------" << endl << "行注释: ";
		string s = "";
		int line = t.line;
		t = GetToken();
		while (t.line <= line){		// 未换行 
			s += t.text;
			s += " ";
			t = GetToken();
		}
		cout << s << endl << "------------------------------" << endl;
		p = new TreeNode;
		return p;
	}
	while (t.kind == BANNO){
		cout << "------------------------------" << endl << "块注释: " << endl;
		string s = "";
		t = GetToken();
		int line = t.line;
		while (t.text != "*/"){
			if (t.line>line){
				s += "\n";
				s += t.text;
				line = t.line;
				s += " ";
			}
			else{
				s += t.text;
				s += " ";
			}
			t = GetToken();
		}
		cout << s << endl << "------------------------------" << endl;
		p = new TreeNode;
		t = GetToken();
		return p;
	}
	if (t.kind >= CHAR_CONST ){
		cout << "Not Key Words, line: ";
		if (t.line > t.preln)
			cout << t.preln << endl;
		else
			cout << t.line << endl;
		return NULL;
	}
	// 保存关键字kind 
	keyidx = t.kind;
	// const常量 
	if (t.kind == 6){
		t = GetToken();
		keyidx = t.kind;
		constflag = 1;
	}
	t = GetToken();
	if (t.kind != IDENT){
		cout << "Not Identifier, line: ";
		if (t.line > t.preln)
			cout << t.preln << endl;
		else
			cout << t.line << endl;
		return NULL;
	}
	// 保存文本值text 
	text = t.text;
	t = GetToken();
	root = new TreeNode;
	// 调用外部变量序列 
	if (t.kind != LP)
		p = Extvardef(root->child);
	// 调用函数定义 
	else
		p = Funcdef(root->child);
	if (p){
		return root;
	}
	else{
		DestroyTree(root);
		return NULL;
	}
}

// 外部变量序列
TreeNode* Extvardef(TreeNode*& root)
{
	cout << endl << "  外部变量定义:" << endl;
	root = new TreeNode;
	root->child = new TreeNode;
	root->child->token = keywords[keyidx];
	if (constflag){
		cout << "    CONST常量" << endl;
		constflag = 0;
	}
	cout << "    类型：" << keywords[keyidx] << endl;
	TreeNode* p = root->next;
	p = new TreeNode;
	p->child = new TreeNode;
	p->child->token = text;
	cout << "    变量名: " << text << endl;
	int skip = 0;
	while(1){
		if (t.kind == ASSIGN){			// e.g. int i=3;
			cout << "    初始化语句ASSIGN: " << endl;
			t = GetToken();
			TreeNode *o = Sentence();
			if (!o){
				return NULL;
			}
			skip = 1;
		}
		if (t.kind == LK){				// 数组 int a[10];
			t = GetToken();
			if (t.kind != INT_CONST){
				cout << "Expected Integer in [], line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			string size = t.text;
			t = GetToken();
			if (t.kind != RK){
				cout << "Expected ']' after '[', line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "    附加类型: 数组, size: " << size << endl;
			t = GetToken();
		}
		if (!skip && t.kind != SEMI && t.kind != COMMA){
			cout << "Expected ',' or ';' line: ";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		if (skip || t.kind == SEMI){
			if (!skip)
				t = GetToken();
			return root;
		}
		t = GetToken();
		if (t.kind != IDENT){
			cout << "Not Identifier, line: ";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		TreeNode *q = p->next;
		q = new TreeNode;
		p = q;
		p->child = new TreeNode;
		p->child->token = t.text;
		cout << "    变量名: " << t.text << endl;
		t = GetToken();
	}
}

// 函数定义 
TreeNode* Funcdef(TreeNode*& root)
{
	cout << endl << "  函数:" << endl;
	root = new TreeNode;
	root->child = new TreeNode;
	root->child->token = keywords[keyidx];
	cout << "    类型: " << keywords[keyidx] << endl;
	root->next = new TreeNode;
	root->next->token = text;
	function.push_back(text);
	cout << "    函数名: " << text << endl;
	cout << "    函数形参: " << endl;
	// 调用形参 
	TreeNode *r = FormPara(root->next->child);
	if (!r){
		return NULL;
	}
	t = GetToken();
	TreeNode *p = NULL;
	// 之后出现注释 
	if (t.kind == LANNO){
		cout << "------------------------------" << endl << "行注释: ";
		string s = "";
		int line = t.line;
		t = GetToken();
		while (t.line <= line){			// 未换行,以换行符为注释结束标志 
			s += t.text;
			s += " ";
			t = GetToken();
		}
		cout << s << endl << "------------------------------" << endl;
	}
	else if (t.kind == BANNO){
		cout << "------------------------------" << endl << "块注释: " << endl;
		string s = "";
		t = GetToken();
		int line = t.line;
		while (t.text != "*/"){			// 以*/为块注释结束标志 
			if (t.line>line){
				s += "\n";
				s += t.text;
				line = t.line;
				s += " ";
			}
			else{
				s += t.text;
				s += " ";
			}
			t = GetToken();
		}
		cout << s << endl << "------------------------------" << endl;
		t = GetToken();
	}
	// 函数声明 
	if (t.kind == SEMI){
		t = GetToken();
		root->final = NULL;
		cout << "      附加: 函数声明" << endl;
		root = new TreeNode;
		return root;
	}
	// 函数定义 
	else if (t.kind == LBP){
		cout << "      附加: 函数定义" << endl;
		p = Compound(root->final);
		if (!p){
			return NULL;
		}
	}
	else{
		cout << "Invalid Syntax, line:";
		if (t.line > t.preln)
			cout << t.preln << endl;
		else
			cout << t.line << endl;
		DestroyTree(root);
		return NULL;
	}
	if (!p){
		DestroyTree(root);
		return NULL;
	}
	return root;
}

// 形式参数定义
TreeNode* FormPara(TreeNode*& root)
{
	t = GetToken();
	root = new TreeNode;
	TreeNode* p = root;
	// 无形参 
	if (t.kind == RP){
		if (!formflag){
			root->child = NULL;
			cout << "      void" << endl;
		}
		else{
			formflag = 0;
			cout << "Expected Parameter, line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		return root;
	}
	// 处理形参 
	else if (t.kind < ERROR_TOKEN){		// keywords
		formflag = 0;
		if (t.kind == 6){
			cout << "      CONST常量" << endl;
			t = GetToken();
			if (t.kind > ERROR_TOKEN){
				cout << "Not Key Words, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
		}
		p->child = new TreeNode;
		p->child->child = new TreeNode;
		p->child->child->token = t.text;
		cout << "      类型: " << t.text << ", ";
		t = GetToken();
		if (t.kind != IDENT){
			cout << "Not Identifier, line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		p->child->next = new TreeNode;
		p->child->next->token = t.text;
		cout << "参数名: " << t.text << endl;
		t = GetToken();
		if (t.kind == RP){
			p->next = NULL;
			return root;
		}
		if (t.kind != COMMA){
			cout << "Expected ',', line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		formflag = 1;
		TreeNode *r = FormPara(p->next);
		if (!r){
			return NULL;
		}
	}
	else{
		formflag = 0;
		cout << "Not Key Words, line:";
		if (t.line > t.preln)
			cout << t.preln << endl;
		else
			cout << t.line << endl;
		return NULL;
	}
	return root;
}

// 复合语句 
TreeNode* Compound(TreeNode*& root)
{
	root = new TreeNode;
	// 复合语句块开始 
	cout << "  Start of Compound Sentence {" << endl;
	t = GetToken();
	TreeNode* p = NULL;
	TreeNode *q = NULL;
	if (t.kind == 6){			//const常量
		constflag = 1;
		t = GetToken();
	}
	if (t.kind <= 5){
		keyidx = t.kind;
		cout << "    局部变量:" << endl;
		// 调用局部变量函数
		p = Localvar(root->child);
	}
	else{
		root->child = NULL;
	}
	// 调用语句序列 
	q = Sentenlist(root->next);
	if (!q){
		DestroyTree(root);
		return NULL;
	}
	if (t.kind != RBP){
		cout << "Invalid Syntax, line:";
		if (t.line > t.preln)
			cout << t.preln << endl;
		else
			cout << t.line << endl;
		DestroyTree(root);
		return NULL;
	}
	t = GetToken();
	return root;
}

// 局部变量定义 
TreeNode* Localvar(TreeNode*& root)
{
	root = new TreeNode;
	root->child = new TreeNode;
	root->child->child = new TreeNode;
	root->child->child->token = keywords[keyidx];
	if (constflag){
		cout << "      CONST常量" << endl;
		constflag = 0;
	}
	cout << "      类型: " << keywords[keyidx] << ", ";
	t = GetToken();
	if (t.kind != IDENT){
		cout << "Not Identifier, line:";
		int line = t.line;
		t = GetToken();
		if (t.line > line)
			cout << t.line-1 << endl;
		else
			cout << t.line << endl;
		DestroyTree(root);
		return NULL;
	}
	root->child->next = new TreeNode;
	root->child->next->token = t.text;
	cout << "变量名: " << t.text << endl;
	t = GetToken();
	int skip = 0;
	if (t.kind == ASSIGN){			// e.g. int i=3;
		cout << "      初始化语句ASSIGN: " << endl;
		t = GetToken();
		TreeNode *o = Sentence();
		if (!o){
			return NULL;
		}
		skip = 1;
	}
	if (t.kind == LK){				// 数组 int a[10];
		t = GetToken();
		if (t.kind != INT_CONST){
			cout << "Expected Integer in [], line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		string size = t.text;
		t = GetToken();
		if (t.kind != RK){
			cout << "Expected ']' after '[', line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		cout << "      附加类型: 数组, size: " << size << endl;
		t = GetToken();
	}
	if (!skip && t.kind != SEMI && t.kind != COMMA){
		cout << "Expected ',' or ';', line:";
		if (t.line > t.preln)
			cout << t.preln << endl;
		else
			cout << t.line << endl;
		return NULL;
	}
	if (skip || t.kind == SEMI){
		if (!skip)
			t = GetToken();
		return root;
	}
	// 递归调用局部变量函数 
	TreeNode* p = Localvar(root->next);
	if (!p){
		DestroyTree(root);
		return NULL;
	}
	return root;
}

// 语句序列 
TreeNode* Sentenlist(TreeNode*& root)
{
	TreeNode* p = Sentence();
	if (p == NULL){
		if (errors > 0){
			cout << "Invalid Syntax, line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return p;
		}
		// 复合语句块结束 
		else{
			cout << "  End of Compound Sentence }" << endl;
			return root;
		}
	}
	else{
		root = new TreeNode;
		root->child = p;
		// 递归调用语句序列 
		TreeNode *q = Sentenlist(root->next);
		if (!q && errors>0){
			return NULL;
		}
		return root;
	}
}

// 语句分析
TreeNode* Sentence()
{
	TreeNode* p = NULL;
	TreeNode* q = NULL;
	TreeNode* r = NULL;
	TreeNode* s = NULL;
	TreeNode* u = NULL;
	string str;
	int line;
	switch (t.kind){
		case 7:							// if
			t = GetToken();
			cout << endl << "  IF语句:" << endl; 
			if (t.kind != LP){
				cout << "Invalid Syntax, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "    条件:" << endl;
			t = GetToken();
			q = Expression(RP);			// 条件表达式
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    IF子句:" << endl;
			r = Sentence();				// if子句
			if (r == NULL){
				return NULL;
			}
			if (t.kind == 8){			// else
				t = GetToken();
				cout << "    ELSE子句:" << endl;
				s = Sentence();
				p = new TreeNode;		// if-else
				p->child = q;
				p->next = r;
				p->final = s;
				return p;
			}
			else{
				p = new TreeNode;		// if-then
				p->child = q;
				p->next = r;
				return p;
			}
		case LBP:						// {
			p = Compound(q);
			if (!p){
				return NULL;
			}
			return p;
		case 9:							// while
			t = GetToken();
			cout << endl << "  WHILE语句:" << endl;
			if (t.kind != LP){
				cout << "Invalid Syntax, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "    条件:" << endl;
			t = GetToken();
			q = Expression(RP);			// 条件表达式
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			r = Sentence();				// while子句
			if (r == NULL)	return NULL;
			p = new TreeNode;
			p->child = q;
			p->next = r;
			return p;
		case 10:						// for
			t = GetToken();
			cout << endl << "  FOR语句:" << endl;
			if (t.kind != LP){
				cout << "Invalid Syntax, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    初始表达式:" << endl;
			q = Expression(SEMI);		// 赋值表达式
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "    条件:" << endl;
			t = GetToken();
			r = Expression(SEMI);		// 条件表达式
			if (r == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    循环后表达式:" << endl;
			s = Expression(RP);			// 循环后处理表达式
			if (s == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    FOR子句:" << endl; 
			u = Sentence();				// for子句
			if (u == NULL)	return NULL;
			p = new TreeNode;
			p->child = q;
			p->next = r;
			p->final = s;
			p->finall = u;
			return p;
		case 11:						// return 
			cout << endl << "  RETURN语句:" << endl;
			t = GetToken();
			q = Expression(SEMI);
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			p = new TreeNode;
			p->child = q;
			t = GetToken();
			return p;
		case 12:						// break
			cout << endl << "  BREAK语句" << endl;
			t = GetToken();
			if (t.kind != SEMI){
				cout << "Missing ';', wrong syntax, line:";
				errors++;
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			p = new TreeNode;
			t = GetToken();
			return p;
		case 14:						// continue
			cout << endl << "  CONTINUE语句" << endl;
			t = GetToken();
			if (t.kind != SEMI){
				cout << "Missing ';', wrong syntax, line:";
				errors++;
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			p = new TreeNode;
			t = GetToken();
			return p;
		case 13:						// do while
			cout << endl << "  DO-WHILE语句:" << endl;
			t = GetToken();
			cout << "    DO子句: " << endl;
			q = Sentence();
			if (q == NULL)	return NULL;
			if (t.kind != 9){
				cout << "Invalid Syntax, expected while, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    WHILE条件: " << endl;
			r = Sentence();
			if (r == NULL)	return NULL;
			t = GetToken();
			if (t.kind != SEMI){
				cout << "Missing ';', wrong syntax, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			p = new TreeNode;
			p->child = q;
			p->next = r;
			t = GetToken();
			return p;
		case LP:						// ( , 表达式以 ) 为结束标志 
			t = GetToken();
			q = Expression(RP);
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			p = new TreeNode;
			p->child = q;
			return p;
		case PLUSPLUS:
		case MINUSMINUS:
		case IDENT:						// 标志符 
		case CHAR_CONST:				// 常量const
		case INT_CONST:
		case FLOAT_CONST:
		case DOUBLE_CONST:
		case LONG_CONST:
		case STRING_CONST:
			// 函数调用，查找是否已定义该函数 
			for (vector<string>::iterator i=function.begin(); i<function.end(); i++){
				if (*i == t.text){
					cout << endl << "    函数调用:" << endl;
					cout << "      函数名: " << *i << endl;
					t = GetToken();
					if (t.kind != LP){
						cout << "Expected '(', line:";
						if (t.line > t.preln)
							cout << t.preln << endl;
						else
							cout << t.line << endl;
						return NULL;
					}
					t = GetToken();
					if (t.kind != IDENT){
						cout << "Not Identifier, line:";
						if (t.line > t.preln)
							cout << t.preln << endl;
						else
							cout << t.line << endl;
						return NULL;
					}
					cout << "      参数: ";
					while(1){
						cout << t.text << " ";
						t = GetToken();
						if (t.kind == RP)
							break;
						else if (t.kind == COMMA){
							t = GetToken();
							continue;
						}
						else{
							cout << "Wrong Syntax, line:";
							if (t.line > t.preln)
								cout << t.preln << endl;
							else
								cout << t.line << endl;
							return NULL;
						}
					}
					cout << endl;
					t = GetToken();
					if (t.kind != SEMI){
						cout << "Expected ';', line:";
						if (t.line > t.preln)
							cout << t.preln << endl;
						else
							cout << t.line << endl;
						return NULL;
					}
					p = new TreeNode;
					t = GetToken();
					return p;
				}
			}
			// 调用表达式处理 
			q = Expression(SEMI);
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			p = new TreeNode;
			p->child = q;
			t = GetToken();
			return p;
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			cout << "    局部变量:" << endl;
			if (t.kind == 6){
				cout << "      CONST常量" << endl;
				t = GetToken();
			}
			keyidx = t.kind;
			p = Localvar(q);
			return p;
		case PRE:
			t = GetToken();
			cout << "  预编译: " << endl;
			if (t.text != "define" && t.text != "include"){
				cout << "Precompiled Error, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			if (t.text == "define"){
				cout << "    宏定义: " << endl;
				t = GetToken();
				if (t.kind != IDENT){
					cout << "Not Identifier, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				cout << "      变量名: " << t.text;
				t = GetToken();
				if (t.kind!=INT_CONST && t.kind!=DOUBLE_CONST){
					cout << endl << "Precomplied Error, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				cout << ", 参数值: " << t.text << endl;
				p = new TreeNode;
				t = GetToken();
				return p;
			}
			else{						// include
				cout << "    文件包含: " << endl;
				t = GetToken();
				if (t.kind != LESS){
					cout << endl << "Precomplied Error, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				t = GetToken();			// 文件名 
				string s = t.text;
				t = GetToken();			// .
				s += t.text;
				t = GetToken();			// 拓展名 
				s += t.text;
				t = GetToken();
				if (t.kind != MORE){
					cout << endl << "Precomplied Error, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				cout << "      文件名: " << s << endl;
				t = GetToken();
				p = new TreeNode;
				return p;
			}
		case LANNO:
			cout << "------------------------------" << endl << "行注释: ";
			str = "";
			line = t.line;
			t = GetToken();
			while (t.line <= line){		// 未换行 
				str += t.text;
				str += " ";
				t = GetToken();
			}
			cout << str << endl << "------------------------------" << endl;
			p = new TreeNode;
			return p;
		case BANNO:
			cout << "------------------------------" << endl << "块注释: " << endl;
			str = "";
			t = GetToken();
			line = t.line;
			while (t.text != "*/"){
				if (t.line>line){
					str += "\n";
					str += t.text;
					line = t.line;
					str += " ";
				}
				else{
					str += t.text;
					str += " ";
				}
				t = GetToken();
			}
			cout << str << endl << "------------------------------" << endl;
			p = new TreeNode;
			t = GetToken();
			return p;
		case RBP:						// }, 复合语句块结束标志 
			return NULL;
		case eof: 
			cout << "文件格式错误EOF" << endl;
			return NULL;
		default:
			errors += 1;
			return NULL;
	} 
}

// 表达式分析 
TreeNode* Expression(int end)
{
	cout << "    表达式:" << endl;
	// 创建映射运算符表 
	map<int, string> mapping;
	mapping[EQUAL] = "==";
	mapping[ASSIGN] = "=";
	mapping[PLUSPLUS] = "++";
	mapping[PLUS] = "+";
	mapping[MINUSMINUS] = "--";
	mapping[MINUS] = "-";
	mapping[MULTIPLY] = "*";
	mapping[DIVIDE] = "/";
	mapping[MORE] = ">";
	mapping[LESS] = "<";
	mapping[notEQ] = "!=";
	mapping[AND] = "&&";
	mapping[OR] = "||";
	mapping[LP] = "(";
	mapping[RP] = ")";
	mapping[SEMI] = ";";
	mapping[MOD] = "%";
	string table[20] = {"+", "-", "*", "/", "(", ")", "=", ">", "<", "==", "!=", "&&", "||", "#", "++", "--", "%"};
	// 创建优先级表 
	char precede[128][128];
	int k = PLUS;
	for (int i=109; i<=123; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][111] = precede[k][112] = precede[k][113] = precede[k][122] = precede[k][123] = precede[k][124] ='<';
	k = MINUS;
	for (int i=109; i<=123; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][111] = precede[k][112] = precede[k][113] = precede[k][122] = precede[k][123] = precede[k][123] ='<';
	k = MULTIPLY;
	for (int i=109; i<=124; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][113] = precede[k][122] = precede[k][123] = '<';
	k = DIVIDE;
	for (int i=109; i<=124; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][113] = precede[k][122] = precede[k][123] = precede[k][124]='<';
	k = LP;
	for (int i=109; i<=124; i++)
		precede[k][i] = '<';
	precede[k][35] = '>';
	precede[k][114] = '=';
	precede[k][115] = precede[k][116] = precede[k][117] = precede[k][118] = precede[k][119] = '>';
	k = RP;
	for (int i=109; i<=124; i++)
		precede[k][i] = '>';
	precede[k][114] = '<';
	precede[k][35] = '>';
	k = ASSIGN;
	for (int i=109; i<=124; i++)
		precede[k][i] = '<';
	precede[k][35] = '>';
	k = MORE;
	for (int i=109; i<=113; i++)
		precede[k][i] = '<';
	for (int i=114; i<=121; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][122] = precede[k][123] = precede[k][124]='<';
	k = LESS;
	for (int i=109; i<=113; i++)
		precede[k][i] = '<';
	for (int i=114; i<=121; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][122] = precede[k][123] = precede[k][124]='<';
	k = EQUAL;
	for (int i=109; i<=124; i++)
		precede[k][i] = '<';
	precede[k][35] = '>';
	precede[k][114] = precede[k][115] = precede[k][118] = precede[k][119] = precede[k][120] = precede[k][121] = '>';
	k = notEQ;
	for (int i=109; i<=124; i++)
		precede[k][i] = '<';
	precede[k][35] = '>';
	precede[k][114] = precede[k][115] = precede[k][118] = precede[k][119] = precede[k][120] = precede[k][121] = '>';
	k = 35;
	for (int i=109; i<=124; i++)
		precede[k][i] = '<';
	precede[k][35] = '=';
	k = AND;
	for (int i=109; i<=124; i++)
		precede[k][i] = '<';
	precede[k][35] = '>';
	precede[k][113] = precede[k][115] = precede[k][120] = precede[k][121] = '>';
	k = OR;
	for (int i=109; i<=124; i++)
		precede[k][i] = '<';
	precede[k][35] = '>';
	precede[k][113] = precede[k][115] = precede[k][121] = '>';
	k = PLUSPLUS;
	for (int i=109; i<=124; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][114] = precede[k][122] = precede[k][123] = '<';
	k = MINUSMINUS;
	for (int i=109; i<=124; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][114] = precede[k][122] = precede[k][123] = '<';
	k = MOD;
	for (int i=109; i<=124; i++)
		precede[k][i] = '>';
	precede[k][35] = '>';
	precede[k][114] = precede[k][122] = precede[k][123] = '<';
	
	stack<int> op;						// 操作符栈 
	stack<TreeNode*> opn; 				// 结点栈 
	op.push('#');
	errors = 0;
	int flag = 0;
	while ((t.kind!='#'||op.top()!='#') && !errors){
		if (t.kind == IDENT||t.kind==CHAR_CONST||t.kind==INT_CONST||t.kind==FLOAT_CONST||t.kind==DOUBLE_CONST||t.kind==LONG_CONST||t.kind==STRING_CONST){
			TreeNode* p = new TreeNode;
			p->token = t.text;
			opn.push(p);
			t = GetToken();
			if (t.kind == IDENT){
				return NULL;
			}
		}
		// 遇到结束标志 ) or ; 
		else if(t.kind == end){
			//cout << "end" << endl;
			t.kind = '#';
		}
		else if (t.kind>=109 && t.kind<=124 || t.kind == '#'){		// 运算符
			flag = 1;
			int m;
			TreeNode *t1, *t2, *p;
			switch (precede[op.top()][t.kind]){		// 优先级比较 
				case '<':
					//cout << "pass<" << endl;
					op.push(t.kind);				// 压栈 
					t = GetToken();
					break;
				case '=':
					//cout << "pass=" << endl;
					if (!(m = op.top())){			// 取栈顶 
						errors++;
					}
					op.pop();
					t = GetToken();
					break;
				case '>':							// 运算并重新压栈 
					//cout << "pass>" << endl;
					if (op.top()==PLUSPLUS || op.top()==MINUSMINUS){
						if (opn.empty()){
							errors++;
							break;
						}
						t1 = opn.top();
						opn.pop();
						if (op.empty()){
							errors++;
							break;
						}
						m = op.top();
						op.pop();
						p = new TreeNode;
						p->token = mapping[m];		// 运算符表
						cout << "      运算符: " << p->token << endl;
						p->child = t1;
						cout << "      ID: " << t1->token << endl;
						opn.push(p);
						break;
					}
					else{
						if (opn.empty()){
							errors++;
							break;
						}
						t2 = opn.top();
						opn.pop();
						if (opn.empty()){
							errors++;
							break;
						}
						t1 = opn.top();
						opn.pop();
						if (op.empty()){
							errors++;
							break;
						}
						m = op.top();
						op.pop();
						p = new TreeNode;
						p->token = mapping[m];		// 运算符表
						cout << "      运算符: " << p->token << endl;
						p->child = t1;
						cout << "      ID: " << t1->token << endl;
						p->next = t2;
						cout << "      ID: " << t2->token << endl;
						opn.push(p);
						break;
					}
				default:
					if (t.kind == end)
						t.kind = '#';
					else{
						errors = 1;
						return NULL;
					}
					break;
			}
		}
		else{
			//cout << "pass errors: " << t.text << endl;
			errors = 1;
			return NULL;
		}
	}
	if (opn.size()==1 && op.top()=='#' && !errors){
		if (!flag){
			cout << "      ID: " << opn.top()->token << endl;
		}
		return opn.top();
	}
	else	return NULL;
}

// 销毁树结点
void DestroyTree(TreeNode*& root)
{
	if (root){
		// 递归销毁每个子树 
		DestroyTree(root->child);
		DestroyTree(root->next);
		DestroyTree(root->final);
		DestroyTree(root->finall);
		delete root;
		root = NULL;
	}
}
