#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Urlmon.lib")


extern int Update_Flag;
extern int Answer_Num;
extern int Root;

struct GUID_StreamData
{
	int ver;
	int date;
	int age;
	GUID guid;
};

struct PdbInfo
{
	DWORD	Signature;
	GUID	Guid;
	DWORD	Age;
	char	PdbFileName[1];
};

typedef struct _Pdb_Task
{
	char szDllDir[MAX_PATH];
	char szDllBaseName[MAX_PATH];
	char szSymbolServerUrl[1024];//Pdb 下载路径

	char szPdbPath[MAX_PATH];//Pdb加载路径
	char szDllFullPath[MAX_PATH];//
	HANDLE hProcess; //当前进程句柄
	DWORD64 kenhandle;//符号句柄
	HANDLE My_Drive;//驱动句柄
	DWORD64 Ntoskrnl_Base;//nt内核基址


}Pdb_Task, * PPdb_Task;

typedef struct _Command {
	int Type;//读？还是写之类的命令类型

	DWORD64 Address;//操作地址
	int width;//字节 1 ，字 2 ，双字 4 ，四字 8
	UCHAR Parameter[10][50];//提供10个50长度的空间放 命令和参数，第一个是命令，其余的是参数
	int Parameter_Num;//实用参数个数，不计第一个命令
	DWORD64 OutPut_Len;//输出长度
}Command, * PCommand;

extern Pdb_Task Pdbfile; //pdb_information

extern DWORD  error;//error_code

extern LPCSTR Ntoskrnl_Path;
extern CHAR Current_Path[MAX_PATH];

extern DWORD64 Ntoskrnl_Base_bf;//内核基址，需要通过KdDebuggerEnabled地址获得
extern PBYTE Ntoskrnl_Image;//read Ntoskrnl.exe add

extern UCHAR Cin_Command[2048];//输入的总命令

#define DEVICE_NAME L"\\\\.\\CEYDBG" //符号链接名
#define GET_BASE CTL_CODE(FILE_DEVICE_UNKNOWN,0x99f,METHOD_NEITHER,FILE_ANY_ACCESS)//初始化，通过偏移计算内核基址
#define GET_VALUE CTL_CODE(FILE_DEVICE_UNKNOWN,0x99e,METHOD_NEITHER,FILE_ANY_ACCESS)//读0x80字节数据





void Error_Out(int E_C);
int Read_Memory(PPdb_Task Pdbfile, PCommand Task,int width);
BOOL CALLBACK EnumSymProc(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext);
int CheckPdb_X(PCSTR Pdbname);

void Load_Drive();
BOOL Install_Drive();
BOOL Start_Drive();
BOOL Stop_Drive();
BOOL Unload_Drive();

void Read_Ntoskrnl_Image();
DWORD Get_Ntoskrnl_Pdb();
void Load_Pdb();

void Ascii_To_Unicode(IN char* a, IN OUT WCHAR* u);