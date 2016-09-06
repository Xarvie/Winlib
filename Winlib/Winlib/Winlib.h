#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;

//������ encode
namespace enc
{
	///<summary>UnicodeתASCII��</summary><param name = "Unicode">Unicode�ı���</param><returns>�ɹ�:�����ı���ʧ��:string(����)��</returns>
	string u2a(wstring Unicode)
	{
		int iSize;
		string rt;
		iSize = WideCharToMultiByte(CP_ACP, 0, Unicode.c_str(), -1, NULL, 0, NULL, NULL);
		rt.resize(iSize, 0);
		if (!WideCharToMultiByte(CP_ACP, 0, Unicode.c_str(), -1, &rt[0], iSize, NULL, NULL))
		{
			return string("");
		}
		return rt;
	}

	wstring a2u(string a)
	{
		int iSize; 
		wstring rt;
		iSize = MultiByteToWideChar(CP_ACP, 0, a.c_str(), -1, NULL, 0);
		rt.resize(iSize, 0);
		if(!MultiByteToWideChar(CP_ACP, 0, a.c_str(), -1, &rt[0], iSize))
		{
			return wstring(L"");
		}
		return rt;
	}
}

//���а�  Clipboard
namespace cli
{
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
}
namespace tim
{
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
	class timer
	{
	public:
		HANDLE hTimerQueue = 0;
		HANDLE hTimer = 0;
		WAITORTIMERCALLBACK func;
		int timerCycle;

		int CreateTimer(WAITORTIMERCALLBACK func, int timerCycle = 1000)
		{
			if (hTimerQueue != 0 || hTimer != 0)
			{
				deleteTimer();
			}
			this->func = func;
			this->hTimerQueue = CreateTimerQueue();
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
			timerCycle = 0;
			return bol;
		}
	};
}
namespace key
{
	///<summary>����2��ϼ���</summary><param name = "keyA">����A��</param><param name = "keyB">����B��</param>
	void keyClick(BYTE keyA, BYTE keyB)
	{
		keybd_event(keyB, 0, 0, 0);
		keybd_event(keyA, 0, 0, 0);
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
}
namespace fil
{
	///<summary>ȡ����·�����ɹ�:·���ı���ʧ��:���ı���</summary>
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
	///<summary>ȡ�ļ��ߴ硣�ɹ�:�ߴ� ʧ��:-1��</summary><param name = "fileName">�ļ���</param>
	LONGLONG getFileSize(wstring fileName)
	{
		LARGE_INTEGER FileSize;
		HFILE hFile = _lopen(enc::u2a(fileName).c_str(), 0);
		if (!GetFileSizeEx((HANDLE)hFile, &FileSize))
		{
			return -1;
		}
		_lclose(hFile);
		return FileSize.QuadPart;
	}
	///<summary>ȡ�ļ�ѹ��(ʵ��)�ߴ硣�ɹ�:�ߴ硣</summary><param name = "fileName">�ļ���</param>
	LONGLONG getFileCompressedSize(wstring fileName)
	{
		ULARGE_INTEGER s;
		s.LowPart=GetCompressedFileSize(fileName.c_str(), &s.HighPart);
		return s.HighPart * 4294967296 + s.LowPart;
		/*
		ȡʵ����ѹ������ռ�ô��̿ռ�,����������ͷ�ѹ��,ʵ��Ч������GetFileSize,
		MSDN:
		If the file is not located on a volume that supports compression or sparse files,
		or if the file is not compressed or a sparse file, the value obtained is the actual file size,
		the same as the value returned by a call to GetFileSize.
		*/
	}
	///<summary>Ѱ���ļ���Ŀ¼���ɹ�:�ļ��� ʧ��:���ı���</summary><param name = "filterText">ƥ����������1����./*.txt ��</param>
	wstring findFile(wstring filterText)
	{
		wstring rt;
		WIN32_FIND_DATA WFD;
		HANDLE hFind = FindFirstFile(filterText.c_str(), &WFD);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return rt;
		}
		rt = WFD.cFileName;
		::FindClose(hFind);
		return rt;
	}
}
namespace win
{
	///<summary>����ָ�����ڡ��ɹ�:����ľ�� ʧ��:NULL��</summary><param name = "hWnd">���ھ��</param>
	HWND ActiveWindow(HWND hWnd)
	{
		DWORD idCurrent = GetCurrentThreadId();
		DWORD idTarget = GetWindowThreadProcessId(hWnd, 0);
		AttachThreadInput(idTarget, idCurrent, true);
		HWND rt = SetActiveWindow(hWnd);
		AttachThreadInput(idTarget, idCurrent, false);
		return rt;
	}
	///<summary>����ǰ�á��ɹ�:����ľ�� ʧ��:NULL��</summary><param name = "hWnd">���ھ��</param>
	BOOL focusWindow(HWND hWnd)
	{
		return SetForegroundWindow(hWnd);
	}
}
namespace mou
{
	///<summary>��굥����</summary><param name = "hWnd">���ھ��</param><param name = "x">X</param><param name = "y">Y</param>
	void mouseClick(HWND hWnd, int x, int y)
	{
		SendMessage(hWnd, WM_LBUTTONDOWN, 1, x + y * 65536);
		SendMessage(hWnd, WM_LBUTTONUP, 0, x + y * 65536);
	}
	///<summary>��굥����(�ƶ���x,y,�ȴ�10ms,����)</summary><param name = "hWnd">���ھ��</param><param name = "x">X</param><param name = "y">Y</param>
	void mouseClick2(int x, int y)
	{
		SetCursorPos(x, y);
		tim::wait(10);
		mouse_event(2, 0, 0, 0, 0);
		mouse_event(4, 0, 0, 0, 0);
	}
}
