#include"CryptoFunction.h"
#include"FileFunction.h"

bool EnCryptoFile(char * file_name, unsigned char * key)
{
	unsigned char Buff[BuffRows][BUFFSIZE] = { 0 };//�洢����������
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



	long long fileSize = SizeOfFile(file_name);//�����ļ���С

	long long BuffRound = fileSize / (BuffRows*BUFFSIZE);//���뻺�����Ĵ���
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFSIZE;//���һ�ζ��뻺���������ݴ�С
	int ByteRest = BuffRest%BUFFSIZE;//���һ�е����ݴ�С

	memset(xorBlock, 0, AES::BLOCKSIZE);//����
	AESEncryption aesEncryptor;//����ʵ��
	aesEncryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//���ü�����Կ

	for (long long i = 0; i < BuffRound; i++)
	{
		ReadBuffFromFile(Buff[0], infile);//��ȡ���ݵ�BUFF

		for (int j = 0; j<BuffRows; j++)//����buff�е�����
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
		memset((char*)Buff[0], ByteRest, BuffRows*BUFFSIZE);

		infile.read((char*)Buff[0], BuffRest);
		int LastBuffRows = BuffRest / BUFFSIZE;//����������
		int LastRowData = BuffRest%BUFFSIZE;//���һ�в�����������
		for (int i = 0; i <LastBuffRows; i++)
		{
			aesEncryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}

		aesEncryptor.ProcessAndXorBlock(Buff[LastBuffRows], xorBlock, outBuff[LastBuffRows]);

		if (LastRowData != 0)//���һ�в���������
		{
			ofile.write((char*)outBuff[0], (LastBuffRows + 1)*BUFFSIZE);//���һ��BUFF��������ݵ��Ǽ���
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
	unsigned char Buff[BuffRows][BUFFSIZE] = { 0 };//�洢����������
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


	long long fileSize = SizeOfFile(file_name);//�����ļ���С

	long long BuffRound = fileSize / (BuffRows*BUFFSIZE);//�������������Ĵ���
	int BuffRest = fileSize - BuffRound * BuffRows*BUFFSIZE;//���һ�ζ��뻺���������ݴ�С



	memset(xorBlock, 0, AES::BLOCKSIZE);//����

	AESDecryption aesDecryptor;//����ʵ��
	aesDecryptor.SetKey(key, AES::DEFAULT_KEYLENGTH);//���ý�����Կ

	if (BuffRest > BUFFSIZE)//Flag��������һ��BUFF��Ҳ�������BUFF
	{	
		//����ǰBuffRound������
		for (long long i = 0; i < BuffRound; i++)
		{
			ReadBuffFromFile(Buff[0], infile);//��ȡ���ݵ�BUFF

			for (int j = 0; j<BuffRows; j++)//����buff�е�����
			{
				aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//����
			}
			WriteDataFromBuff(outBuff[0], ofile);//���buff�е�����
			ClearBuff((char*)Buff[0]);
			ClearBuff((char*)outBuff);
		}

		//�������һ������
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

	else if(BuffRest<=16&&BuffRest>0)//Flag����һ��
	{
		//����ǰBuffRound-1������
		for (long long i = 0; i < BuffRound-1; i++)
		{
			ReadBuffFromFile(Buff[0], infile);//��ȡ���ݵ�BUFF

			for (int j = 0; j<BuffRows; j++)//����buff�г����һ�е�����
			{
				aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//����
			}
			WriteDataFromBuff(outBuff[0], ofile);//���buff�е�����
			ClearBuff((char*)Buff[0]);
			ClearBuff((char*)outBuff);
		}
		//�������һ������

		ReadBuffFromFile(Buff[0], infile);

		for (int i = 0; i <BuffRows; i++)//���������ݵ����һ��BUFF
		{
			aesDecryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}
		unsigned char inFlag[AES::BLOCKSIZE] = { 0 };
		unsigned char outFlag[AES::BLOCKSIZE] = { 0 };
		infile.read((char*)inFlag, BUFFSIZE);
		aesDecryptor.ProcessAndXorBlock(inFlag, xorBlock, outFlag);
		int ByteRest = outFlag[1];
		if (ByteRest == 0)
			WriteDataFromBuff(outBuff[0], ofile);//�������ݿ�ȫ����Ч����
		else
		{
			ofile.write((char*)outBuff[0], (BuffRows-1)*BUFFSIZE+ByteRest);//���һ��ֻ�в�����Ч����
		}

	}

	else//���ݿ�ǡ�����ģ������һ��ΪFALG
	{
		//����ǰBuffRound-1������
		for (long long i = 0; i < BuffRound - 1; i++)
		{
			ReadBuffFromFile(Buff[0], infile);//��ȡ���ݵ�BUFF

			for (int j = 0; j<BuffRows; j++)//����buff�г����һ�е�����
			{
				aesDecryptor.ProcessAndXorBlock(Buff[j], xorBlock, outBuff[j]);//����
			}
			WriteDataFromBuff(outBuff[0], ofile);//���buff�е�����
			ClearBuff((char*)Buff[0]);
			ClearBuff((char*)outBuff);
		}

		//�������һ������
		ReadBuffFromFile(Buff[0], infile);

		for (int i = 0; i <BuffRows; i++)//���������ݵ����һ��BUFF
		{
			aesDecryptor.ProcessAndXorBlock(Buff[i], xorBlock, outBuff[i]);
		}

		int ByteRest= outBuff[BuffRows - 1][BUFFSIZE - 1];//���һ��Ϊflag����

		if (ByteRest == 0)
		{
			ofile.write((char*)outBuff[0], BuffRows*BUFFSIZE - BUFFSIZE);//�����һ��ΪFLAG��ȫ������
		}			
		else
		{
			ofile.write((char*)outBuff[0], (BuffRows-2)*BUFFSIZE+ByteRest);//���һ��ֻ�в�����Ч����
		}
	}


	infile.close();
	ofile.close();

	return true;
}