#include "WordDetect.h"
#include "SyntaxAnalyse.h"
#include <windows.h>
extern char* FileIn;		// 读入文件 
extern char* FileOut;		// 写入文件
extern Word t;
extern Word w;
extern vector<Word> token_text;
int main() {
	// 短暂交互 
	cout << "The file you are reading: \""<< FileIn <<"\"" << endl;
	Sleep(1000);
	cout << "Now parsing the file" << endl;
	for (int i=0; i<30; i++){
		cout << ".";
		Sleep(100);			// 系统每个循环暂停0.1s 
	}
	cout << endl;
	
	int flag = GetWords();
	// 检测各单词及相关格式是否合法
	PrintWords();
	if (flag != 1){
		cout << "ERROR TOKEN, EXIT." << endl;
		return 0;
	}
	system("pause");
	system("cls");
	TreeNode* root = NULL;
	TreeNode* p = NULL;
	root = Program(p);				// 运行解析源程序 
	if (root||t.kind==eof){
		cout << "End of Parsing." << endl;
	}
	else if (!root){
		return 0;
	}
	system("pause");
	system("cls");
	//int n = save_ver2();			// 写入指定文件
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
		if (read.peek() == EOF){		// 到达文件尾 
			break;
		}
	}
	read.close();
	return 0;
}
