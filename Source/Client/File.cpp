#include "stdafx.h"
#include "File.h"

File::File(const char *Name, unsigned int size)
{
	this->m_name = new char[strlen(Name) + 1];
	strcpy(this->m_name, Name);
	m_size = size;
}

File::~File()
{
	delete[] m_name;
	m_name = NULL;
	m_size = 0;
}

int File::GetSize()
{
	return this->m_size;
}

char * File::GetName()
{
	char* temp = new char[strlen(m_name) + 1];
	strcpy(temp, m_name);
	return temp;
}

int File::Get(int pos, int bytes, char result[])
{
	FILE *f = fopen(this->m_name, "rb");
	if(f)
	{
		fseek(f, pos, SEEK_SET);
		int trueLength;
		trueLength = fread(result, sizeof(char), bytes, f);
		fclose(f);
		return trueLength;
	}
	return NULL;
}

void File::Merge(char* S, int trueLength)
{
	FILE *f = fopen(this->m_name, "ab");
	m_size += trueLength;
	fwrite(S, sizeof(char), trueLength, f);
	fclose(f);
}

void File::Show()
{
	cout << m_name << " " << m_size << endl;
}

void File::changeName(char *Name)
{
	m_name = new char[strlen(Name) + 1];
	strcpy(m_name, Name);
}