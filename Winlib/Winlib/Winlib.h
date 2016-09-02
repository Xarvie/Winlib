#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <iostream>


using namespace std;

///<summary>�ü��а��ı���</summary><param name = "strText">���ַ���ָ�롣</param>
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

///<summary>ȡ���а��ı����ɹ�:�����ı���ʧ��:NULL��</summary><returns>�ɹ�:�����ı���ʧ��:NULL��</returns>
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

///<summary>�����¼���</summary>
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

///<summary>�߾��ȵȴ���</summary><param name = "milliSecond">�ȴ��ĺ�������</param><returns>�ɹ�:0��ʧ��:-1��</returns>
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

///<summary>�ļ��ƶ���</summary><param name = "oldName">��λ�á�</param><param name = "newName">��λ��B��</param><returns>�ɹ�:0��ʧ��:-1��</returns>
int FileMove(const char *oldName, const char* newName)
{
	return rename(oldName, newName);
}

///<summary>����2��ϼ���</summary><param name = "keyA">����A��</param><param name = "keyB">����B��</param>
void keyClick(BYTE keyA, BYTE keyB)
{
	keybd_event(keyA, 0, 0, 0);
	keybd_event(keyB, 0, 0, 0);
	keybd_event(keyB, 0, 0x2, 0);
	keybd_event(keyA, 0, 0x2, 0);
}

///<summary>����3��ϼ���</summary><param name = "keyA">����A��</param><param name = "keyB">����B��</param><param name = "keyC">����C��</param>
void keyClick(BYTE keyA, BYTE keyB, BYTE keyC)
{
	keybd_event(keyA, 0, 0, 0);
	keybd_event(keyB, 0, 0, 0);
	keybd_event(keyC, 0, 0, 0);
	keybd_event(keyC, 0, 0x2, 0);
	keybd_event(keyB, 0, 0x2, 0);
	keybd_event(keyA, 0, 0x2, 0);
}

///<summary>ȡ��������Ŀ¼���ɹ�:�����ı���ʧ��:NULL��</summary><returns>�ɹ�:�����ı���ʧ��:NULL��</returns>
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