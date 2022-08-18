#include"Ceydbg.h"

Pdb_Task Pdbfile = { 0 }; //pdb_information

LPCSTR Ntoskrnl_Path = "C:\\Windows\\System32\\ntoskrnl.exe";

PBYTE Ntoskrnl_Image = { 0 };//read Ntoskrnl.exe add




//road Ntoskrnl.exe
void Read_Ntoskrnl_Image() {
	HANDLE Ntoskrnl_H = CreateFileA(Ntoskrnl_Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);//载入文件
	if (Ntoskrnl_H == INVALID_HANDLE_VALUE)//是否成功载入文件
		Error_Out(2);
	char rdos[sizeof(IMAGE_DOS_HEADER)];//临时dos指针空间
	char rpe[sizeof(IMAGE_NT_HEADERS)];//临时NT空间
	DWORD NumberOfBytesRW = 0;
	if (ReadFile(Ntoskrnl_H, rdos, sizeof(IMAGE_DOS_HEADER), &NumberOfBytesRW, NULL) == FALSE)
		Error_Out(2);
	PIMAGE_DOS_HEADER rdoshead = (PIMAGE_DOS_HEADER)rdos;//DOS头指针（临时）
	SetFilePointer(Ntoskrnl_H, rdoshead->e_lfanew, NULL, FILE_BEGIN);//下一次读PE头
	if (ReadFile(Ntoskrnl_H, rpe, sizeof(IMAGE_NT_HEADERS), &NumberOfBytesRW, NULL) == FALSE)
		Error_Out(2);
	PIMAGE_NT_HEADERS rpehead = (PIMAGE_NT_HEADERS)rpe;
	Ntoskrnl_Image = (PBYTE)malloc(rpehead->OptionalHeader.SizeOfImage);
	SetFilePointer(Ntoskrnl_H, 0, NULL, FILE_BEGIN);//下一次读PE头
	if (ReadFile(Ntoskrnl_H, Ntoskrnl_Image, rpehead->OptionalHeader.SizeOfHeaders, &NumberOfBytesRW, NULL) == FALSE)
		Error_Out(2);
	rdoshead = (PIMAGE_DOS_HEADER)Ntoskrnl_Image;
	rpehead = (PIMAGE_NT_HEADERS)(rdoshead->e_lfanew + Ntoskrnl_Image);


	IMAGE_SECTION_HEADER* pSectionHeader = IMAGE_FIRST_SECTION(rpehead);//取区段信息
	for (int i = 0; i < rpehead->FileHeader.NumberOfSections; i++, pSectionHeader++)//读区段数据,模仿镜像加载
	{
		SetFilePointer(Ntoskrnl_H, pSectionHeader->PointerToRawData, NULL, FILE_BEGIN);//设置下次读取地址
		//ALIGN(sectionsize, sectionalign);//对齐读取大小
		if (ReadFile(Ntoskrnl_H, Ntoskrnl_Image + pSectionHeader->VirtualAddress, pSectionHeader->SizeOfRawData, &NumberOfBytesRW, NULL) == FALSE)
			Error_Out(2);
	}
}

//get pdb in Current Directory
DWORD Get_Ntoskrnl_Pdb()
{
	PPdb_Task Pdb = &Pdbfile;

	GetCurrentDirectoryA(MAX_PATH, Current_Path);
	strcpy(Pdb->szDllBaseName, "ntoskrnl.exe");
	//strcpy(Pdb->szDllDir, szDllFullPath);
	//Pdb->szDllDir[GetBaseName(szDllFullPath) - szDllFullPath] = NULL;

	strcpy(Pdb->szSymbolServerUrl, "https://msdl.microsoft.com/download/symbols/");//设置pdb服务器

	strcpy(Pdb->szDllFullPath, Ntoskrnl_Path);//ntoskrnl.exe路径

	strcpy(Pdb->szPdbPath, Current_Path);//get pdb path

	if (Pdb->szPdbPath[strlen(Pdb->szPdbPath) - 1] != '\\')
	{
		strcat(Pdb->szPdbPath, "\\");
	}


	// get pdb info from debug info directory
	//PBYTE FileBuffer = NULL;
	Read_Ntoskrnl_Image();//读文件到内存
	IMAGE_DOS_HEADER* pDos = (IMAGE_DOS_HEADER*)Ntoskrnl_Image;
	IMAGE_NT_HEADERS* pNT = (IMAGE_NT_HEADERS*)(Ntoskrnl_Image + pDos->e_lfanew);
	IMAGE_FILE_HEADER* pFile = &pNT->FileHeader;
	IMAGE_OPTIONAL_HEADER64* pOpt64 = (IMAGE_OPTIONAL_HEADER64*)(&pNT->OptionalHeader);

	IMAGE_DATA_DIRECTORY* pDataDir = nullptr;
	pDataDir = &pOpt64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	IMAGE_DEBUG_DIRECTORY* pDebugDir = (IMAGE_DEBUG_DIRECTORY*)(Ntoskrnl_Image + pDataDir->VirtualAddress);
	if (!pDataDir->Size || IMAGE_DEBUG_TYPE_CODEVIEW != pDebugDir->Type)
	{
		// invalid debug dir
		free(Ntoskrnl_Image);
		Error_Out(2);
	}
	PdbInfo* pdb_info = (PdbInfo*)(Ntoskrnl_Image + pDebugDir->AddressOfRawData);
	if (pdb_info->Signature != 0x53445352)
	{
		// invalid debug dir
		free(Ntoskrnl_Image);
		Error_Out(2);
	}

	// sometimes pdb_info->PdbFileName is a abs path, sometimes is just a base name.
	// In first case, we have to calc its base name.
	strcat(Pdb->szPdbPath, pdb_info->PdbFileName);//get pdb name

	// download pdb

	//判断文件是否存在
	WIN32_FIND_DATAA wfd;
	HANDLE hFind = FindFirstFileA(Pdb->szPdbPath, &wfd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		//已经有pdb文件了
		if (Update_Flag != 1)//系统更新了？要重新下载pdb，置Update_Flag==1
		{

			free(Ntoskrnl_Image);
			FindClose(hFind);//不需要重新下载
			return 0;
		}
	}

	wchar_t w_GUID[100] = { 0 };
	if (!StringFromGUID2(pdb_info->Guid, w_GUID, 100))
	{
		free(Ntoskrnl_Image);
		Error_Out(2);
	}
	char a_GUID[100]{ 0 };//get debug_guid
	size_t l_GUID = 0;
	if (wcstombs_s(&l_GUID, a_GUID, w_GUID, sizeof(a_GUID)) || !l_GUID)
	{
		free(Ntoskrnl_Image);
		Error_Out(2);
	}

	char guid_filtered[256] = { 0 };
	for (UINT i = 0; i != l_GUID; ++i)
	{
		if ((a_GUID[i] >= '0' && a_GUID[i] <= '9') || (a_GUID[i] >= 'A' && a_GUID[i] <= 'F') || (a_GUID[i] >= 'a' && a_GUID[i] <= 'f'))
		{
			guid_filtered[strlen(guid_filtered)] = a_GUID[i];
		}
	}

	char age[3] = { 0 };
	_itoa_s(pdb_info->Age, age, 10);

	// url
	char url[1024] = { 0 };
	strcpy(url, Pdb->szSymbolServerUrl);
	strcat(url, pdb_info->PdbFileName);
	url[strlen(url)] = '/';
	strcat(url, guid_filtered);
	strcat(url, age);
	url[strlen(url)] = '/';
	strcat(url, pdb_info->PdbFileName);

	// download
	HRESULT hr = URLDownloadToFileA(NULL, url, Pdb->szPdbPath, NULL, NULL);
	if (FAILED(hr))
	{
		free(Ntoskrnl_Image);
		Error_Out(2);
	}

	free(Ntoskrnl_Image);
	return 0;
}

//load pdb file and exe
void Load_Pdb() {


	Pdbfile.hProcess = OpenProcess(FILE_ALL_ACCESS, 0, GetCurrentProcessId());

	SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
	if (!SymInitialize(Pdbfile.hProcess, Pdbfile.szPdbPath, TRUE))
	{
		// SymInitialize failed
		error = GetLastError();
		printf("SymInitialize returned error : %d\n", error);
		system("pause");
		Error_Out(2);
	}
	//TCHAR  szImageName[MAX_PATH] = TEXT("foo.dll");
	DWORD64 dwBaseAddr = 0;

	Pdbfile.kenhandle = SymLoadModuleEx(Pdbfile.hProcess,    // target process 
		NULL,        // handle to image - not used
		Ntoskrnl_Path, // name of image file
		NULL,        // name of module - not required
		dwBaseAddr,  // base address - not required
		0,           // size of image - not required
		NULL,        // MODLOAD_DATA used for special cases 
		0);        // flags - not required

	if (Pdbfile.kenhandle == NULL)
	{
		error = GetLastError();
		printf("SymLoadModuleEx error : %d\n", error);
		system("pause");
		Error_Out(2);
	}
}
