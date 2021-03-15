#ifndef SYNTAXANALYSE_H
#define SYNTAXANALYSE_h

#include <stack> 
#include "WordDetect.h"
extern vector<string> keywords;
extern vector<Word> token_text;
/*
<程序> ：：=<外部定义序列>
<外部定义序列>：：=<外部定义> <外部定义序列>  |  <外部定义>
<外部定义>：：=<外部变量定义>|  <函数定义>
<外部变量定义>：：=<类型说明符> <变量序列> ；
<类型说明符>：：= int | float | char
<变量序列>：：=<变量>  ，  <变量序列>  |  <变量>
<函数定义>：：=<类型说明符> <函数名>（<形式参数序列>）<复合语句>
<形式参数序列>：：=<形式参数> ， <形式参数序列>  |  <空>
<形式参数>：：=<类型说明符> 标识符
<复合语句>：：={ <局部变量定义序列>  <语句序列> }
<局部变量定义序列>：：=<局部变量定义>  <局部变量定义序列>  |  <空>
<局部变量定义>：：=<类型说明符> <变量序列> ；
<语句序列>：：=<语句><语句序列> | <空>
<语句>：：= <表达式>； | return  <表达式>；
           | if （<表达式>）<语句>
		   | if （<表达式>）<语句> else <语句>
<表达式>：：=<表达式> + <表达式>  |  <表达式> - <表达式> |<表达式> * <表达式>
            |<表达式> / <表达式>  | INT_CONST | IDENT | IDENT(<实参序列>)
			|<表达式> == <表达式>  |<表达式> != <表达式>  |<表达式> > <表达式>  
			|<表达式>  > <表达式>  |<表达式> >= <表达式> |<表达式>  <  <表达式>  
			|<表达式>  <=  <表达式>  | 标识符=<表达式>
<实参序列>：：=<表达式> <实参序列>  |  <空> 
*/

TreeNode* Program(TreeNode*& root);			// 源程序分析入口 
TreeNode* Extdeflist(TreeNode*& root);		// 外部定义序列 
TreeNode* Extdef(TreeNode*& root);			// 外部定义分析 
TreeNode* Extvardef(TreeNode*& root);		// 外部变量序列 
TreeNode* Funcdef(TreeNode*& root);			// 函数定义 
TreeNode* FormPara(TreeNode*& root);		// 形式参数定义 
TreeNode* Compound(TreeNode*& root);		// 复合语句 
TreeNode* Localvar(TreeNode*& root);		// 局部变量定义 
TreeNode* Sentenlist(TreeNode*& root);		// 语句序列 
TreeNode* Sentence();						// 语句分析 
TreeNode* Expression(int end);				// 表达式分析 
void DestroyTree(TreeNode*& root);			// 销毁树结点 
#endif
