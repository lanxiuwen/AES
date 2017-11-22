#pragma once
#include"FileFunction.h"

using namespace std;

long long SizeOfFile(char * file_loc)
{
	long long size;
	fstream file(file_loc,ios::binary|ios::in);
	if (!file)
		return -1;
	file.seekg(0, ios::end);
	size = file.tellg();
	file.close();
	return size;
}

void ReadBuffFromFile(unsigned char * buff, fstream &infile)//????????BUFF
{
	infile.read((char*)buff, BUFFSIZE*BuffRows);
}

void WriteDataFromBuff(unsigned char * buff, ofstream & ofile)//??????buff
{
	ofile.write((char*)buff, BUFFSIZE*BuffRows);
}

void ClearBuff(char * Buff)
{
	memset(Buff, 0, BuffRows*BUFFSIZE);
}