#include "stdafx.h"
#include "Support.h"

void ThongBaoLoiVaThoat(CSocket &Connector)
{
	cout << "Client " << std::this_thread::get_id() << " da ngat ket noi\n";
	Connector.Close();
	ExitThread(1);
}

vector<File*> ReadFileList()
{
	std::vector<File*> FileList;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(_T(".\\Shared Folder\\*"), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		FindNextFile(hFind, &FindFileData);
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			char* FileName = new char[101];
			wcstombs(FileName, FindFileData.cFileName, 100);
			FileList.push_back(new File(FileName, FindFileData.nFileSizeLow));
			delete FileName;
			FileName = 0;
		}
		FindClose(hFind);
	}
	return FileList;
}

void SendFile(CSocket &Connector, File* file)
{
	char* fName = file->GetName();
	int fNameLength = strlen(fName);
	int fSize = file->GetSize();
	Connector.Send(&fNameLength, sizeof(int), 0);			//Gửi độ dài tên file
	Connector.Send(fName, fNameLength, 0);					//Gửi tên file
	Connector.Send(&fSize, sizeof(int), 0);					//Gửi file size
}

void SendFileList(CSocket &Connector, const vector<File*> &FileList)
{
	int FileListSize = FileList.size();
	Connector.Send(&FileListSize, sizeof(int), 0);			//Gửi số file
	for(int i = 0; i < FileListSize; i++)
		SendFile(Connector, FileList[i]);
}

void Provide(CSocket &Connector, File* file)
{
	numClientDownloading++;
	char* filePath = new char[strlen(file->GetName()) + 14 + 1];
	filePath[0] = '\0';
	strcat(filePath, "Shared Folder\\");
	strcat(filePath, file->GetName());
	File temp(filePath, file->GetSize());
	int pos = 0;
	int current_size = 0;
	int file_size = temp.GetSize();
	char current_string[BYTE];
	while(current_size < file_size)
	{
		int trueLength = temp.Get(pos, BYTE,current_string);
		if (current_string!=NULL)
		{
			current_size += BYTE;
			pos = current_size;
		}
		if(Connector.Send(&trueLength, sizeof(int), 0)  == SOCKET_ERROR)
		{
			numClientDownloading--;
			ThongBaoLoiVaThoat(Connector);
		}
		if(Connector.Send(current_string, trueLength, 0) == SOCKET_ERROR)
		{
			numClientDownloading--;
			ThongBaoLoiVaThoat(Connector);
		}
	}
	cout << "Client " << std::this_thread::get_id() << " hoan tat tai file " << file->GetName() << " \n";
	numClientDownloading--;
	delete[] filePath;
}