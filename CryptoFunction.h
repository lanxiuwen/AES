
#pragma once

#include"FileFunction.h"
#include<aes.h>

#pragma comment(lib,"cryptlib.lib")

using namespace std;
using namespace CryptoPP;

bool EnCryptoFile(char * file_name, unsigned char * key);
bool DeCryptoFile(char * file_name, unsigned char * key); 
