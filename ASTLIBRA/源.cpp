#include "windows.h"
#include "iostream"
#include "tlhelp32.h"
#include "assert.h"
#include "conio.h"
#include "CHook.h"

#define QWORD unsigned __int64
QWORD GetGameBase(DWORD Pid)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, Pid);
	assert(hProcessSnap != INVALID_HANDLE_VALUE);
	MODULEENTRY32  me32 = { 0 };
	me32.dwSize = sizeof(MODULEENTRY32);
	QWORD qwBaseAddr = 0;
	if (Module32First(hProcessSnap, &me32))
	{
		do
		{
			if (strcmp(me32.szModule, "ASTLIBRA.exe") == 0)
			{
				qwBaseAddr = (QWORD)me32.modBaseAddr;
				std::cout << std::hex << qwBaseAddr << std::ends;
				std::cout << me32.szModule << std::endl;
			}


		} while (Module32Next(hProcessSnap, &me32));
	}
	assert(qwBaseAddr != NULL);
	CloseHandle(hProcessSnap);
	return qwBaseAddr;
}

int main()
{
	//准备工作
	HWND hGame = ::FindWindow("ASTLIBRA Revision", "ASTLIBRA Revision");
	if (!hGame)
	{
		std::cout << "ERROR:请先打开游戏!\r\n按任意键退出" << std::endl;
		_getch();
		return 0;
	}
	DWORD Pid;
	GetWindowThreadProcessId(hGame, &Pid);
	QWORD qwBaseAddr = GetGameBase(Pid);
	HANDLE hGameProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if (!hGameProcess)
	{
		std::cout << "ERROR:获取进程句柄失败,请尝试以管理员权限启动!\r\n按任意键退出" << std::endl;
		_getch();
		return 0;
	}
	//目标进程申请内存
	LPVOID addr = VirtualAllocEx(hGameProcess, NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (addr == NULL) 
	{
		printf("VirtualAllocEx failed: %d\n", GetLastError());
		return 1;
	}
	printf("VirtualAllocEx: %llX\n", addr);	

	CHook HookObj(hGameProcess, qwBaseAddr, (QWORD)addr);	
	
	std::cout << "按任意键退出" << std::endl;
	_getch();

}