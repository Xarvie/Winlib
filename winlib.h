#ifndef WINLIB_H
#define WINLIB_H
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>

using namespace std;
#ifdef _UNICODE
typedef std::wstring tstring;
typedef wchar_t tchar;
extern tstring _TS(const std::wstring tmp);
extern tstring _TS(const std::string tmp);
extern tstring _TS(const wchar_t* tmp);
extern tstring _TS(const char* tmp);
#else
typedef std::string tstring;
typedef char tchar;
extern tstring _TS(const std::wstring tmp);
extern tstring _TS(const std::string tmp);
extern tstring _TS(const wchar_t* tmp);
extern tstring _TS(const char* tmp);
#endif

namespace  enc
{
extern string w2a(const wstring& wideString);
extern wstring a2w(const string& a);
}
namespace  cli
{
extern void setClipText(const tstring& strText);
extern tstring getClipText();
}
namespace  tim
{
extern void __doEvents();
extern int wait(UINT64 milliSecond);
class timer;
}
namespace  key
{
extern void keyClick(BYTE keyA, BYTE keyB);
extern void keyClick(BYTE keyA, BYTE keyB, BYTE keyC);
}
namespace  fil
{
extern tstring getProgramPath();
extern LONGLONG getFileSize(const tstring& fileName);
extern LONGLONG getFileCompressedSize(const tstring& fileName);
extern tstring findFile(const tstring& filterText);
}
namespace  win
{
extern HWND ActiveWindow(HWND hWnd);
extern BOOL focusWindow(HWND hWnd);
}
namespace  mou
{
extern void mouseClick(HWND hWnd, int x, int y);
extern void mouseClick2(int x, int y);
}
namespace  str
{
extern tstring antiBlank(const tchar text[]);
extern tstring antiBlank(const tstring& text);
}
namespace sys
{
extern tstring __formatPath(const tchar *perfData);
extern int __isPathExist(tchar *perfData, const tchar *myPath);
extern int addPath(const tstring& tcstrPath);
}

#ifdef _UNICODE
tstring _TS(const std::wstring& tmp)
{
	return wstring(tmp);
}
tstring _TS(const std::string& tmp)
{
	return enc::a2w(tmp);
}
tstring _TS(const wchar_t* tmp)
{
	return wstring(tmp);
}
tstring _TS(const char* tmp)
{
	return enc::a2w(string(tmp));
}
#else
tstring _TS(const std::wstring& tmp)
{
	return enc::w2a(tmp);
}
tstring _TS(const std::string& tmp)
{
	return string(tmp);
}
tstring _TS(const wchar_t* tmp)
{
	return enc::w2a(wstring(tmp));
}
tstring _TS(const char* tmp)
{
	return string(tmp);
}
#endif

//编码类 encode
namespace enc
{
///<summary>WideChar转ASCII。</summary><param name = "WideChar">WideChar文本。</param><returns>成功:返回文本，失败:string(“”)。</returns>
string w2a(const wstring& wideString)
{
	int iSize;
	string rt;
	iSize = WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, NULL, 0,
	NULL, NULL);
	char *tmp = new char[iSize]; //短字符串可优化
	rt.resize(iSize, 0);
	if (!WideCharToMultiByte(CP_ACP, 0, wideString.c_str(), -1, tmp, iSize,
	NULL, NULL))
	{
		return string("");
	}
	string ret = tmp;
	delete[] tmp;
	return string(ret);
}
///<summary>ASCII转WideChar。</summary><param name = "WideChar">ASCII文本。</param><returns>成功:返回文本，失败:wstring(“”)。</returns>
wstring a2w(const string& a)
{
	int iSize;
	iSize = MultiByteToWideChar(CP_ACP, 0, a.c_str(), -1, NULL, 0);
	wchar_t *tmp = new wchar_t[iSize]; //短字符串可优化
	if (!MultiByteToWideChar(CP_ACP, 0, a.c_str(), -1, tmp, iSize))
	{
		return wstring(L"");
	}
	wstring ret = tmp;
	delete[] tmp;
	return wstring(ret);
}
}
//剪切板  Clipboard
namespace cli
{
///<summary>置剪切板文本。</summary><param name = "strText">字符串指针。</param>
void setClipText(const tstring& strText)
{
	if (OpenClipboard(NULL))
	{
		HGLOBAL hmem = GlobalAlloc(GHND,
				(strText.length() + 1) * sizeof(strText[0]));
		tchar *pmem = (tchar*) GlobalLock(hmem);
		EmptyClipboard();
		memcpy(pmem, &strText[0], (strText.length() + 1) * sizeof(strText[0]));
		SetClipboardData(CF_UNICODETEXT, hmem);
		CloseClipboard();
		GlobalFree(hmem);
	}
}
///<summary>取剪切板文本。成功:返回文本，失败:NULL。</summary><returns>成功:返回文本，失败:NULL。</returns>
tstring getClipText()
{
	tstring rt=_T("");
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
		tchar* str = (tchar*) GlobalLock(hMem);
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
///<summary>处理事件。</summary>
void __doEvents()
{
	MSG msg;
	while (PeekMessage(&msg, (HWND) NULL, 0, 0, PM_REMOVE))
	{
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
	__doEvents();
	hTimer = CreateWaitableTimer(nullptr, FALSE, nullptr);
	if (NULL == hTimer)
	{
		return -1;
	}
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
	{
		return -1;
	}
	DWORD ret;
	while (ret = MsgWaitForMultipleObjects(1, &hTimer, FALSE, INFINITE,
			QS_ALLINPUT) != WAIT_OBJECT_0)
	{
		if (((WAIT_OBJECT_0 + 1) == ret) || (WAIT_TIMEOUT == ret))
		{
			__doEvents();
		}
		else
		{
			break;
		}
	}
	CancelWaitableTimer(hTimer);
	CloseHandle(hTimer);
	__doEvents();
	return 0;
}
//计时器类
class timer
{
public:
	HANDLE hTimerQueue = 0;
	HANDLE hTimer = 0;
	WAITORTIMERCALLBACK func;
	int timerCycle;
	///<summary>创建时钟。</summary><param name = "func">响应函数。</param><param name = "timerCycle">默认1000ms做一次函数响应。</param><returns>成功:0，失败:-1。</returns>
	int CreateTimer(WAITORTIMERCALLBACK func, int timerCycle = 1000)
	{
		if (hTimerQueue != 0 || hTimer != 0)
		{
			deleteTimer();
		}
		this->func = func;
		this->hTimerQueue = CreateTimerQueue();
		return (CreateTimerQueueTimer(&this->hTimer, hTimerQueue, func, 0, 0,
				timerCycle, WT_EXECUTEDEFAULT));
	}
	///<summary>删除时钟。</summary></param><returns>成功:非0，失败:0。</returns>
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
}
namespace fil
{
///<summary>取程序路径。成功:路径文本，失败:空文本。</summary>
tstring getProgramPath()
{
	tchar rt[MAX_PATH]={ 0 };
	GetModuleFileName(NULL, rt, MAX_PATH * sizeof(tchar));
	return _TS(rt);
}
///<summary>取文件尺寸。成功:尺寸 失败:-1。</summary><param name = "fileName">文件名</param>
LONGLONG getFileSize(const tstring& fileName)
{
	LARGE_INTEGER FileSize;
	HFILE hFile = _lopen(fileName.c_str(), 0);
	if (!GetFileSizeEx((HANDLE) hFile, &FileSize))
	{
		return -1;
	}
	_lclose(hFile);
	return FileSize.QuadPart;
}
///<summary>取文件压缩(实际)尺寸。成功:尺寸。</summary><param name = "fileName">文件名</param>
LONGLONG getFileCompressedSize(const tstring& fileName)
{
	ULARGE_INTEGER s;
	s.LowPart = GetCompressedFileSize(fileName.c_str(), &s.HighPart);
	return s.HighPart * 4294967296 + s.LowPart;
	/*
	 取实际上压缩后所占用磁盘空间,如果磁盘类型非压缩,实际效果等于GetFileSize,
	 MSDN:
	 If the file is not located on a volume that supports compression or sparse files,
	 or if the file is not compressed or a sparse file, the value obtained is the actual file size,
	 the same as the value returned by a call to GetFileSize.
	 */
}
///<summary>寻找文件或目录。成功:文件名 失败:空文本。</summary><param name = "filterText">匹配条件。例1：“./*.txt ”</param>
tstring findFile(const tstring& filterText)
{
	tstring rt=_T("");
	WIN32_FIND_DATA WFD;
	HANDLE hFind = FindFirstFile(filterText.c_str(), &WFD);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return rt;
	}
	rt = _TS(WFD.cFileName);
	::FindClose(hFind);
	return rt;
}
}
namespace win
{
///<summary>激活指定窗口。成功:激活的句柄 失败:NULL。</summary><param name = "hWnd">窗口句柄</param>
HWND ActiveWindow(HWND hWnd)
{
	DWORD idCurrent = GetCurrentThreadId();
	DWORD idTarget = GetWindowThreadProcessId(hWnd, 0);
	AttachThreadInput(idTarget, idCurrent, true);
	HWND rt = SetActiveWindow(hWnd);
	AttachThreadInput(idTarget, idCurrent, false);
	return rt;
}
///<summary>窗口前置。成功:激活的句柄 失败:NULL。</summary><param name = "hWnd">窗口句柄</param>
BOOL focusWindow(HWND hWnd)
{
	return SetForegroundWindow(hWnd);
}
}
namespace mou
{
///<summary>鼠标单击。</summary><param name = "hWnd">窗口句柄</param><param name = "x">X</param><param name = "y">Y</param>
void mouseClick(HWND hWnd, int x, int y)
{
	SendMessage(hWnd, WM_LBUTTONDOWN, 1, x + y * 65536);
	SendMessage(hWnd, WM_LBUTTONUP, 0, x + y * 65536);
}
///<summary>鼠标单击。(移动至x,y,等待10ms,单击)</summary><param name = "x">X</param><param name = "y">Y</param>
void mouseClick2(int x, int y)
{
	SetCursorPos(x, y);
	tim::wait(10);
	mouse_event(2, 0, 0, 0, 0);
	mouse_event(4, 0, 0, 0, 0);
}
}
//文本类 string
namespace str
{
///<summary>去首尾空格。返回:tstring型变量</summary><param name = "text">待格式化字符串</param>
tstring antiBlank(const tchar text[])
{
	tstring s(text);
	int i, j;
	for (i = 0; s[i] == ' '; i++)
		;
	for (j = 0; s[i];)
		s[j++] = s[i++];
	for (i--; s[i] == ' '; i--)
		s[i] = '\0';
	return s;
}
///<summary>去首尾空格。返回:tstring型变量</summary><param name = "text">待格式化字符串</param>
tstring antiBlank(const tstring& text)
{
	tstring s(text);
	int i, j;
	for (i = 0; s[i] == ' '; i++)
		;
	for (j = 0; s[i];)
		s[j++] = s[i++];
	for (i--; s[i] == ' '; i--)
		s[i] = '\0';
	return s;
}
}
//系统类 system
namespace sys
{
tstring __formatPath(const tchar *perfData)
{
	tstring allPath = (tchar *) perfData;
	int len = _tcslen(perfData);
	int blankflag = 1;
	tstring out1 = _T(""), out2 = _T("");
	if (len == 0)
		return out2;
	tchar token = 0;
	for (int i = 0; i < len; i++)
	{
		token = perfData[i];
		if (token == ' ' && blankflag == 1)
		{
			while (i + 1 < len && perfData[i + 1] == ' ')
				i++;
			if (i + 1 == len)
				return out2;
			blankflag = 0;
			continue;
		}
		if (token == '\\')
		{
			while (i + 1 < len && perfData[i + 1] == ' ')
				i++;
		}
		out1 += token;
	}
	blankflag = 1;
	len = out1.length();
	if (len == 0)
		return out2;
	for (int i = len - 1; i >= 0; i--)
	{
		token = out1[i];
		if (token == ' ' && blankflag == 1)
		{
			while (i - 1 >= 0 && out1[i - 1] == ' ')
				--i;
			if (i - 1 < 0)
				return out2;
			blankflag = 0;
			continue;
		}
		if (token == '\\')
		{
			while (i - 1 >= 0 && out1[i - 1] == ' ')
				i--;
		}
		out2 = token + out2;
	}
	return out2;
}
int __isPathExist(tchar *perfData, const tchar *myPath)
{
	tstring allPath = __formatPath(perfData);
	tstring newPath = __formatPath(myPath);
	int pathLen = 0;
	if (allPath.length() == 0)
		return 0;
	if ((pathLen = newPath.length()) == 0)
		return 1;
	int tick = -1;
	while (-1 != (tick = allPath.find(newPath, tick + 1)))
	{
		if (allPath[tick + pathLen] == '\0' || allPath[tick + pathLen] == ';')
			return 1;
		if ((tick + pathLen - 1 >= 0 && allPath[tick + pathLen] == '\\')
				&& (allPath[tick + pathLen + 1] == '\0'
						|| allPath[tick + pathLen + 1] == ';'))
			return 1;
	}
	return 0;
}
///<summary>添加用户环境变量。成功返回:0 失败:-1,路径末尾不用加反斜杠,自动去多余空格,检测是否已存在</summary><param name = "strPath">待添加的路径</param>
int addPath(const tstring& tcstrPath)
{
	tstring strPath = _T(";") + __formatPath(tcstrPath.c_str());
	int ret = 0;
	HKEY hkResult;
	LPCTSTR environmentKey = _T(
			"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
	if (ERROR_SUCCESS
			!= (ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, environmentKey, 0,
					KEY_SET_VALUE | KEY_READ, &hkResult)))
		return ret;
	DWORD BufferSize = 8192;
	DWORD cbData;
	DWORD dwRet;
	VOID *PerfData = malloc(BufferSize);
	cbData = BufferSize;
	LPCTSTR myVariable = _T("PATH");
	dwRet = RegQueryValueEx(hkResult, myVariable, NULL, NULL, (LPBYTE) PerfData,
			&cbData);
	while (dwRet == ERROR_MORE_DATA)
	{
		BufferSize += 4096;
		PerfData = realloc(PerfData, BufferSize);
		cbData = BufferSize;
		dwRet = RegQueryValueEx(hkResult, myVariable, NULL, NULL,
				(LPBYTE) PerfData, &cbData);
	}
	if (dwRet == ERROR_SUCCESS)
	{
		if (__isPathExist((tchar *) PerfData, strPath.c_str()))
		{
			RegCloseKey(hkResult);
			free(PerfData);
			return 1;
		}
		LPTSTR myValueEx = new TCHAR[strPath.length() + 1];
		_tcscpy((tchar *) myValueEx, strPath.c_str());
		TCHAR lastChar = ((LPTSTR) PerfData)[cbData - 2];
		if (lastChar != ';')
		{
			_tcscpy((tchar *) myValueEx, _T(";"));
			_tcscpy((tchar *) myValueEx, strPath.c_str());
		}
		tstring strMyValueEx;
		strMyValueEx = myValueEx;
		TCHAR *temp = new TCHAR[cbData + strlen((char *) myValueEx)];
		_tcscpy(temp, (tchar *) PerfData);
		LPTSTR pathValue = _tcscat(temp, strMyValueEx.c_str());
		long setResult = RegSetValueEx(hkResult, myVariable, 0, REG_EXPAND_SZ,
				(LPBYTE) pathValue, _tcslen(pathValue) * sizeof(TCHAR));
		DWORD dwResult;
		SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
				LPARAM(_T("Environment")), SMTO_ABORTIFHUNG, 5000, &dwResult);
		delete[] myValueEx;
		delete[] temp;
		temp = NULL;
		if (ERROR_SUCCESS == setResult)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	RegCloseKey(hkResult);
	free(PerfData);
	PerfData = NULL;
	return ret;
}
}

#endif //namespace end
