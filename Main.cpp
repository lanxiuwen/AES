#include<iostream>
#include<iomanip>
#include<stdlib.h>
#include<conio.h>
#include<string>
#include<string.h>
#include<fstream>
#include<time.h>
#include"CryptoFunction.h"
#include"FileFunction.h"

#define CMD_COLS 80
#define CMD_LINES 25

using namespace std;

void SetScreenGrid()
{
	char sysSetBuf[80];
	sprintf_s(sysSetBuf, "Mode con cols=%d lines=%d", CMD_COLS, CMD_LINES);
	system(sysSetBuf);
}

void SetSysCaption()
{
	system("title Sample");
}

void ClearScreen()
{
	system("cls");
}

void SetSysCaption(const char * pText)
{
	char sysSetBuf[80];
	sprintf_s(sysSetBuf, "title %s", pText);
	system(sysSetBuf);
}

void ShowWelcome()
{
	for (int i = 0; i < 7; i++)
		cout << endl;
	cout << setw(40);
	cout << "**************" << endl;
	cout << setw(40);
	cout << "*AES文件加密*" << endl;
	cout << setw(40);
	cout << "**************" << endl;
}

void ShowRootMenu()
{
	cout << endl;
	cout << setw(38);
	cout << "请选择功能" << endl;
	cout << endl;
	cout << setw(38);
	cout << "1  加密文件" << endl;
	cout << endl;
	cout << setw(38);
	cout << "2  解密文件" << endl;

}

void WaitUser()
{
	int iInputPage = 0;
	cout << "B 返回主菜单 Q 退出" << endl;
	char c[256];
	cin >> c;
	if (c[0] == 'q')
		system("exit");
}

int GetSelect()
{
	char buf[256];
	gets_s(buf);
	return atoi(buf);
}

void InputKey(char * key)
{
	string Key;
	char ch;
	while (1)
	{
		ch = getch();
		if (ch == '\b') //退格键
		{
			if (Key.size() != 0)
			{
				cout << '\b' << " " << '\b';
				Key.pop_back();
			}
		}
		else if (ch == '\r') //回车键
		{
			cout << endl;
			break;
		}
		else
		{
			cout << "*";
			Key += ch;
		}
	}

	char * KeyStr = (char*)Key.c_str();
	for (int i = 0; i < Key.size(); i++)
	{
		if (AES::DEFAULT_KEYLENGTH == i)
			break;
		key[i] = KeyStr[i];
	}

}


void EncrypFile()
{
	clock_t start, finish;
	double duration;
	string file_name;

	unsigned char key[AES::DEFAULT_KEYLENGTH] = { 0 };
	cout << "Tips:文件名不要有空格,密码不超过16位" << endl;
	cout << "要加密的文件：";
	cin >> file_name;
	cout<< "输入密码" << endl;
	InputKey((char*)key);

	long long file_size = SizeOfFile((char*)file_name.c_str());

	start = clock();
	cout << "加密中" << endl;
	EnCryptoFile((char*)file_name.c_str(), key);

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;

	cout << "加密结束" << endl;
	cout << endl;
	cout << "加密用时：" << duration << " 秒" << endl;
	cout << "平均加密速度：" << (double)file_size / 1024 / 1024 / duration << " M/S" << endl;

	WaitUser();
}

void DecrypFile()
{
	string file_name;
	clock_t start, finish;
	double duration;
	unsigned char key[AES::DEFAULT_KEYLENGTH] = { 0 };
	cout << "Tips:文件名不要有空格，密码不超过16位" << endl;
	cout << "要解密的文件：";
	cin >> file_name;
	cout << "输入密码" << endl;
	InputKey((char*)key);


	long long file_size = SizeOfFile((char*)file_name.c_str());

	start = clock();
	cout << "解密中" << endl;
	DeCryptoFile((char*)file_name.c_str(), key);
	
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "解密结束" << endl;
	cout << "解密用时：" << duration << " 秒" << endl;
	cout << "平均解密速度：" << (double)file_size / 1024 / 1024 / duration << " MB/S" << endl;
	WaitUser();
}

void mainloop()
{
	ShowWelcome();
	while (1)
	{
		ClearScreen();
		ShowWelcome();
		ShowRootMenu();
		switch (GetSelect())
		{
		case 1:
			ClearScreen();
			EncrypFile();
			break;
		case 2:
			ClearScreen();
			DecrypFile();
			break;
		}
	}
}

int main(void)
{

	mainloop();
	return 0;


}