// Test No1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include "File.h"
#include <vector>
#include <iomanip>
#include<string>

using namespace std;
void ThongBaoLoiVaThoat(CSocket &Client, int errorNumber)
{
	switch(errorNumber)
	{
	case 0:
		cout << "Server dang ban\n";
		break;
	case 1:
		cout << "Server da bi tat\n";
		break;
	}
	system("Pause");
	Client.Close();
	exit(1);
}

vector<File*> receiveFileList(CSocket &cs)
{
	vector<File*> fileList;
	char *FName;
	unsigned int Fsize;
	int FName_Length;
	int Lsize = -1;
	cs.Receive(&Lsize,sizeof(int),0);
	if (Lsize == -1)
		ThongBaoLoiVaThoat(cs, 0);
	cout << "Da ket noi\n";
	cout << "Co " << Lsize << " file tren server" << endl;
	for (int i = 0; i < Lsize; i++)
	{
		cs.Receive(&FName_Length,sizeof(int),0);
		FName = new char[FName_Length+1];
		cs.Receive(FName,FName_Length,0);
		FName[FName_Length]='\0';
		cs.Receive(&Fsize,sizeof(int),0);
		File* temp = new File(FName,Fsize);
		fileList.push_back(temp);
	}
	return fileList;
}

void printFileList(vector<File*> fileList)
{
	for (int i = 0; i < fileList.size();i++)
	{
		cout<< i+1 << ": "<<fileList[i]->GetName() << "\t " << fileList[i]->GetSize() << endl;
	}
}

void Get(CSocket &Client, File* file)
{
	//Client nhận các bytes từ server
	char* filePath = new char[strlen(file->GetName()) + 10 + 1];
	filePath[0] = '\0';
	strcat(filePath, "Downloads\\");
	strcat(filePath, file->GetName());
	File Downloaded_File (filePath, 0);
	FILE *f = fopen(filePath, "wb");
	fclose(f);
	int current_length = 0;
	int byte;
	int total_length = file->GetSize();
	while(current_length < total_length)
	{
		if(Client.Receive(&byte, sizeof(int), 0)  == SOCKET_ERROR)
			ThongBaoLoiVaThoat(Client,1);
		char *current_string = new char[byte + 1];
		if(Client.Receive((char*)current_string, byte, 0)  == SOCKET_ERROR)
			ThongBaoLoiVaThoat(Client,1);
		current_string[byte] = NULL;
		if(current_string==NULL)
			ThongBaoLoiVaThoat(Client,1);
		else
		{
			Downloaded_File.Merge(current_string, byte);
			current_length += byte;
			std::cout << std::fixed;
			cout << fixed << std::setprecision(2) << current_length / (float)total_length * 100 << "%    \r";
		}
	}
	delete[] filePath;
}
// The one and only application object

CWinApp theApp;

bool checkFileName(vector<File*> List, char *filename)
{
	int ListSize = List.size();
	for(int i = 0; i < ListSize; i++)
		if (!strcmp(List[i]->GetName(),filename))
			return 1;
	return 0;
}

vector<File*> getList()
{
	std::vector<File*> FileList;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(_T(".\\Downloads\\*"), &FindFileData);
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
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	CreateDirectory(_T(".\\Downloads"), NULL);
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			// Khởi tạo client
			CSocket client;
			vector<File*> FileList;
			vector<File*> FilesDownloaded;
			
			AfxSocketInit(NULL);
			client.Create();
			int check = client.Connect(_T("127.0.0.1"),1234);
			if (check!=0)
			{
				FileList = receiveFileList(client);
				
				printFileList(FileList);
				int index_file = 0; //số thứ tự của file
				do
				{	
					
					cout<< "Nhap so thu tu file (nhap so 0 de thoat ket noi): ";
					scanf("%d", &index_file);
					while (index_file < 0 || index_file > FileList.size())
					{
						cout<<"So nhap khong hop le. Nhap so > -1 va < "<<FileList.size() + 1<<": ";
						scanf("%d", &index_file);
					}

					//Nếu client muốn ngắt kết nối mà không download file nào
					if(index_file == 0)
					{
						if(client.Send(&index_file, sizeof(int), 0) == SOCKET_ERROR)
							ThongBaoLoiVaThoat(client, 1);
						client.Close();
					}
					else
					{
						FilesDownloaded = getList();
						File FileTemp (FileList[index_file - 1]->GetName(), FileList[index_file - 1]->GetSize());
						string new_name = "1";
						while (checkFileName(FilesDownloaded, FileTemp.GetName()))
						{
							cout<<"File "<<FileTemp.GetName()<<" da ton tai trong thu muc Download"<<endl;
							cout<<"Nhap ten khac neu muon download file nay (nhap '0' neu khong muon download): ";
							
							fflush(stdin);
							getline(cin, new_name);
							fflush(stdin);
							if (new_name!="0")
							{
								char *new_name_pointer = new char [new_name.length() + 1];
								strcpy(new_name_pointer, new_name.c_str());
								new_name_pointer[new_name.length()] = NULL;
								FileTemp.changeName(new_name_pointer);
							}

							else
								break;
						}
						if (new_name!="0")
						{
							if(client.Send(&index_file, sizeof(int), 0) == SOCKET_ERROR)
								ThongBaoLoiVaThoat(client, 1);
							Get(client, &FileTemp);
							printf("Hoan tat\n");
						}
						
					}
				}while(index_file!=0);
			}
			else
			{
				cout << "Ket noi khong thanh cong!" << endl;
				system("Pause");
			}
			client.Close();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
