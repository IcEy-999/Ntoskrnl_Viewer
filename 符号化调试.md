# ReadMe

## 本项目含三个模块：

1.驱动模块

2.用户模块

3.成品模块（驱动已签名）



## 功能：

可在不开启测试模式和WinDbg本地调试的情况下，以符号化的形式浏览本地内存！

不管是否为导出函数、未导出函数、系统内核变量、标志，只要在PDB文件中的符号，都能浏览其内存。

只支持X64系统，理论上支持任何版本（只要微软公开了此版本的PDB）

###### Windows 10:

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/win10_test.png)

###### Windows 11:

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/win11_test.png)



## 目前已实现的命令：

### “db” "dw" "dd" "dq"，这四种功能同WinDbg，例如：

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/d_test.png)

### “x” 同WinDbg：

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/x_test.png)



## 注意：

### ”d“默认只能通过符号读取内存，通过地址读取内存需要开启”ROOT“权限：例如：

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/d_test2.png)

原因：可能会读写权限不足，发生蓝屏。



### 退出需要通过命令"quit"。

不然驱动无法正常卸载！







## 此次更新：

###### 由于有PG、读写权限的原因，测试版先不提供写功能。

###### 测试版 只支持X64系统，理论上支持任何版本（只要微软公开了此版本的PDB）

以后可能会更新 “eb、ew、ed、eq”命令。

和”a“命令

会逐渐向 windbg本地调试可实现的功能靠近！

