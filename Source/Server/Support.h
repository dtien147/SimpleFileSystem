#define BYTE 1000
#include "File.h"
extern int numClientDownloading;		//Số client đang download
vector<File*> ReadFileList();
void DeleteFileList(vector<File*> FileList);
void SendFile(CSocket &Connector, File* file);
void SendFileList(CSocket &Connector, const vector<File*> &FileList);
void Provide(CSocket &Connector, File* file);
void ThongBaoLoiVaThoat(CSocket &Connector);