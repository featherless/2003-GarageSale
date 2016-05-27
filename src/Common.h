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

#ifndef RETURNS
#define RETURNS

#include <windows.h>

#define ERROR			0
#define OK				1

#define ERRORDEFAULT	MB_OK | MB_ICONEXCLAMATION

#endif

#ifndef STRUCTS
#define STRUCTS

struct PIXEL
{
	int X,Y;
};

struct MOUSE
{
	int X,Y;
	bool Left,Right,Middle;
	bool hLeft,hRight,hMiddle;
};

#endif

#ifndef MACROS
#define MACROS

#define ISPOW2(a) ((a) & ((a)-1))

#endif