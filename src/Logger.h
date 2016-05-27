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

#ifndef LOGGER
#define LOGGER

#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include "common.h"

class Logger
{
private:
	char* FileName;
public:
	bool LoggingIsOn;

	int InitLog(char *filename);
	int WriteToLog(const char* text, ...);

	void Error(char* title, UINT uType, const char* fmt, ...);
};

#endif