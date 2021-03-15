#ifndef SYNTAXANALYSE_H
#define SYNTAXANALYSE_h

#include <stack> 
#include "WordDetect.h"
extern vector<string> keywords;
extern vector<Word> token_text;
/*
<����> ����=<�ⲿ��������>
<�ⲿ��������>����=<�ⲿ����> <�ⲿ��������>  |  <�ⲿ����>
<�ⲿ����>����=<�ⲿ��������>|  <��������>
<�ⲿ��������>����=<����˵����> <��������> ��
<����˵����>����= int | float | char
<��������>����=<����>  ��  <��������>  |  <����>
<��������>����=<����˵����> <������>��<��ʽ��������>��<�������>
<��ʽ��������>����=<��ʽ����> �� <��ʽ��������>  |  <��>
<��ʽ����>����=<����˵����> ��ʶ��
<�������>����={ <�ֲ�������������>  <�������> }
<�ֲ�������������>����=<�ֲ���������>  <�ֲ�������������>  |  <��>
<�ֲ���������>����=<����˵����> <��������> ��
<�������>����=<���><�������> | <��>
<���>����= <���ʽ>�� | return  <���ʽ>��
           | if ��<���ʽ>��<���>
		   | if ��<���ʽ>��<���> else <���>
<���ʽ>����=<���ʽ> + <���ʽ>  |  <���ʽ> - <���ʽ> |<���ʽ> * <���ʽ>
            |<���ʽ> / <���ʽ>  | INT_CONST | IDENT | IDENT(<ʵ������>)
			|<���ʽ> == <���ʽ>  |<���ʽ> != <���ʽ>  |<���ʽ> > <���ʽ>  
			|<���ʽ>  > <���ʽ>  |<���ʽ> >= <���ʽ> |<���ʽ>  <  <���ʽ>  
			|<���ʽ>  <=  <���ʽ>  | ��ʶ��=<���ʽ>
<ʵ������>����=<���ʽ> <ʵ������>  |  <��> 
*/

TreeNode* Program(TreeNode*& root);			// Դ���������� 
TreeNode* Extdeflist(TreeNode*& root);		// �ⲿ�������� 
TreeNode* Extdef(TreeNode*& root);			// �ⲿ������� 
TreeNode* Extvardef(TreeNode*& root);		// �ⲿ�������� 
TreeNode* Funcdef(TreeNode*& root);			// �������� 
TreeNode* FormPara(TreeNode*& root);		// ��ʽ�������� 
TreeNode* Compound(TreeNode*& root);		// ������� 
TreeNode* Localvar(TreeNode*& root);		// �ֲ��������� 
TreeNode* Sentenlist(TreeNode*& root);		// ������� 
TreeNode* Sentence();						// ������ 
TreeNode* Expression(int end);				// ���ʽ���� 
void DestroyTree(TreeNode*& root);			// ��������� 
#endif
