#pragma once
#include <ntddk.h>


#define my_device_name  L"\\Device\\CEYDBG"//设备名
#define my_link_name L"\\??\\CEYDBG"//符号链接名

#define GET_BASE CTL_CODE(FILE_DEVICE_UNKNOWN,0x99f,METHOD_NEITHER,FILE_ANY_ACCESS)//
#define GET_VALUE CTL_CODE(FILE_DEVICE_UNKNOWN,0x99e,METHOD_NEITHER,FILE_ANY_ACCESS)//读0x80字节数据
