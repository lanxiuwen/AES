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
	cout << "待加密文件：" << endl;
	cin >> fileName;
	cout << "输入密码" << endl;
	cin >> key;
	EnCryptoFile((char*)fileName.c_str(), key);
	memset(key, 0, AES::DEFAULT_KEYLENGTH);
	cout << "带解密的文件" << endl;
	cin >> defilename;
	cout << "输入密码" << endl;
	cin >> key;
	DeCryptoFile((char *)defilename.c_str(), key);

	return 0;



}