/*
 Copyright 2003, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#ifndef WIN32WINDOW
#define WIN32WINDOW

#include <windows.h>
#include "common.h"
#include "Logger.h"

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Win32Window
{
private:
    WNDCLASSEX wClass;
	HWND hWnd;
	HINSTANCE hInstance;

	HICON hIcon;
	HICON hIconSmall;
	HCURSOR hCursor;

	Logger* Log;

	int NumMenus;
public:
	PIXEL WinCoord;
	PIXEL WinSize;

	MOUSE Mouse;

	HMENU* Menus;

	void InitWindow(Logger *Log);
	int CreateWin(char* Title,int SizeX,int SizeY,HINSTANCE hInstance,int cmdShow=SW_SHOW);

	void AddMenu();

	void InitCursor(HCURSOR hCursor) { this->hCursor=hCursor; };
	void InitIcon(HICON hIcon,HICON hIconSmall) { this->hIcon=hIcon; this->hIconSmall=hIconSmall; };

	void UpdateWindowDetails(int Value,WPARAM wParam,LPARAM lParam);

	int KillWin();

	HWND GetHWND() { return hWnd; };
	HINSTANCE GetHINSTANCE() { return hInstance; };

	void Error(char* title, UINT uType, const char* fmt, ...);
};

#endif