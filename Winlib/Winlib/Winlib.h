#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <iostream>

using namespace std;

string u2a(wstring u)
{
	int iSize;
	string rt;
	iSize = WideCharToMultiByte(CP_ACP, 0, u.c_str(), -1, NULL, 0, NULL, NULL);
	rt.resize(iSize + 1, 0);
	WideCharToMultiByte(CP_ACP, 0, u.c_str(), -1, &rt[0], iSize, NULL, NULL);
	return rt;
}

wstring a2u(string a)
{
	int iSize;
	wstring rt;
	iSize = MultiByteToWideChar(CP_ACP, 0, a.c_str(), -1, NULL, 0);
	rt.resize(iSize + 1, 0);
	MultiByteToWideChar(CP_ACP, 0, a.c_str(), -1, &rt[0], iSize);
	return rt;
}

///<summary>置剪切板文本。</summary><param name = "strText">宽字符串指针。</param>
void setClipText(wstring strText)
{
	if (OpenClipboard(NULL))
	{
		HGLOBAL hmem = GlobalAlloc(GHND, (strText.length() + 1) * sizeof(strText[0]));
		WCHAR *pmem = (WCHAR*)GlobalLock(hmem);
		EmptyClipboard();
		memcpy(pmem, &strText[0], (strText.length() + 1) * sizeof(strText[0]));
		SetClipboardData(CF_UNICODETEXT, hmem);
		CloseClipboard();
		GlobalFree(hmem);
	}
}

///<summary>取剪切板文本。成功:返回文本，失败:NULL。</summary><returns>成功:返回文本，失败:NULL。</returns>
wstring getClipText()
{
	wstring rt;
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		return rt;
	}
	if (!OpenClipboard(NULL))
	{
		return rt;
	}
	HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
	if (hMem)
	{
		WCHAR* str = (WCHAR*)GlobalLock(hMem);
		if (str)
		{
			rt = str;
			GlobalUnlock(hMem);
		}
	}
	CloseClipboard();
	return rt;
}

///<summary>处理事件。</summary>
void doEvents()
{
	MSG msg;
	while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
		{
			return;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

///<summary>高精度等待。</summary><param name = "milliSecond">等待的毫秒数。</param><returns>成功:0，失败:-1。</returns>
int wait(UINT64 milliSecond)
{
	HANDLE hTimer;
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -10 * milliSecond * 1000LL;
	doEvents();
	hTimer = CreateWaitableTimer(nullptr, FALSE, nullptr);
	if (NULL == hTimer)
	{
		return  -1;
	}
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
	{
		return -1;
	}
	DWORD ret;
	while (ret = MsgWaitForMultipleObjects(1, &hTimer, FALSE, INFINITE, QS_ALLINPUT) != WAIT_OBJECT_0)
	{
		if (((WAIT_OBJECT_0 + 1) == ret) || (WAIT_TIMEOUT == ret))
		{
			doEvents();
		}
		else
		{
			break;
		}
	}
	CancelWaitableTimer(hTimer);
	CloseHandle(hTimer);
	doEvents();
	return 0;
}

///<summary>键盘2组合键。</summary><param name = "keyA">按键A。</param><param name = "keyB">按键B。</param>
void keyClick(BYTE keyA, BYTE keyB)
{
	keybd_event(keyB, 0, 0, 0);
	keybd_event(keyA, 0, 0, 0);
	keybd_event(keyB, 0, 0x2, 0);
	keybd_event(keyA, 0, 0x2, 0);

}

///<summary>键盘3组合键。</summary><param name = "keyA">按键A。</param><param name = "keyB">按键B。</param><param name = "keyC">按键C。</param>
void keyClick(BYTE keyA, BYTE keyB, BYTE keyC)
{
	keybd_event(keyA, 0, 0, 0);
	keybd_event(keyB, 0, 0, 0);
	keybd_event(keyC, 0, 0, 0);
	keybd_event(keyC, 0, 0x2, 0);
	keybd_event(keyB, 0, 0x2, 0);
	keybd_event(keyA, 0, 0x2, 0);
}

///<summary>取程序路径。成功:路径文本，失败:空文本。</summary>
wstring getProgramPath()
{
	wstring rt(MAX_PATH, 0);
	if (GetModuleFileName(NULL, &rt[0], MAX_PATH * sizeof(WCHAR)))
	{
		return rt;
	}
	else
	{
		rt[0] = 0;
		return rt;
	}
}

///<summary>寻找文件或目录。成功:文件名 失败:空文本。</summary><param name = "path">匹配条件。例1：“./*.txt ”</param>
wstring findFile(wstring path)
{
	wstring rt;
	WIN32_FIND_DATA WFD;
	HANDLE hFind = FindFirstFile(path.c_str(), &WFD);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return rt;
	}
	rt = WFD.cFileName;
	::FindClose(hFind);
	return rt;
}

///<summary>激活指定窗口。成功:激活的句柄 失败:NULL。</summary><param name = "path">匹配条件。例1：“./*.txt ”</param>
HWND ActiveWindow(HWND hWnd)
{
	DWORD idCurrent = GetCurrentThreadId();
	DWORD idTarget = GetWindowThreadProcessId(hWnd, 0);
	AttachThreadInput(idTarget, idCurrent, true);
	HWND rt = SetActiveWindow(hWnd);
	AttachThreadInput(idTarget, idCurrent, false);
	return rt;
}


BOOL focusWindow(HWND hWnd)
{
	return SetForegroundWindow(hWnd);
}

class timer
{
public:
	HANDLE hTimerQueue=0;
	HANDLE hTimer=0;
	WAITORTIMERCALLBACK func;
	int timerCycle;

	int CreateTimer(WAITORTIMERCALLBACK func, int timerCycle=1000)
	{
		if (hTimerQueue != 0 || hTimer != 0)
		{
			deleteTimer();
		}
		this->func = func;
		this->hTimerQueue= CreateTimerQueue();
		return(CreateTimerQueueTimer(&this->hTimer, hTimerQueue, func, 0, 0, timerCycle, WT_EXECUTEDEFAULT));
	}
	int deleteTimer()
	{
		if (hTimerQueue == 0 || hTimer == 0)
		{
			return 1;
		}
		BOOL bol = DeleteTimerQueueTimer(hTimerQueue, hTimer, 0);
		DeleteTimerQueue(hTimerQueue);
		hTimerQueue = 0;
		hTimer = 0;
		timerCycle=0;
		return bol;
	}

};

void mouseClick(HWND hWnd, int x, int y)
{
	SendMessage(hWnd, WM_LBUTTONDOWN, 1, x + y * 65536);
	SendMessage(hWnd, WM_LBUTTONUP, 0, x + y * 65536);
}
void mouseClick2(int x, int y)
{
	wait(10);
	SetCursorPos(x, y);
	wait(100);
	mouse_event(2, 0, 0, 0, 0);
	mouse_event(4, 0, 0, 0, 0);
}
LONGLONG getFileSize(wstring fileName)
{
	/*
	LARGE_INTEGER FileSize;
	HFILE hFile=_lopen(u2a(fileName).c_str(), 0);
	GetFileSizeEx((HANDLE)hFile, &FileSize);
	_lclose(hFile);
	return FileSize.QuadPart;
	*/
	ULARGE_INTEGER s;
	s.LowPart=GetCompressedFileSize(fileName.c_str(), &s.HighPart);
	return s.HighPart * 4294967296 + s.LowPart;
}
