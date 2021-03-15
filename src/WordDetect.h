#ifndef WORDDETECT_H
#define WORDDETECT_H
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <iostream>
#include <map>

using namespace std;
/* BNF:
	< > Required
	[ ] Optional
	{ } Repeatable
	|   OR
	::= Is defined as
	" " String
*/
/*
	ERROR_TOKEN: error token
	eof: end of file
	IDENT: identifier

	CHAR_CONST: const & char
	INT_CONST: const & int
	FLOAT_CONST: const & float
	DOUBLE_CONST: const & double
	LONG_CONST: const & long
	STRING_CONST: const & string

	EQUAL: equal ==
	ASSIGN: assign =
	PLUSPLUS: plusplus ++
	PLUS: plus +
	MINUSMINUS: minusminus --
	MINUS: minus -
	MULTIPLY: multiply *
	DIVIDE: divide /
	MOD: mod %

	MOREEQ: more or equal >=
	MORE: more >
	LESSEQ: less or equal <=
	LESS: less <
	NOTEQ: not qeual !=
	NOT: not !
	AND: and &&
	OR: or ||
	LP: left parentheses (
	RP: right parentheses )
	SEMI: semicolon ;
	COMMA: comma ,
	
	LBP: left big bracket {
	RBP: }
	LK: [
	RK: ]
	PRE: #
	LANNO: //
	BANNO: /* or * /
	DOT: .
*/
enum token_kind {ERROR_TOKEN = 100, eof, IDENT/*=102*/, 
			CHAR_CONST, INT_CONST, FLOAT_CONST, DOUBLE_CONST, LONG_CONST, STRING_CONST, 
				PLUS /*=109*/, MINUS, MULTIPLY, DIVIDE, LP, RP, ASSIGN, MORE, LESS, EQUAL, NOTEQ, AND, OR, PLUSPLUS, MINUSMINUS, MOD/*=124*/, 
					MOREEQ, LESSEQ, notEQ, NOT, SEMI, COMMA, LBP, RBP, LK, RK, PRE, LANNO, BANNO/*=137*/, DOT};
/*  结点定义
	child: first child;
	next: second child;
	final: third child;
	finall: forth child;
	token: text;
*/				
struct TreeNode{
	TreeNode* child = NULL;
	TreeNode* next = NULL;
	TreeNode* final = NULL;
	TreeNode* finall = NULL;
	string token;
};
/*  词汇定义 
	kind: kind of word;
	text: text value of word;
	line: exact line;
*/
struct Word{
	int kind = -1;
	string text;
	int line;
	int preln = 0;
};
/*	树定义 
	lchild: left child of current node
	rchild: right child of current node
	token: corrspoding char saved 
*/
struct tree{
	tree* lchild = NULL;
	tree* rchild = NULL;
	char token;
};
// read file and put words into vector
int GetWords();
// get one word from vector
Word GetToken();
// save to file
int save();
// create tree with tokens 
void CreateTree(vector<char> &s, tree*& T);
// pre-traverse of the tree
void PreTraver(tree *&T, ofstream& fout);
// save into file(VERSION2)
int save_ver2();
// save into file(VERSION3)
int save_ver3(vector<Word> vec); 
// print vector
void PrintWords();
// destroy the tree
void Destoytree(tree*& T);
#endif
 
