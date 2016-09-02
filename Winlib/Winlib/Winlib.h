#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <iostream>


using namespace std;

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
	wstring rt = NULL;
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
int sleepEx(UINT64 milliSecond)
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

///<summary>文件移动。</summary><param name = "oldName">旧位置。</param><param name = "newName">新位置B。</param><returns>成功:0，失败:-1。</returns>
int FileMove(const char *oldName, const char* newName)
{
	return rename(oldName, newName);
}

///<summary>键盘2组合键。</summary><param name = "keyA">按键A。</param><param name = "keyB">按键B。</param>
void keyClick(BYTE keyA, BYTE keyB)
{
	keybd_event(keyA, 0, 0, 0);
	keybd_event(keyB, 0, 0, 0);
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

///<summary>取程序所在目录。成功:返回文本，失败:NULL。</summary><returns>成功:返回文本，失败:NULL。</returns>
wstring getProgramDir()
{
	wstring rt(MAX_PATH, 0);

	if (GetModuleFileName(NULL, &rt[0], MAX_PATH * sizeof(WCHAR)))
	{
		return rt;
	}
	else
	{
		return NULL;
	}
}