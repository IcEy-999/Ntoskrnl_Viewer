# ReadMe

## 本项目含三个模块 The project includes three modules：

1.驱动模块 Drive module

2.用户模块 User module

3.成品模块（驱动已签名）Finished module (driver signed)



## 功能 function：

可在不开启测试模式和WinDbg本地调试的情况下，以符号化的形式浏览本地内存！

You can browse local memory in symbolic form without turning on test mode and WinDbg local debugging!

不管是否为导出函数、未导出函数、系统内核变量、标志，只要在PDB文件中的符号，都能浏览其内存。

No matter whether it is an exported function, an unexported function, a system kernel variable, or a flag, as long as the symbol in the pdb file is displayed, its memory can be browsed.

只支持X64系统，理论上支持任何版本（只要微软公开了此版本的PDB）

It only supports x64 system, and theoretically supports any version (as long as Microsoft discloses this version of PDB)

###### Windows 10:

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/win10_test.png)

###### Windows 11:

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/win11_test.png)



## 目前已实现的命令 Currently implemented commands：

### “db” "dw" "dd" "dq"，这四种功能同WinDbg，例如 

### "db" "dw" "dd" "dq", these four functions are the same as WinDbg, for example:：

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/d_test.png)

### “d”也可以通过地址读取内存：（读取非法地址时会报错）


### (“D ”you can also read the memory through the address: (an error will be reported when reading the illegal address)



![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/d_test2.png)





### “x” 同WinDbg Same as WinDbg：

![image](https://github.com/IcEy-999/IC_Ntoskrnl_Viewer/blob/main/Release/x_test.png)





### 退出需要通过命令"quit"。

### Exit requires the command "quit".

不然驱动无法正常卸载！

Otherwise, the driver cannot be uninstalled normally!





## 此次更新 This update：

###### 由于有PG、读写权限的原因，测试版先不提供写功能。

###### (Due to PG and read / write permission, the beta does not provide write function.)

###### 测试版 只支持X64系统，理论上支持任何版本（只要微软公开了此版本的PDB）

###### (The beta version only supports x64 system, and theoretically supports any version (as long as Microsoft discloses this version of PDB))





以后可能会更新 “eb、ew、ed、eq”命令。

The commands "EB, EW, ED, EQ" may be updated later.


会逐渐向 windbg本地调试可实现的功能靠近！

It will gradually approach the functions that WinDbg local debugging can achieve!
