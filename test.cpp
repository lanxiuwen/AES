#include<iostream>
#include<fstream>
#include<string>
#include"FileFunction.h"
#include"CryptoFunction.h"

using namespace std;
using namespace CryptoPP;

int main(void)
{
	unsigned char key[AES::DEFAULT_KEYLENGTH] = { 0 };

	string fileName;
	string defilename;
	cout << "�������ļ���" << endl;
	cin >> fileName;
	cout << "��������" << endl;
	cin >> key;
	EnCryptoFile((char*)fileName.c_str(), key);
	memset(key, 0, AES::DEFAULT_KEYLENGTH);
	cout << "�����ܵ��ļ�" << endl;
	cin >> defilename;
	cout << "��������" << endl;
	cin >> key;
	DeCryptoFile((char *)defilename.c_str(), key);

	return 0;



}