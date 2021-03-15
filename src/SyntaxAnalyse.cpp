#include "SyntaxAnalyse.h"
// �����ǣ���errors>0���ر��� 
int errors = 0;
// ���ڱ���ؼ���kind
int keyidx;
// ���ڱ����ı�ֵ 
string text;
Word t;
int formflag = 0;
// ��¼�Ѷ���ĺ������ں��ڵ��� 
vector<string> function;
int constflag = 0;
extern int count;
extern int num_lbp, num_rbp;
extern int num_lba, num_rba;
extern int num_lp, num_rp;

// Դ���������� 
TreeNode* Program(TreeNode*& root)
{
	cout << endl << "����:" << endl;
	t = GetToken();
	// �����ⲿ�������� 
	if (Extdeflist(root)||t.kind==eof)
		return root;
	else{
		cout << "Wrong Syntax." << endl;
		DestroyTree(root);
		return NULL;
	}
}

// �ⲿ��������
TreeNode* Extdeflist(TreeNode*& root)
{
	// Ԥ�ж������Ƿ���� 
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
	// �жϵ����ļ�β 
	if (t.kind == eof){
		cout << endl << "End of File" << endl;
		return NULL;
	}
	root = new TreeNode;
	// �����ⲿ���� 
	TreeNode *p = Extdef(root->child);
	if (!p){
		DestroyTree(root);
		return NULL;
	}
	// �ݹ�����ⲿ�������� 
	TreeNode *q = Extdeflist(root->next);
	if (!q){
		DestroyTree(root);
		return NULL;
	}
	return root;
}

// �ⲿ�������
TreeNode* Extdef(TreeNode*& root)
{
	TreeNode* p = NULL;
	while (t.kind == PRE){				// # 
		t = GetToken();
		cout << "  Ԥ����: " << endl;
		if (t.text != "define" && t.text != "include"){
			cout << "Precompiled Error, line:";
			if (t.line > t.preln)
				cout << t.preln << endl;
			else
				cout << t.line << endl;
			return NULL;
		}
		if (t.text == "define"){		// �궨�� 
			cout << "    �궨��: " << endl;
			t = GetToken();
			if (t.kind != IDENT){
				cout << "Not Identifier, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "      ������: " << t.text;
			t = GetToken();
			if (t.kind!=INT_CONST && t.kind!=DOUBLE_CONST){
				cout << endl << "Precomplied Error, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << ", ����ֵ: " << t.text << endl;
			p = new TreeNode;
			t = GetToken();
			return p;
		}
		else{							// include, �ļ����� 
			cout << "    �ļ�����: " << endl;
			t = GetToken();
			if (t.kind != LESS){
				cout << endl << "Precomplied Error, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();				// �ļ��� 
			string s = t.text;
			t = GetToken();				// .
			s += t.text;
			t = GetToken();				// ��չ�� 
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
			cout << "      �ļ���: " << s << endl;
			t = GetToken();
			p = new TreeNode;
			return p;
		}
	}
	// �������û������ʼ��
	while (t.kind == IDENT){ 
		// �������ã����Ѷ��庯���в��ҵ����Ƿ���� 
		for (vector<string>::iterator i=function.begin(); i<function.end(); i++){ 
			if (*i == t.text){
				cout << endl << "    ��������:" << endl;
				cout << "      ������: " << *i << endl;
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
				cout << "      ����: ";
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
					//�ж��Ƿ���
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
		// ���ñ��ʽ�����ʼ�����ʽ 
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
	// ����ע��(������ע�ͺͿ�ע��) 
	while (t.kind == LANNO){		// ��ע�� 
		cout << "------------------------------" << endl << "��ע��: ";
		string s = "";
		int line = t.line;
		t = GetToken();
		while (t.line <= line){		// δ���� 
			s += t.text;
			s += " ";
			t = GetToken();
		}
		cout << s << endl << "------------------------------" << endl;
		p = new TreeNode;
		return p;
	}
	while (t.kind == BANNO){
		cout << "------------------------------" << endl << "��ע��: " << endl;
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
	// ����ؼ���kind 
	keyidx = t.kind;
	// const���� 
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
	// �����ı�ֵtext 
	text = t.text;
	t = GetToken();
	root = new TreeNode;
	// �����ⲿ�������� 
	if (t.kind != LP)
		p = Extvardef(root->child);
	// ���ú������� 
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

// �ⲿ��������
TreeNode* Extvardef(TreeNode*& root)
{
	cout << endl << "  �ⲿ��������:" << endl;
	root = new TreeNode;
	root->child = new TreeNode;
	root->child->token = keywords[keyidx];
	if (constflag){
		cout << "    CONST����" << endl;
		constflag = 0;
	}
	cout << "    ���ͣ�" << keywords[keyidx] << endl;
	TreeNode* p = root->next;
	p = new TreeNode;
	p->child = new TreeNode;
	p->child->token = text;
	cout << "    ������: " << text << endl;
	int skip = 0;
	while(1){
		if (t.kind == ASSIGN){			// e.g. int i=3;
			cout << "    ��ʼ�����ASSIGN: " << endl;
			t = GetToken();
			TreeNode *o = Sentence();
			if (!o){
				return NULL;
			}
			skip = 1;
		}
		if (t.kind == LK){				// ���� int a[10];
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
			cout << "    ��������: ����, size: " << size << endl;
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
		cout << "    ������: " << t.text << endl;
		t = GetToken();
	}
}

// �������� 
TreeNode* Funcdef(TreeNode*& root)
{
	cout << endl << "  ����:" << endl;
	root = new TreeNode;
	root->child = new TreeNode;
	root->child->token = keywords[keyidx];
	cout << "    ����: " << keywords[keyidx] << endl;
	root->next = new TreeNode;
	root->next->token = text;
	function.push_back(text);
	cout << "    ������: " << text << endl;
	cout << "    �����β�: " << endl;
	// �����β� 
	TreeNode *r = FormPara(root->next->child);
	if (!r){
		return NULL;
	}
	t = GetToken();
	TreeNode *p = NULL;
	// ֮�����ע�� 
	if (t.kind == LANNO){
		cout << "------------------------------" << endl << "��ע��: ";
		string s = "";
		int line = t.line;
		t = GetToken();
		while (t.line <= line){			// δ����,�Ի��з�Ϊע�ͽ�����־ 
			s += t.text;
			s += " ";
			t = GetToken();
		}
		cout << s << endl << "------------------------------" << endl;
	}
	else if (t.kind == BANNO){
		cout << "------------------------------" << endl << "��ע��: " << endl;
		string s = "";
		t = GetToken();
		int line = t.line;
		while (t.text != "*/"){			// ��*/Ϊ��ע�ͽ�����־ 
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
	// �������� 
	if (t.kind == SEMI){
		t = GetToken();
		root->final = NULL;
		cout << "      ����: ��������" << endl;
		root = new TreeNode;
		return root;
	}
	// �������� 
	else if (t.kind == LBP){
		cout << "      ����: ��������" << endl;
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

// ��ʽ��������
TreeNode* FormPara(TreeNode*& root)
{
	t = GetToken();
	root = new TreeNode;
	TreeNode* p = root;
	// ���β� 
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
	// �����β� 
	else if (t.kind < ERROR_TOKEN){		// keywords
		formflag = 0;
		if (t.kind == 6){
			cout << "      CONST����" << endl;
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
		cout << "      ����: " << t.text << ", ";
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
		cout << "������: " << t.text << endl;
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

// ������� 
TreeNode* Compound(TreeNode*& root)
{
	root = new TreeNode;
	// �������鿪ʼ 
	cout << "  Start of Compound Sentence {" << endl;
	t = GetToken();
	TreeNode* p = NULL;
	TreeNode *q = NULL;
	if (t.kind == 6){			//const����
		constflag = 1;
		t = GetToken();
	}
	if (t.kind <= 5){
		keyidx = t.kind;
		cout << "    �ֲ�����:" << endl;
		// ���þֲ���������
		p = Localvar(root->child);
	}
	else{
		root->child = NULL;
	}
	// ����������� 
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

// �ֲ��������� 
TreeNode* Localvar(TreeNode*& root)
{
	root = new TreeNode;
	root->child = new TreeNode;
	root->child->child = new TreeNode;
	root->child->child->token = keywords[keyidx];
	if (constflag){
		cout << "      CONST����" << endl;
		constflag = 0;
	}
	cout << "      ����: " << keywords[keyidx] << ", ";
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
	cout << "������: " << t.text << endl;
	t = GetToken();
	int skip = 0;
	if (t.kind == ASSIGN){			// e.g. int i=3;
		cout << "      ��ʼ�����ASSIGN: " << endl;
		t = GetToken();
		TreeNode *o = Sentence();
		if (!o){
			return NULL;
		}
		skip = 1;
	}
	if (t.kind == LK){				// ���� int a[10];
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
		cout << "      ��������: ����, size: " << size << endl;
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
	// �ݹ���þֲ��������� 
	TreeNode* p = Localvar(root->next);
	if (!p){
		DestroyTree(root);
		return NULL;
	}
	return root;
}

// ������� 
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
		// ����������� 
		else{
			cout << "  End of Compound Sentence }" << endl;
			return root;
		}
	}
	else{
		root = new TreeNode;
		root->child = p;
		// �ݹ����������� 
		TreeNode *q = Sentenlist(root->next);
		if (!q && errors>0){
			return NULL;
		}
		return root;
	}
}

// ������
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
			cout << endl << "  IF���:" << endl; 
			if (t.kind != LP){
				cout << "Invalid Syntax, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "    ����:" << endl;
			t = GetToken();
			q = Expression(RP);			// �������ʽ
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    IF�Ӿ�:" << endl;
			r = Sentence();				// if�Ӿ�
			if (r == NULL){
				return NULL;
			}
			if (t.kind == 8){			// else
				t = GetToken();
				cout << "    ELSE�Ӿ�:" << endl;
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
			cout << endl << "  WHILE���:" << endl;
			if (t.kind != LP){
				cout << "Invalid Syntax, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "    ����:" << endl;
			t = GetToken();
			q = Expression(RP);			// �������ʽ
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			r = Sentence();				// while�Ӿ�
			if (r == NULL)	return NULL;
			p = new TreeNode;
			p->child = q;
			p->next = r;
			return p;
		case 10:						// for
			t = GetToken();
			cout << endl << "  FOR���:" << endl;
			if (t.kind != LP){
				cout << "Invalid Syntax, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    ��ʼ���ʽ:" << endl;
			q = Expression(SEMI);		// ��ֵ���ʽ
			if (q == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			cout << "    ����:" << endl;
			t = GetToken();
			r = Expression(SEMI);		// �������ʽ
			if (r == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    ѭ������ʽ:" << endl;
			s = Expression(RP);			// ѭ��������ʽ
			if (s == NULL){
				cout << "Wrong Expression, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			t = GetToken();
			cout << "    FOR�Ӿ�:" << endl; 
			u = Sentence();				// for�Ӿ�
			if (u == NULL)	return NULL;
			p = new TreeNode;
			p->child = q;
			p->next = r;
			p->final = s;
			p->finall = u;
			return p;
		case 11:						// return 
			cout << endl << "  RETURN���:" << endl;
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
			cout << endl << "  BREAK���" << endl;
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
			cout << endl << "  CONTINUE���" << endl;
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
			cout << endl << "  DO-WHILE���:" << endl;
			t = GetToken();
			cout << "    DO�Ӿ�: " << endl;
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
			cout << "    WHILE����: " << endl;
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
		case LP:						// ( , ���ʽ�� ) Ϊ������־ 
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
		case IDENT:						// ��־�� 
		case CHAR_CONST:				// ����const
		case INT_CONST:
		case FLOAT_CONST:
		case DOUBLE_CONST:
		case LONG_CONST:
		case STRING_CONST:
			// �������ã������Ƿ��Ѷ���ú��� 
			for (vector<string>::iterator i=function.begin(); i<function.end(); i++){
				if (*i == t.text){
					cout << endl << "    ��������:" << endl;
					cout << "      ������: " << *i << endl;
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
					cout << "      ����: ";
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
			// ���ñ��ʽ���� 
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
			cout << "    �ֲ�����:" << endl;
			if (t.kind == 6){
				cout << "      CONST����" << endl;
				t = GetToken();
			}
			keyidx = t.kind;
			p = Localvar(q);
			return p;
		case PRE:
			t = GetToken();
			cout << "  Ԥ����: " << endl;
			if (t.text != "define" && t.text != "include"){
				cout << "Precompiled Error, line:";
				if (t.line > t.preln)
					cout << t.preln << endl;
				else
					cout << t.line << endl;
				return NULL;
			}
			if (t.text == "define"){
				cout << "    �궨��: " << endl;
				t = GetToken();
				if (t.kind != IDENT){
					cout << "Not Identifier, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				cout << "      ������: " << t.text;
				t = GetToken();
				if (t.kind!=INT_CONST && t.kind!=DOUBLE_CONST){
					cout << endl << "Precomplied Error, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				cout << ", ����ֵ: " << t.text << endl;
				p = new TreeNode;
				t = GetToken();
				return p;
			}
			else{						// include
				cout << "    �ļ�����: " << endl;
				t = GetToken();
				if (t.kind != LESS){
					cout << endl << "Precomplied Error, line:";
					if (t.line > t.preln)
						cout << t.preln << endl;
					else
						cout << t.line << endl;
					return NULL;
				}
				t = GetToken();			// �ļ��� 
				string s = t.text;
				t = GetToken();			// .
				s += t.text;
				t = GetToken();			// ��չ�� 
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
				cout << "      �ļ���: " << s << endl;
				t = GetToken();
				p = new TreeNode;
				return p;
			}
		case LANNO:
			cout << "------------------------------" << endl << "��ע��: ";
			str = "";
			line = t.line;
			t = GetToken();
			while (t.line <= line){		// δ���� 
				str += t.text;
				str += " ";
				t = GetToken();
			}
			cout << str << endl << "------------------------------" << endl;
			p = new TreeNode;
			return p;
		case BANNO:
			cout << "------------------------------" << endl << "��ע��: " << endl;
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
		case RBP:						// }, �������������־ 
			return NULL;
		case eof: 
			cout << "�ļ���ʽ����EOF" << endl;
			return NULL;
		default:
			errors += 1;
			return NULL;
	} 
}

// ���ʽ���� 
TreeNode* Expression(int end)
{
	cout << "    ���ʽ:" << endl;
	// ����ӳ��������� 
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
	// �������ȼ��� 
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
	
	stack<int> op;						// ������ջ 
	stack<TreeNode*> opn; 				// ���ջ 
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
		// ����������־ ) or ; 
		else if(t.kind == end){
			//cout << "end" << endl;
			t.kind = '#';
		}
		else if (t.kind>=109 && t.kind<=124 || t.kind == '#'){		// �����
			flag = 1;
			int m;
			TreeNode *t1, *t2, *p;
			switch (precede[op.top()][t.kind]){		// ���ȼ��Ƚ� 
				case '<':
					//cout << "pass<" << endl;
					op.push(t.kind);				// ѹջ 
					t = GetToken();
					break;
				case '=':
					//cout << "pass=" << endl;
					if (!(m = op.top())){			// ȡջ�� 
						errors++;
					}
					op.pop();
					t = GetToken();
					break;
				case '>':							// ���㲢����ѹջ 
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
						p->token = mapping[m];		// �������
						cout << "      �����: " << p->token << endl;
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
						p->token = mapping[m];		// �������
						cout << "      �����: " << p->token << endl;
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

// ���������
void DestroyTree(TreeNode*& root)
{
	if (root){
		// �ݹ�����ÿ������ 
		DestroyTree(root->child);
		DestroyTree(root->next);
		DestroyTree(root->final);
		DestroyTree(root->finall);
		delete root;
		root = NULL;
	}
}
