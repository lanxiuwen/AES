#include"CryptoFunction.h"
#include"FileFunction.h"

bool EnCryptoFile(char * file_name, unsigned char * key)
{
	unsigned char Buff[BuffRows][BUFFKSIZE] = { 0 };//�洢����������
	unsigned char xorBlock[AES::BLOCKSIZE];
	unsigned char outBuff[BuffRows][AES::BLOCKSIZE] = { 0 };//������ݿ�

															//���ɼ��ܺ��ļ�������
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

	//��������ܺ���ļ�
	ofstream ofile(newFileName, ios::binary);
	//�������ļ�
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



	long fileSize = SizeOfFile(file_name);//�����ļ���С

	long BuffRound = fileSize / (BuffRows*BUFFKSIZE);//���뻺�����Ĵ���
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFKSIZE;//���һ�ζ��뻺���������ݴ�С



	long Round = fileSize / BUFFKSIZE;
	int rest = fileSize%BUFFKSIZE;

	memset(xorBlock, 0, AES::BLOCKSIZE);//����
	AESEncryption aesEncryptor;//����ʵ��
	aesEncryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//���ü�����Կ

	for (int i = 0; i < BuffRound; i++)
	{
		ReadBuffFromFile(Buff[0], infile);//��ȡ���ݵ�BUFF

		for (long j = 0; j<BuffRows; j++)//����buff�е�����
		{
			aesEncryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//����
		}

		WriteDataFromBuff(outBuff[0], ofile);//���buff�е�����
		ClearBuff((char*)Buff[0]);
		ClearBuff((char*)outBuff);
	}

	//�������һ��BUFF�������
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
		ofile.write((char*)outBuff[0], (LastBuffRows + 1)*BUFFKSIZE);//���һ��BUFF��������ݵ��Ǽ���


	}







	infile.close();
	ofile.close();

	return true;
}

bool DeCryptoFile(char * file_name, unsigned char * key)
{
	unsigned char Buff[BuffRows][BUFFKSIZE] = { 0 };//�洢����������
	unsigned char xorBlock[AES::BLOCKSIZE];
	unsigned char outBuff[BuffRows][AES::BLOCKSIZE] = { 0 };//������ݿ�

															//���ܺ��ļ�������
	string newFileName(file_name);
	int dot_pos;
	for (dot_pos = newFileName.size() - 1; dot_pos >= 0; dot_pos--)
	{
		if (newFileName[dot_pos] == '.')
			break;
	}
	newFileName.insert(dot_pos, "_Decryped");

	ofstream ofile(newFileName, ios::binary);//������ܺ���ļ�

	fstream infile(file_name, ios::binary | ios::in);

	if (!infile)
	{
		throw "file open erro";
		return false;
	}


	long fileSize = SizeOfFile(file_name);//�����ļ���С

	long BuffRound = fileSize / (BuffRows*BUFFKSIZE);//���뻺�����Ĵ���
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFKSIZE;//���һ�ζ��뻺���������ݴ�С



	memset(xorBlock, 0, AES::BLOCKSIZE);//����

	AESDecryption aesDecryptor;//����ʵ��
	aesDecryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//���ý�����Կ

													 //��ǰBUFFROUND��buff����
	for (int i = 0; i < BuffRound; i++)
	{
		ReadBuffFromFile(Buff[0], infile);//��ȡ���ݵ�BUFF

		for (long j = 0; j<BuffRows; j++)//����buff�е�����
		{
			aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//����
		}
		WriteDataFromBuff(outBuff[0], ofile);//���buff�е�����
		ClearBuff((char*)Buff[0]);
		ClearBuff((char*)outBuff);
	}

	//�������һ��BUFF�������
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