#include "WordDetect.h"
#include "SyntaxAnalyse.h"
#include <windows.h>
extern char* FileIn;		// �����ļ� 
extern char* FileOut;		// д���ļ�
extern Word t;
extern Word w;
extern vector<Word> token_text;
int main() {
	// ���ݽ��� 
	cout << "The file you are reading: \""<< FileIn <<"\"" << endl;
	Sleep(1000);
	cout << "Now parsing the file" << endl;
	for (int i=0; i<30; i++){
		cout << ".";
		Sleep(100);			// ϵͳÿ��ѭ����ͣ0.1s 
	}
	cout << endl;
	
	int flag = GetWords();
	// �������ʼ���ظ�ʽ�Ƿ�Ϸ�
	PrintWords();
	if (flag != 1){
		cout << "ERROR TOKEN, EXIT." << endl;
		return 0;
	}
	system("pause");
	system("cls");
	TreeNode* root = NULL;
	TreeNode* p = NULL;
	root = Program(p);				// ���н���Դ���� 
	if (root||t.kind==eof){
		cout << "End of Parsing." << endl;
	}
	else if (!root){
		return 0;
	}
	system("pause");
	system("cls");
	//int n = save_ver2();			// д��ָ���ļ�
	int n = save_ver3(token_text);
	if (n == 1){
		cout << "Successfully Saved in \"" << FileOut << "\"." << endl;
	}
	cout << "Now open the file \"" << FileOut << "\"......." << endl;
	system("pause");
	ifstream read;
	read.open(FileOut, ios::in);
	char ch[100];
	string s;
	while(read.getline(ch, 100)){
		s = ch;
		cout << s << endl;
		if (read.peek() == EOF){		// �����ļ�β 
			break;
		}
	}
	read.close();
	return 0;
}
