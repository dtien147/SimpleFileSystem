// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include "File.h"
#include <thread>
#include "Support.h"
SOCKET hConnected;
int numClientDownloading = 0;
void Process(vector<File*> &FileList)
{
	CSocket Connector;
	Connector.Attach(hConnected);
	cout << "Client " << std::this_thread::get_id() << " da ket noi\n";
	int index_file = 0;
	SendFileList(Connector, FileList);			//Gửi danh sách file cho Client
	do
	{
		if(Connector.Receive(&index_file, sizeof(int), 0) == SOCKET_ERROR) //Nhận yêu cầu từ Client
			ThongBaoLoiVaThoat(Connector);				
		if(index_file!=0)
		{
			cout << "Client " << std::this_thread::get_id() << " tai ve file " << FileList[index_file-1]->GetName() << " \n";
			Provide(Connector, FileList[index_file-1]);
		}
	}while (index_file!=0);
	cout << "Client " << std::this_thread::get_id() << " da ngat ket noi\n";
	Connector.Close();
}
CWinApp theApp;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	
	//Tạo thư mục Shared Folder nếu chưa có
	CreateDirectory(_T(".\\Shared Folder"), NULL);
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
			if(AfxSocketInit() != 0)
			{
				CSocket Server;
				if(Server.Create(1234, 1, NULL) != 0)
				{
					vector<File*> FileList;
					int index_file = 0;
					cout << "Khoi tao server thanh cong\n";
					FileList = ReadFileList();		//Đọc danh sách file trong thư mục hiện tại
					Server.Listen(1);
					vector<thread*> threadList;
					do 
					{
						CSocket Connector;
						if(Server.Accept(Connector) == TRUE)
							if(numClientDownloading == 0)
							{
								hConnected = Connector.Detach();
								threadList.push_back(new thread(Process, ref(FileList)));
							}
							else
								Connector.Close();
					} while (1);
					Server.Close();
				}
				else
					cout << "Khoi tao server that bai\n";
			}
			else
				cout << "Init socket failed" << endl;
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
