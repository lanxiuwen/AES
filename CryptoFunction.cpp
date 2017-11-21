#include"CryptoFunction.h"
#include"FileFunction.h"

bool EnCryptoFile(char * file_name, unsigned char * key)
{
	unsigned char Buff[BuffRows][BUFFKSIZE] = { 0 };//存储待加密数据
	unsigned char xorBlock[AES::BLOCKSIZE];
	unsigned char outBuff[BuffRows][AES::BLOCKSIZE] = { 0 };//输出数据块

															//生成加密后文件的名字
	string newFileName(file_name);
	int dot_pos;
	for (dot_pos = newFileName.size() - 1; dot_pos >= 0; dot_pos--)
	{
		if (newFileName[dot_pos] == '.')
			break;
	}
	if (dot_pos > 0)
	{
		newFileName.insert(dot_pos, "_Encryped");
	}

	//打开输出加密后的文件
	ofstream ofile(newFileName, ios::binary);
	//打开输入文件
	fstream infile(file_name, ios::binary | ios::in);
	try
	{
		if (!infile)
		{
			throw "file open erro";
		}
	}
	catch (...)
	{
		return false;
	}



	long fileSize = SizeOfFile(file_name);//输入文件大小

	long BuffRound = fileSize / (BuffRows*BUFFKSIZE);//读入缓存区的次数
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFKSIZE;//最后一次读入缓存区的数据大小



	long Round = fileSize / BUFFKSIZE;
	int rest = fileSize%BUFFKSIZE;

	memset(xorBlock, 0, AES::BLOCKSIZE);//置零
	AESEncryption aesEncryptor;//加密实体
	aesEncryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//设置加密密钥

	for (int i = 0; i < BuffRound; i++)
	{
		ReadBuffFromFile(Buff[0], infile);//读取数据到BUFF

		for (long j = 0; j<BuffRows; j++)//加密buff中的数据
		{
			aesEncryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//加密
		}

		WriteDataFromBuff(outBuff[0], ofile);//输出buff中的数据
		ClearBuff((char*)Buff[0]);
		ClearBuff((char*)outBuff);
	}

	//处理最后一个BUFF里的数据
	if (BuffRest != 0)
	{
		int ByteRest = BuffRest%BUFFKSIZE;
		memset((char*)Buff[0], ByteRest, BuffRows*BUFFKSIZE);

		infile.read((char*)Buff[0], BuffRest);
		int LastBuffRows = BuffRest / BUFFKSIZE;

		for (int i = 0; i <LastBuffRows; i++)
		{
			aesEncryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}

		aesEncryptor.ProcessAndXorBlock(Buff[LastBuffRows], xorBlock, outBuff[LastBuffRows]);
		ofile.write((char*)outBuff[0], (LastBuffRows + 1)*BUFFKSIZE);//最后一个BUFF输出有数据的那几行


	}







	infile.close();
	ofile.close();

	return true;
}

bool DeCryptoFile(char * file_name, unsigned char * key)
{
	unsigned char Buff[BuffRows][BUFFKSIZE] = { 0 };//存储待解密数据
	unsigned char xorBlock[AES::BLOCKSIZE];
	unsigned char outBuff[BuffRows][AES::BLOCKSIZE] = { 0 };//输出数据块

															//解密后文件的名字
	string newFileName(file_name);
	int dot_pos;
	for (dot_pos = newFileName.size() - 1; dot_pos >= 0; dot_pos--)
	{
		if (newFileName[dot_pos] == '.')
			break;
	}
	newFileName.insert(dot_pos, "_Decryped");

	ofstream ofile(newFileName, ios::binary);//输出解密后的文件

	fstream infile(file_name, ios::binary | ios::in);

	if (!infile)
	{
		throw "file open erro";
		return false;
	}


	long fileSize = SizeOfFile(file_name);//输入文件大小

	long BuffRound = fileSize / (BuffRows*BUFFKSIZE);//读入缓存区的次数
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFKSIZE;//最后一次读入缓存区的数据大小



	memset(xorBlock, 0, AES::BLOCKSIZE);//置零

	AESDecryption aesDecryptor;//加密实体
	aesDecryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//设置解密密钥

													 //对前BUFFROUND组buff解密
	for (int i = 0; i < BuffRound; i++)
	{
		ReadBuffFromFile(Buff[0], infile);//读取数据到BUFF

		for (long j = 0; j<BuffRows; j++)//解密buff中的数据
		{
			aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//解密
		}
		WriteDataFromBuff(outBuff[0], ofile);//输出buff中的数据
		ClearBuff((char*)Buff[0]);
		ClearBuff((char*)outBuff);
	}

	//处理最后一个BUFF里的数据
	if (BuffRest != 0)
	{


		infile.read((char*)Buff[0], BuffRest);
		int LastBuffRows = BuffRest / BUFFKSIZE;

		for (int i = 0; i <LastBuffRows; i++)
		{
			aesDecryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}

		int ByteRest = outBuff[LastBuffRows - 1][BUFFKSIZE - 1];

		ofile.write((char*)outBuff[0], BuffRest + ByteRest - BUFFKSIZE);


	}


	infile.close();
	ofile.close();

	return true;
}