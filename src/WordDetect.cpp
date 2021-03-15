#include "WordDetect.h"
// 统计行数 
int count;
// 关键字数组 
vector<string> keywords = {"char", "short", "int", "float", "double", "long", "const", "if", "else", "while", "for", "return", "break", "do", "continue"/*=14*/};

map<int, string> kind2str = {{IDENT,"IDENT"}, {CHAR_CONST,"CHAR_CONST"}, {INT_CONST,"INT_CONST"}, {FLOAT_CONST,"FLOAT_CONST"}, {DOUBLE_CONST,"DOUBLE_CONST"}, 
				{LONG_CONST,"LONG_CONST"}, {STRING_CONST,"STRING_CONST"}, {PLUS,"OPERATOR"}, {MINUS,"OPERATOR"}, {MULTIPLY,"OPERATOR"}, {DIVIDE,"OPERATOR"}, 
				{LP,"MARK"}, {RP,"MARK"}, {ASSIGN,"OPERATOR"}, {MORE,"OPERATOR"}, {LESS,"OPERATOR"}, {EQUAL,"OPERATOR"}, {NOTEQ,"OPERATOR"}, {AND,"OPERATOR"}, 
				{OR,"OPERATOR"}, {PLUSPLUS,"OPERATOR"}, {MINUSMINUS,"OPERATOR"}, {MOD,"OPERATOR"}, {MOREEQ,"OPERATOR"}, {LESSEQ,"OPERATOR"}, {notEQ,"OPERATOR"},
				 {NOT,"OPERATOR"}, {SEMI,"MARK"}, {COMMA,"MARK"}, {LBP,"MARK"}, {RBP,"MARK"}, {LK,"MARK"}, {RK,"MARK"}, {PRE,"MARK"}, {LANNO,"MARK"}, 
				 {DOT, "MARK"},{BANNO,"MARK"}};
// 存储单词 
vector<Word> token_text;
// 文件名（读取和写入） 
const char *FileIn = "D:\\testIn.cpp";
const char *FileOut = "D:\\testOut_ver2.cpp";
/*r
	num_rbp: number of }
	num_lbp: number of {
	num_lba: number of /*
	num_rba: number of * /
	num_lp: number of (
	num_rp: number of )
*/
int num_rbp = 0;
int num_lbp = 0;
int num_lba = 0;
int num_rba = 0;
int num_lp = 0;
int num_rp = 0;
Word w;
// 错误标志
int error = 0;
vector<Word> errortoken;
// 读取文件中的所有单词到vector中, 无异常返回1, 异常返回-1 
int GetWords()
{
	ifstream read;
	read.open(FileIn,ios::in);
	if (!read){
		cout << "File Not Open" << endl;
		return -1;
	}
	count = 1; 
	char word;
	string s;
	int num;
	while(read.get(word)){
		if (read.peek() == EOF){	// 是否到达文件尾 
			w.kind = eof;
			w.line = count;
			w.text = "EOF";
			if (!token_text.empty()){
				w.preln = token_text[token_text.size()-1].line;
			}
			token_text.push_back(w);
			break;
		}
		if (word == ' '||word == '\t')
			continue;				// filter the blank char
		else if (word == '\n')
			count++;				// count the number of lines			
		else{
			if (isalpha(word)){		// IDENT or Key words
				s = word;
				while(read.get(word) && (isalpha(word) || isdigit(word)))	// read the next char
					s += word;
				read.unget();		// put back
				w.text = s;
				num = 0;
				int flag = 0;
				for (vector<string>::iterator i=keywords.begin(); i<keywords.end(); i++){
					if (*i == s){
						w.kind = num;
						flag = 1;
						break;
					}
					num++;
				}
				if (!flag)
					w.kind = IDENT;
				w.line = count;
				if (!token_text.empty()){
					w.preln = token_text[token_text.size()-1].line;
				}
				token_text.push_back(w);
			}
			
			else if (isdigit(word)){	// INT(LONG)_CONST or FLOAT(DOUBLE)_CONST
				s = word;
				char ch = word;
				read.get(word);
				if (isalpha(word)&&word!='x'&&word!='X'){
					s+=word;
					w.kind = ERROR_TOKEN;
					w.line = count;
					if (!token_text.empty()){
						w.preln = token_text[token_text.size()-1].line;
					}
					w.text = s;
					errortoken.push_back(w);
					error++;
				}
				read.unget();
				int flag = 0, hexflag = 0;
				while (read.get(word)){
					if (word == '.' && flag == 0){	// count '.' only once
						s += word;
						flag = 1;
					}
					else if (isdigit(word))
						s += word;
					else if (ch=='0' && (word == 'x' || word == 'X') && !hexflag){
						s += word;
						hexflag = 1;
					}					
					else
						break;
				}
				if (s[1] == 'x' || s[1] == 'X'){	// hexadecimal prefix
					read.unget();
					if (s.size() > 2){
						w.kind = INT_CONST;
						w.text = s;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						continue;
					}
					else{
						w.kind = ERROR_TOKEN;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						return -1;
					}
				}
				if ((word == 'l' || word == 'L') && !flag){		// postfix
					s += word;
					w.kind = LONG_CONST;
					w.text = s;
					w.line = count;
					if (!token_text.empty()){
						w.preln = token_text[token_text.size()-1].line;
					}
					token_text.push_back(w);
					continue;
				}
				else if ((word == 'f' || word == 'F') && flag){
					s += word;
					w.kind = FLOAT_CONST;
					w.text = s;
					w.line = count;
					if (!token_text.empty()){
						w.preln = token_text[token_text.size()-1].line;
					}
					token_text.push_back(w);
					continue;
				}
				read.unget();
				w.text = s;
				if (flag){
					w.kind = DOUBLE_CONST;
					w.line = count;
					if (!token_text.empty()){
						w.preln = token_text[token_text.size()-1].line;
					}
					token_text.push_back(w);
				}
				else{
					w.kind = INT_CONST;
					w.line = count;
					if (!token_text.empty()){
						w.preln = token_text[token_text.size()-1].line;
					}
					token_text.push_back(w);
				}
			}
			
			else{				// other cases
				switch (word){
					case '=': 
						read.get(word);
						if (word == '='){	// ==
							w.kind = EQUAL;
							w.text = "==";
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{				// =
							read.unget();
							w.text = "=";
							w.kind = ASSIGN;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '+':
						read.get(word);
						if (word == '+'){	// ++
							w.text = "++";
							w.kind = PLUSPLUS;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{				// +
							read.unget();
							w.text = "+";
							w.kind = PLUS;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '-':
						read.get(word);
						if (word == '-'){	// --
							w.text = "--";
							w.kind = MINUSMINUS;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{				// -
							read.unget();
							w.text = "-";
							w.kind = MINUS;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '*':				// */
						read.get(word);
						if (word == '/'){
							num_rba++;
							w.text = "*/";
							w.kind = BANNO;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{				// *
							read.unget();
							w.text = "*";
							w.kind = MULTIPLY;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '/':
						read.get(word);
						if (word == '/'){
							w.text = "//";
							w.kind = LANNO;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else if (word == '*'){
							num_lba++;
							w.text = "/*";
							w.kind = BANNO;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{
							read.unget();
							w.text = "/";
							w.kind = DIVIDE;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '%':
						w.text = "%";
						w.kind = MOD;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case '>':
						read.get(word);
						if (word == '='){
							w.text = ">=";
							w.kind = MOREEQ;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{
							read.unget();
							w.text = ">";
							w.kind = MORE;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '<':
						read.get(word);
						if (word == '='){
							w.text = "<=";
							w.kind = LESSEQ;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{
							read.unget();
							w.text = "<";
							w.kind = LESS;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '!':
						read.get(word);
						if (word == '='){
							w.text = "!=";
							w.kind = notEQ;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{
							read.unget();
							w.text = "!";
							w.kind = NOT;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						break;
					case '&':
						read.get(word);
						if (word == '&'){
							w.text = "&&";
							w.kind = AND;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{
							w.kind = ERROR_TOKEN;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							w.text = word;
							errortoken.push_back(w);
							error++;
						}
						break;
					case '|':
						read.get(word);
						if (word == '|'){
							w.text = "||";
							w.kind = OR;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{
							w.kind = ERROR_TOKEN;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							w.text = word;
							errortoken.push_back(w);
							error++;
						}
						break;
					case '(':
						num_lp++;
						w.text = "(";
						w.kind = LP;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case ')':
						num_rp++;
						w.text = ")";
						w.kind = RP;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case ';':
						w.text = ";";
						w.kind = SEMI;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case ',':
						w.text = ",";
						w.kind = COMMA;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case '\'':
						s = word;
						read.get(word);
						s += word;
						read.get(word);
						if (word == '\''){
							s += word;
							w.text = s;
							w.kind = CHAR_CONST;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							token_text.push_back(w);
						}
						else{
							w.kind = ERROR_TOKEN;
							w.line = count;
							if (!token_text.empty()){
								w.preln = token_text[token_text.size()-1].line;
							}
							w.text = word;
							errortoken.push_back(w);
							error++;
						}
						break;
					case '\"':
						s = word;
						do{
							read.get(word);
							s += word;
						} while (word != '\"');
						w.text = s;
						w.kind = STRING_CONST;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case '{':
						num_lbp++;
						w.text = "{";
						w.kind = LBP;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case '}':
						num_rbp++;
						w.text = "}";
						w.kind = RBP;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case '[':
						w.text = "[";
						w.kind = LK;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case ']':
						w.text = "]";
						w.kind = RK;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case '#':
						w.text = "#";
						w.kind = PRE;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					case '.':
						w.text = ".";
						w.line = count;
						w.kind = DOT;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						token_text.push_back(w);
						break;
					default:{
						w.kind = ERROR_TOKEN;
						w.line = count;
						if (!token_text.empty()){
							w.preln = token_text[token_text.size()-1].line;
						}
						w.text = word;
						errortoken.push_back(w);
						error++;
						break;
					}
				}
			}
		}
	}
	read.close();
	if (error>0)	return -1;
	return 1;
}

// 返回下一个词汇 
Word GetToken()
{
	static int num = 0;
	return token_text[num++];
}

// 按照格式写入文件 
int save()
{
	ifstream in;
	in.open(FileIn, ios::in);
	if (!in){
		cout << "File Not Open" << endl;
		return -1;
	}
	char ch[100];
	string s;
	ofstream fout;
	fout.open(FileOut);
	while(in.getline(ch, 100)){
		s = ch;
		fout << s << endl;
		if (in.peek() == EOF){		// 到达文件尾 
			break;
		}
	}
	in.close();
	fout.close();
	return 1;
}
//创建树
void CreateTree(vector<char> &s, tree*& T)
{
	static int cnt = 0;
	if (cnt == s.size())
		return;
	T = new tree;
	T->token = s[cnt++];
	CreateTree(s, T->lchild);
	CreateTree(s, T->rchild);
}
// 先序遍历
void PreTraver(tree *&T, ofstream& fout)
{
	if (!T){
		return;
	}
	fout << T->token;
	PreTraver(T->lchild, fout);
	PreTraver(T->rchild, fout);
}
// 销毁树
void Dstroytree(tree *&T)
{
	if (T){
		Dstroytree(T->lchild);
		Dstroytree(T->rchild);
		delete T;
		T = NULL;
	} 
} 
// 保存文件VERSION2 
int save_ver2()
{
	ofstream fout;
	fout.open(FileOut);
	ifstream in;
	in.open(FileIn, ios::in);
	if (!in){
		cout << "File Not Open" << endl;
		return -1;
	}
	char word;
	vector<char> s;
	while(in.get(word)){
		s.push_back(word);			// 存储所有字符 
		if (in.peek()==EOF)
			break;
	}
	tree *T = NULL;					// 根 
	CreateTree(s, T);				// 创建树
	PreTraver(T, fout);				// 先序遍历 
	fout.close();
	in.close();
	Dstroytree(T);
	return 1;
}
int save_ver3(vector<Word> vec)
{
	ofstream fout;
	fout.open(FileOut);
	if (!fout){
		cout << "File Not Open" << endl;
		return -1;
	}
	int tab = 0;
	fout << vec[0].text << " ";
	for (int i=1; i<vec.size(); i++){
		if (vec[i].kind == eof)
			break;
		if (vec[i].line > vec[i].preln){		// 换行 
			fout << endl;
			if (vec[i-1].kind == 131)
				tab++;
			if (vec[i].kind == 132)
				tab--;
			for (int i=0; i<tab; i++)
				fout << "\t";
			fout << vec[i].text;
			if (vec[i].kind==LK || vec[i+1].kind==RK)
				continue;
			if (vec[i].kind==LP || vec[i+1].kind==RP)
				continue;
			if (vec[i+1].kind!=SEMI)
				fout << " ";
		}
		else{
			if (vec[i-1].kind == 131)
				tab++;
			fout << vec[i].text;
			if (vec[i].kind==LK || vec[i+1].kind==RK)
				continue;
			if (vec[i].kind==LP || vec[i+1].kind==RP)
				continue;
			if (vec[i+1].kind!=SEMI)
				fout << " ";
		}
	}
	fout.close();
	return 1;
}
// 输出vector中所有词汇
void PrintWords()
{
	vector<string> vec;
	int flag, flag2;
	cout << "KIND: KEY_WORDS" << endl;			// 关键字 
	for (int j=0; j<15; j++){
		for (int i=0; i<token_text.size(); i++){
			flag = 1;
			if (token_text[i].kind==j){
				for (int k=0; k<vec.size(); k++){
					if (token_text[i].text == vec[k]){
						flag = 0;
						break;
					}
				}
				if (flag){
					cout << "  " << token_text[i].text << endl;
					vec.push_back(token_text[i].text);
				}
				else	continue;
			}
		}
	}
	vec.clear();
	vector<string> vec2;
	for (int j=102; j<139; j++){
		if (j == 113 || j==114)
			continue;
		flag2 = 1;
		for (int m=0; m<vec2.size(); m++){
			if (kind2str[j] == vec2[m]){
				flag2 = 0;
				break;
			}
		}
		if (flag2){
			cout << "KIND: " << kind2str[j] << endl;		// 其他种类 
			vec2.push_back(kind2str[j]);
		}
		for (int i=0; i<token_text.size(); i++){
			flag = 1;
			if (token_text[i].kind==j){
				for (int k=0; k<vec.size(); k++){
					if (token_text[i].text == vec[k]){
						flag = 0;
						break;
					}
				}
				if (flag){
					cout << "  " << token_text[i].text << endl;
					vec.push_back(token_text[i].text);
				}
				else	continue;
			}
		}
	}
	vec.clear();
	for (int i=0; i<token_text.size(); i++){
		flag = 1;
		for (int k=0; k<vec.size(); k++){
			if (token_text[i].text == vec[k]){
				flag = 0;
				break;
			}
		}
		if (flag && (token_text[i].kind == 113 ||token_text[i].kind == 114)){
			cout << "  " << token_text[i].text << endl;
			vec.push_back(token_text[i].text);
		}
	}
	cout << "ERROR TOKEN:" << endl;				// 错误符号 
	vec.clear();
	for (int i=0; i<errortoken.size(); i++){
		cout << "  " << errortoken[i].text << "  line:" << errortoken[i].line << endl;
		vec.push_back(errortoken[i].text);
	}
	if (vec.empty()){
		cout << "  No error token" << endl;
	}
}
