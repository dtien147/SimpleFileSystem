#pragma once
class File
{
private:
	char *m_name;
	unsigned int m_size;
public:
	File(const char *Name, unsigned int size);
	~File();
	int Get(int pos, int bytes, char result[]);//Đọc 1 đoạn bytes, từ vị trí pos
	void Merge (char *S, int trueLength);//Thêm chuối S vào cuối file
	char* GetName();//lấy tên file
	int GetSize();
	void Show();
	void changeName(char *Name);
};
