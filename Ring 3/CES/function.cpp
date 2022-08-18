#include "Ceydbg.h"
//E_C:
//0:About Drive install、unload、start、stop
//1:About Drive Symbolic link
//2:About PDB Load
void Error_Out(int E_C) {
	printf("error : %d\n",E_C);
	system("pause");
	exit(0);
}

void Ascii_To_Unicode(IN char *a,IN OUT WCHAR *u) {
	int l = strlen(a);
	for (int i = 0; i < l; i++)
		memcpy(u + i, a + i, 1);
}

int Ascii_To_LONG_LONG(IN char* a, IN OUT PDWORD64 add)
{
	int len = strlen(a);
	if (len > 16)
		return 0;
	PBYTE offset = (PBYTE)add;
	int flag = 0;
	//字符串转8字节地址
	for (int i = 0; i < len; i++)
	{
		if (a[i] >= 0x30 && a[i] <= 0x39)
			a[i] -= 0x30;
		else if (a[i] == 'a' || a[i] == 'A')
			a[i] = 0xa;
		else if (a[i] == 'b' || a[i] == 'B')
			a[i] = 0xb;
		else if (a[i] == 'c' || a[i] == 'C')
			a[i] = 0xc;
		else if (a[i] == 'd' || a[i] == 'D')
			a[i] = 0xd;
		else if (a[i] == 'e' || a[i] == 'E')
			a[i] = 0xe;
		else if (a[i] == 'f' || a[i] == 'F')
			a[i] = 0xf;
		else
			return 0;
	}
	for (int i = 0; i < len; i++)
	{
		if (flag == 0)
		{
			memcpy(offset, &(a[i]), 1);
			flag = 1;
		}
		else
		{
			*add = *add << 4;
			*(offset) += a[i];
		}
	}
	return 1;


}



int Complete_Command(PPdb_Task Pdbfile,PCommand Task) {
	DWORD64 Default_Len = 0x80;
	
	if (Task->OutPut_Len != 0)
		Default_Len = Task->OutPut_Len * Task->width;
	
	
	PBYTE buffer = (PBYTE)malloc(Default_Len);
	DWORD lent = 0;
	if (!DeviceIoControl(Pdbfile->My_Drive, GET_VALUE, buffer, Default_Len, &(Task->Address), 16, &lent, NULL))
		return 0;

	
	switch (Task->width) {//宽度输出
	case 1: {

		for (int i = 0; i < Default_Len; i++)
		{
			if (i % 0x10 == 0)
			{
				printf(" \n %p   ", Task->Address);
				Task->Address += 0x10;
			}
			printf(" %02x", buffer[i]);
		}
		printf("\n");
		break;
	}
	case 2: {
		for (int i = 0; i < Default_Len; i = i + 2)
		{
			if (i % 0x10 == 0)
			{
				printf(" \n %p   ", Task->Address);
				Task->Address += 0x10;
			}
			USHORT out = *(USHORT*)(buffer + i);
			printf(" %04x", out);
		}
		printf("\n");
		break;
	}
	case 4: {
		for (int i = 0; i < Default_Len; i = i + 4)
		{
			if (i % 0x10 == 0)
			{
				printf(" \n %p   ", Task->Address);
				Task->Address += 0x10;
			}
			ULONG out = *(ULONG*)(buffer + i);
			printf(" %08x", out);
		}
		printf("\n");
		break;
	}
	case 8: {
		for (int i = 0; i < Default_Len; i = i + 8)
		{
			if (i % 0x10 == 0)
			{
				printf(" \n %p   ", Task->Address);
				Task->Address += 0x10;
			}
			DWORD64 out = *(DWORD64 *)(buffer + i);
			printf(" %16p", out);
		}
		printf("\n");
		break;
	}
	}

	free(buffer);
	return 1;

}

int Read_Memory(PPdb_Task Pdbfile, PCommand Task, int width) {
	if (Task->Parameter_Num > 2)
		return 0;
	DWORD64 Out_Len = 0;
	if (Task->Parameter[2][0] == 'l')
	{

		if (!Ascii_To_LONG_LONG((char*)&(Task->Parameter[2][1]), &Out_Len))
			return 0;
		if (Out_Len > 0x100)
		{
			printf("\n too long!\n");
			return 0;
		}
		Task->OutPut_Len = Out_Len;
	}
	else if (Task->Parameter[2][0] != 0)
		return 0;


	int Pdb_Or_Add = 0;//0：地址 1：符号
	Task->width = width;
	int len = strlen((char*)&(Task->Parameter[1]));
	//判断是 符号 还是 地址
	for (int i = 0; i < len; i++)
	{
		if (Task->Parameter[1][i] == 'a' || Task->Parameter[1][i] == 'b' || Task->Parameter[1][i] == 'c' || Task->Parameter[1][i] == 'd' || Task->Parameter[1][i] == 'e' || Task->Parameter[1][i] == 'f' || Task->Parameter[1][i] == 'A' || Task->Parameter[1][i] == 'B' || Task->Parameter[1][i] == 'C' || Task->Parameter[1][i] == 'D' || Task->Parameter[1][i] == 'E' || Task->Parameter[1][i] == 'F')
			continue;
		if (Task->Parameter[1][i] < '0' || Task->Parameter[1][i] >'9')
		{
			Pdb_Or_Add = 1;
			break;
		}
	}

	if (Pdb_Or_Add == 1)
	{
		//查询的是符号
		for (int i = 0; i < len; i++)
			if (Task->Parameter[1][i] == '*') //不接受 多个查询
				return 0;
		SYMBOL_INFOW Information = { 0 };
		WCHAR un[256] = { 0 };
		Ascii_To_Unicode((char*)&(Task->Parameter[1]), un);
		SymFromNameW(Pdbfile->hProcess, un, &Information);
		if (Information.Address == 0)
			return 0;
		Task->Address = Information.Address + Pdbfile->Ntoskrnl_Base;
		return Complete_Command(Pdbfile, Task);
	}
	else
	{

		if (Root == 0)//通过地址都内存需要root！！
			return 0;
		//输入的是地址
		DWORD64 add = 0;

		if (!Ascii_To_LONG_LONG((char*)(Task->Parameter[1]), &add))
			return 0;

		Task->Address = add;
		return Complete_Command(Pdbfile, Task);

	}



}

BOOL CALLBACK EnumSymProc(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
	UNREFERENCED_PARAMETER(UserContext);
	/*printf("%S : %P\n",pSymInfo->Name,pSymInfo->Address+ Ntoskrnl_Base);*/
	if (strcmp(pSymInfo->Name, "KdDebuggerEnabled") == 0 && Pdbfile.Ntoskrnl_Base == 0)
	{
		Pdbfile.Ntoskrnl_Base = Ntoskrnl_Base_bf - pSymInfo->Address;
		Answer_Num++;
		return TRUE;
	}

	printf(" %p  %s \n", pSymInfo->Address + Pdbfile.Ntoskrnl_Base, pSymInfo->Name);
	return TRUE;
}

int CheckPdb_X(PCSTR Pdbname) {
	if (!SymEnumSymbols(Pdbfile.hProcess,     // Process handle from SymInitialize.
		Pdbfile.kenhandle,   // Base address of module.
		Pdbname,        // Name of symbols to match.
		EnumSymProc, // Symbol handler procedure.
		NULL)) // User context.
	{
		return 0;
	}
	return 1;
}


