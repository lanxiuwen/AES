#include"CryptoFunction.h"
#include"FileFunction.h"

bool EnCryptoFile(char * file_name, unsigned char * key)
{
	unsigned char Buff[BuffRows][BUFFSIZE] = { 0 };//存储待加密数据
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



	long long fileSize = SizeOfFile(file_name);//输入文件大小

	long long BuffRound = fileSize / (BuffRows*BUFFSIZE);//读入缓存区的次数
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFSIZE;//最后一次读入缓存区的数据大小
	int ByteRest = BuffRest%BUFFSIZE;//最后一行的数据大小

	memset(xorBlock, 0, AES::BLOCKSIZE);//置零
	AESEncryption aesEncryptor;//加密实体
	aesEncryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//设置加密密钥

	for (long long i = 0; i < BuffRound; i++)
	{
		ReadBuffFromFile(Buff[0], infile);//读取数据到BUFF

		for (int j = 0; j<BuffRows; j++)//加密buff中的数据
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
		memset((char*)Buff[0], ByteRest, BuffRows*BUFFSIZE);

		infile.read((char*)Buff[0], BuffRest);
		int LastBuffRows = BuffRest / BUFFSIZE;//满数据行数
		int LastRowData = BuffRest%BUFFSIZE;//最后一行不满的数据量
		for (int i = 0; i <LastBuffRows; i++)
		{
			aesEncryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}

		aesEncryptor.ProcessAndXorBlock(Buff[LastBuffRows], xorBlock, outBuff[LastBuffRows]);

		if (LastRowData != 0)//最后一行不满有数据
		{
			ofile.write((char*)outBuff[0], (LastBuffRows + 1)*BUFFSIZE);//最后一个BUFF输出有数据的那几行
		}
		else
		{
			ofile.write((char*)outBuff[0], LastBuffRows*BUFFSIZE);
		}
	}

	unsigned char Flag[AES::BLOCKSIZE] = { 0 };
	memset(Flag, ByteRest, BUFFSIZE);
	unsigned char outFlag[AES::BLOCKSIZE] = { 0 };
	aesEncryptor.ProcessAndXorBlock(Flag, xorBlock, outFlag);
	ofile.write((char*)outFlag, BUFFSIZE);

	infile.close();
	ofile.close();

	return true;
}

bool DeCryptoFile(char * file_name, unsigned char * key)
{
	unsigned char Buff[BuffRows][BUFFSIZE] = { 0 };//存储待解密数据
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


	long long fileSize = SizeOfFile(file_name);//输入文件大小

	long long BuffRound = fileSize / (BuffRows*BUFFSIZE);//读入满缓存区的次数
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFSIZE;//最后一次读入缓存区的数据大小



	memset(xorBlock, 0, AES::BLOCKSIZE);//置零

	AESDecryption aesDecryptor;//加密实体
	aesDecryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//设置解密密钥

	if (BuffRest > BUFFSIZE)//Flag和数据在一个BUFF里，且不是满的BUFF
	{	
		//处理前BuffRound轮数据
		for (long long i = 0; i < BuffRound; i++)
		{
			ReadBuffFromFile(Buff[0], infile);//读取数据到BUFF

			for (int j = 0; j<BuffRows; j++)//解密buff中的数据
			{
				aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//解密
			}
			WriteDataFromBuff(outBuff[0], ofile);//输出buff中的数据
			ClearBuff((char*)Buff[0]);
			ClearBuff((char*)outBuff);
		}

		//处理最后一轮数据
		infile.read((char*)Buff[0], BuffRest);
		int LastBuffRows = BuffRest / BUFFSIZE;

		for (int i = 0; i <LastBuffRows; i++)
		{
			aesDecryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}

		int ByteRest = outBuff[LastBuffRows - 1][BUFFSIZE - 1];
		if (ByteRest != 0)
		{
			ofile.write((char*)outBuff[0], BuffRest + ByteRest - 2 * BUFFSIZE);
		}
		else
		{
			ofile.write((char*)outBuff[0], BuffRest  -  BUFFSIZE);
		}

	}

	else if(BuffRest<=16&&BuffRest>0)//Flag单独一行
	{
		//处理前BuffRound-1轮数据
		for (long long i = 0; i < BuffRound-1; i++)
		{
			ReadBuffFromFile(Buff[0], infile);//读取数据到BUFF

			for (int j = 0; j<BuffRows; j++)//解密buff中除最后一行的数据
			{
				aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//解密
			}
			WriteDataFromBuff(outBuff[0], ofile);//输出buff中的数据
			ClearBuff((char*)Buff[0]);
			ClearBuff((char*)outBuff);
		}
		//处理最后一轮数据

		ReadBuffFromFile(Buff[0], infile);

		for (int i = 0; i <BuffRows; i++)//解密有数据的最后一个BUFF
		{
			aesDecryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}
		unsigned char inFlag[AES::BLOCKSIZE] = { 0 };
		unsigned char outFlag[AES::BLOCKSIZE] = { 0 };
		infile.read((char*)inFlag, BUFFSIZE);
		aesDecryptor.ProcessAndXorBlock(inFlag, xorBlock, outFlag);
		int ByteRest = outFlag[1];
		if (ByteRest == 0)
			WriteDataFromBuff(outBuff[0], ofile);//整个数据块全是有效数据
		else
		{
			ofile.write((char*)outBuff[0], (BuffRows-1)*BUFFSIZE+ByteRest);//最后一行只有部分有效数据
		}

	}

	else//数据块恰好满的，且最后一行为FALG
	{
		//处理前BuffRound-1轮数据
		for (long long i = 0; i < BuffRound - 1; i++)
		{
			ReadBuffFromFile(Buff[0], infile);//读取数据到BUFF

			for (int j = 0; j<BuffRows; j++)//解密buff中除最后一行的数据
			{
				aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//解密
			}
			WriteDataFromBuff(outBuff[0], ofile);//输出buff中的数据
			ClearBuff((char*)Buff[0]);
			ClearBuff((char*)outBuff);
		}

		//处理最后一轮数据
		ReadBuffFromFile(Buff[0], infile);

		for (int i = 0; i <BuffRows; i++)//解密有数据的最后一个BUFF
		{
			aesDecryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}

		int ByteRest= outBuff[BuffRows - 1][BUFFSIZE - 1];//最后一行为flag数据

		if (ByteRest == 0)
		{
			ofile.write((char*)outBuff[0], BuffRows*BUFFSIZE - BUFFSIZE);//除最后一行为FLAG外全是数据
		}			
		else
		{
			ofile.write((char*)outBuff[0], (BuffRows-2)*BUFFSIZE+ByteRest);//最后一行只有部分有效数据
		}
	}


	infile.close();
	ofile.close();

	return true;
}