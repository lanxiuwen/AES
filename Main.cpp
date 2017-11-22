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
	cout << "*AES�ļ�����*" << endl;
	cout << setw(40);
	cout << "**************" << endl;
}

void ShowRootMenu()
{
	cout << endl;
	cout << setw(38);
	cout << "��ѡ����" << endl;
	cout << endl;
	cout << setw(38);
	cout << "1  �����ļ�" << endl;
	cout << endl;
	cout << setw(38);
	cout << "2  �����ļ�" << endl;

}

void WaitUser()
{
	int iInputPage = 0;
	cout << "B �������˵� Q �˳�" << endl;
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
		if (ch == '\b') //�˸��
		{
			if (Key.size() != 0)
			{
				cout << '\b' << " " << '\b';
				Key.pop_back();
			}
		}
		else if (ch == '\r') //�س���
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
	cout << "Tips:�ļ�����Ҫ�пո�,���벻����16λ" << endl;
	cout << "Ҫ���ܵ��ļ���";
	cin >> file_name;
	cout<< "��������" << endl;
	InputKey((char*)key);

	long long file_size = SizeOfFile((char*)file_name.c_str());

	start = clock();
	cout << "������" << endl;
	EnCryptoFile((char*)file_name.c_str(), key);

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;

	cout << "���ܽ���" << endl;
	cout << endl;
	cout << "������ʱ��" << duration << " ��" << endl;
	cout << "ƽ�������ٶȣ�" << (double)file_size / 1024 / 1024 / duration << " M/S" << endl;

	WaitUser();
}

void DecrypFile()
{
	string file_name;
	clock_t start, finish;
	double duration;
	unsigned char key[AES::DEFAULT_KEYLENGTH] = { 0 };
	cout << "Tips:�ļ�����Ҫ�пո����벻����16λ" << endl;
	cout << "Ҫ���ܵ��ļ���";
	cin >> file_name;
	cout << "��������" << endl;
	InputKey((char*)key);


	long long file_size = SizeOfFile((char*)file_name.c_str());

	start = clock();
	cout << "������" << endl;
	DeCryptoFile((char*)file_name.c_str(), key);
	
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "���ܽ���" << endl;
	cout << "������ʱ��" << duration << " ��" << endl;
	cout << "ƽ�������ٶȣ�" << (double)file_size / 1024 / 1024 / duration << " MB/S" << endl;
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